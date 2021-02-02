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

using namespace SudoMaker;

int main(int argc, char **argv) {
	if (argv[3]) {
		auto path_matcher = ReGlob_Path(argv[1]);
		auto result = path_matcher(argv[2]);
		for (auto &it:result) {
			std::cout << it.first << ": " << it.second << "\n";
		}
	} else if (argv[2]) {
		auto regexp_str = ReGlob_String(argv[1], {
			.ignore_case = false,
			.bash_syntax = true,
			.capture = false,
		});

		std::cout << "Generated regexp str: " << regexp_str << "\n";

		std::regex regexp = ReGlob(argv[1], {
			.ignore_case = false,
			.bash_syntax = true,
			.capture = false,
		});

		std::smatch m;
		std::string str = argv[2];
		std::regex_search(str, m, regexp);

		for (size_t i=0; i<m.size(); i++) {
			std::cout << i << ": " << m[i] << "\n";
		}
	} else {
		puts("Usage: ReGlob_Test <glob expression> <string>");
	}

	return 0;
}