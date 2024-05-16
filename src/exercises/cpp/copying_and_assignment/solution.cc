/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>

class string
{
public:
	string():m_data(nullptr) {}

	string(const char* str)
	{
		if (str)
		{
			size_t str_len = strlen(str);
			m_data = new char[str_len + 1];
			strncpy(m_data, str, str_len + 1);
		}
		else
		{
			m_data = nullptr;
		}
	}

	string(const string& other)
	{
		if (other.m_data)
		{
			size_t other_len = strlen(other.m_data);
			m_data = new char[other_len + 1];
			strncpy(m_data, other.m_data, other_len + 1);
		}
		else
		{
			m_data = nullptr;
		}
	}

	~string()
	{
		if(m_data)
			delete[] m_data;
	}

	string operator=(const string& other)
	{
		if (this != &other)
		{
			delete[] m_data;
			size_t other_len = strlen(other.m_data);
			m_data = new char[other_len + 1];

			strncpy(m_data, other.m_data, other_len + 1);
		}

		return *this;
	}

	string operator=(const char* other)
	{
		delete[] m_data;
		if (other)
		{
			size_t other_len = strlen(other);
			m_data = new char[other_len + 1];

			strncpy(m_data, other, other_len + 1);
		} else {
			m_data=nullptr;
		}

		return *this;
	}

	string operator+(const string& os) const
	{
		size_t this_len = strlen(m_data);
		size_t other_len = strlen(os.m_data);
		char* new_data = new char[this_len + other_len + 1];
		strncpy(new_data, m_data, this_len+1);
		strncat(new_data, os.m_data, this_len + other_len + 1);

		return string(new_data);
	}

	void print_string()
	{
		printf("%s\n", m_data);
	}

private:
	char* m_data;
};

int main(int argc, char** argv, char** envp) {
	string s1("Hello, ");
	string s2("world!");

	s2 = s1;

	s2.print_string();

	string result = s1 + s2;

	result.print_string();

	return EXIT_SUCCESS;
}
