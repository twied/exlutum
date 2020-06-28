// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020 Tim Wiederhake

#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>

std::map<std::string, std::string> macro_definitions;

static bool is_id_start(int c) {
	return c >= 'a' && c <= 'z';
}

static bool is_id_cont(int c) {
	return is_id_start(c) || (c >= '0' && c <= '9') || (c == '_');
}

static bool is_space(int c) {
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static void comment(int c) {
	std::putchar(c);

	for (;;) {
		c = std::getchar();

		if (c < 0) {
			break;
		}

		std::putchar(c);

		if ((c == '\n') || (c == '\r')) {
			break;
		}
	}
}

static void label_definition(int c) {
	std::putchar(c);

	for(;;) {
		c = std::getchar();

		if (c < 0) {
			break;
		}

		std::putchar(c);

		if (c == ':') {
			break;
		}
	}
}

static void macro_definition(int c) {
	c = std::getchar();
	if (!is_id_start(c)) {
		fprintf(
			stderr,
			"Error: Macro starts with invalid character"
				" '%c' (%i)\n",
			c,
			c);
		std::exit(1);
	}

	std::string name {static_cast<char>(c)};

	for(;;) {
		c = std::getchar();
		if (!is_id_cont(c)) {
			break;
		}

		name += c;
	}

	while (is_space(c)) {
		c = std::getchar();
	}

	if (c != ':') {
		fprintf(
			stderr,
			"Error: Expected ':' after definition"
				" of macro \"%s\"\n",
			name.c_str());
		std::exit(1);
	}

	c = std::getchar();
	while (is_space(c)) {
		c = std::getchar();
	}

	if (c != '"') {
		fprintf(
			stderr,
			"Error: Expected '\"' after definition"
				" of macro \"%s\"\n",
			name.c_str());
		std::exit(1);
	}

	std::string value {};
	for(;;) {
		c = std::getchar();
		if (c < 0) {
			fprintf(
				stderr,
				"Error: Unexpected end of file in definition"
					" of macro \"%s\"\n",
				name.c_str());
			std::exit(1);
		}

		if (c == '"') {
			break;
		}

		value += c;
	}

	macro_definitions[name] = value;
}

static void identifier(int c) {
	std::string name {static_cast<char>(c)};

	for(;;) {
		c = std::getchar();
		if (!is_id_cont(c)) {
			break;
		}

		name += c;
	}

	const auto it = macro_definitions.find(name);
	if (it != macro_definitions.end()) {
		printf("%s", it->second.c_str());
	} else {
		printf("%s", name.c_str());
	}

	if (c < 0) {
		return;
	}

	if (!is_space(c)) {
		fprintf(
			stderr,
			"Error: Expected space after usage of"
				" label \"%s\"\n",
			name.c_str());
		std::exit(1);
	}

	printf(" ");
}

int main(void) {
	int c;

	for(;;) {
		c = std::getchar();

		if (c < 0) {
			break;
		}

		if (c == '#') {
			comment(c);
			continue;
		}

		if (c == '.') {
			label_definition(c);
			continue;
		}

		if (c == '%') {
			macro_definition(c);
			continue;
		}

		if (is_id_start(c)) {
			identifier(c);
			continue;
		}

		std::putchar(c);
	}
}
