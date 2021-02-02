/*
	This file is part of ReGlob.

	Copyright (C) 2021 Yukino Song <yukino@sudomaker.com>
	Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the MIT License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "ReGlob.hpp"
#include <cstdio>

std::string SudoMaker::ReGlob_String(const std::string &glob, SudoMaker::reglob_config config) {
	enum domains {
		DOMAIN_NONE = 0, DOMAIN_SQUARE_BRACKET = 1, DOMAIN_BRACES = 2
	};

	std::string regexp_str;

	bool escaped = false;

	std::vector<uint8_t> domain(1, DOMAIN_NONE);

	char c;
	uint8_t current_domain;
	for (size_t i = 0, len = glob.size(); i < len; i++) {
		c = glob[i];

		current_domain = domain.back();

		switch (c) {
			case '\\':
				if (escaped) {
					escaped = false;
					regexp_str += "\\\\";
				} else {
					escaped = true;
				}
				break;
			case '$':
			case '^':
			case '+':
			case '.':
			case '(':
			case ')':
			case '=':
			case '!':
			case '|':
				if (!escaped) {
					regexp_str += '\\';
				}
				regexp_str += c;
				escaped = false;
				break;
			case '?':
				if (!escaped && config.bash_syntax) {
					if (config.capture) {
						regexp_str += "(.)";
					} else {
						regexp_str += '.';
					}
				} else {
					regexp_str += c;
				}
				escaped = false;
				break;

			case '[':
				if (!escaped) {
					if (config.bash_syntax) {
						if (config.capture) {
							if (std::find(domain.begin(), domain.end(), DOMAIN_SQUARE_BRACKET) != domain.end()) {
								throw reglob_error("ReGlob: Cannot group square bracket inside square brackets.");
							}
							regexp_str += '(';
							domain.push_back(DOMAIN_SQUARE_BRACKET);
						}
					} else {
						regexp_str += "\\";
					}
				}
				regexp_str += c;
				escaped = false;
				break;

			case ']':
				if (escaped) {
					regexp_str += c;
				} else {
					if (config.bash_syntax) {
						if (config.capture) {
							if (current_domain == DOMAIN_SQUARE_BRACKET) {
								regexp_str += "])";
								domain.pop_back();
							} else {
								regexp_str += "\\]";
							}
						} else {
							regexp_str += c;
						}
					} else {
						regexp_str += "\\]";
					}
				}
				escaped = false;
				break;

			case '{':
				if (!escaped) {
					if (!escaped && config.bash_syntax) {
						domain.push_back(DOMAIN_BRACES);
						regexp_str += '(';
						if (!config.capture) {
							regexp_str += "?:";
						}
						break;
					} else {
						regexp_str += '\\';
					}
				}
				regexp_str += c;
				escaped = false;
				break;

			case '}':
				if (!escaped) {
					if (!escaped && current_domain == DOMAIN_BRACES && config.bash_syntax) {
						domain.pop_back();
						regexp_str += ")";
						break;
					} else {
						regexp_str += '\\';
					}
				}
				regexp_str += c;
				escaped = false;
				break;

			case ',':
				if (!escaped && current_domain != DOMAIN_NONE) {
					if (current_domain == DOMAIN_BRACES) regexp_str += '|';
					else if (current_domain == DOMAIN_SQUARE_BRACKET) regexp_str += c;
				} else {
					regexp_str += "\\,";
				}
				escaped = false;
				break;

			case '*': {
				if (escaped) {
					regexp_str += c;
				} else {
					char prev_char = i ? glob[i - 1] : 0;

					size_t star_cnt = 1;
					while (glob[i + 1] == '*') {
						star_cnt++;
						i++;
					}
					char next_char = glob[i + 1];

					if (!config.globstars) {
						if (config.capture) {
							regexp_str += "(.*)";
						} else {
							regexp_str += ".*";
						}
					} else {
						if (star_cnt > 1 && (prev_char == '/' || !prev_char) && (next_char == '/' || !next_char)) {
							if (config.capture) {
								regexp_str += "((?:[^/]*(?:/|$))*)";
							} else {
								regexp_str += "(?:[^/]*(?:/|$))*";
							}
							i++;
						} else {
							if (config.capture) {
								regexp_str += "([^/]*)";
							} else {
								regexp_str += "[^/]*";
							}
						}
					}
				}
				escaped = false;
				break;
			}
			default:
				regexp_str += c;
				escaped = false;
		}
	}

	current_domain = domain.back();

	if (escaped) {
		throw reglob_error("ReGlob: Escape at the end of line.");
	}
	if (current_domain != DOMAIN_NONE) {
//			printf("Current domain: %d\n", current_domain);
		std::string error_msg = "ReGlob: unmatched " + std::string(current_domain == DOMAIN_SQUARE_BRACKET ? "]" : "}") + " in glob expression";
		throw reglob_error(error_msg);
	}

	if (config.full_match) {
		regexp_str = "^" + regexp_str + "$";
	}

	return regexp_str;
}

std::regex SudoMaker::ReGlob(const std::string &glob, SudoMaker::reglob_config config) {
	auto regexp_str = ReGlob_String(glob, config);

	std::regex_constants::syntax_option_type regex_type = std::regex::ECMAScript | std::regex::optimize;

	if (config.ignore_case) {
		regex_type = regex_type | std::regex::icase;
	}

	return std::regex(regexp_str, regex_type);
}

std::function<std::unordered_map<std::string, std::string>(std::string)> SudoMaker::ReGlob_Path(std::string path) {
	std::regex variable_pattern(":(.+?)(-|(?=/|:|$))");

	std::string glob_string = std::regex_replace(path, variable_pattern, "*");

	std::smatch m;
	std::vector<std::string> variable_defs;

	while (std::regex_search(path, m, variable_pattern)) {
		variable_defs.emplace_back(m[1]);
		path = m.suffix();
	}

	std::regex glob_pattern = ReGlob(glob_string, {.capture = true});

	return [variable_defs, glob_pattern](std::string incoming_path) {
		std::smatch matched_variables;
		std::regex_search(incoming_path, matched_variables, glob_pattern);

		std::unordered_map<std::string, std::string> result;

		for (size_t i = 1; i < matched_variables.size(); i++) {
			result.insert({variable_defs[i - 1], matched_variables[i]});
		}

		return result;
	};
}
