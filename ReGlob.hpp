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

#pragma once

#include <string>
#include <regex>
#include <iostream>

namespace SudoMaker {
	struct reglob_config {
		bool ignore_case = false;	// Ignore case
		bool bash_syntax = false;	// Use bash-like glob syntax (?[abc][0-9][a-z]), otherwise only asterisks (*) are in effect
		bool full_match = false;	// Match entire string
		bool globstars = true;		// Use globstars rules (https://github.com/isaacs/node-glob#glob-primer)
		bool capture = false;		// Capture wildcards into groups
	};

	class reglob_error : public std::exception {
	private:
		std::string str_;

	public:
		reglob_error(std::string str) {
			str_ = std::move(str);
		}

		const char *what() const noexcept override {
			return str_.c_str();
		}

	};

	extern std::regex ReGlob(const std::string& glob, reglob_config config);
	extern std::string ReGlob_String(const std::string& glob, reglob_config config);
}