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
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * Demo that shows sizes of very small objects in C++
 *
 * Conclusions: (for gcc 4.7.3)
 * 1. Minimal object size is 1 (Empty proves it)
 * 2. Packing, for small objects can be less than 4 (Char and Short prove it)
 * 3. Packing, for larger objects is 4 byte aligned (CharIntChar proves it).
 * 4. You can have combinations of smaller than 4 byte packing and 4
 * byte aligned packing (CharChatInt proves it).
 */

class Empty {
};
class Char {
	char c;
};
class Short {
	short s;
};
class TwoChars {
	char c1;
	char c2;
};
class CharInt {
	char c;
	int i;
};
class IntChar {
	int i;
	char c;
};
class CharIntChar {
	char c1;
	int i;
	char c2;
};
class CharCharInt {
	char c1;
	char c2;
	int i;
};
class ShortInt {
	short s;
	int i;
};
class IntShort {
	int i;
	short s;
};

int main() {
	std::cout << "Empty: " << sizeof(Empty) << std::endl;
	std::cout << "Char: " << sizeof(Char) << std::endl;
	std::cout << "Short: " << sizeof(Short) << std::endl;
	std::cout << "TwoChars: " << sizeof(TwoChars) << std::endl;
	std::cout << "CharInt: " << sizeof(CharInt) << std::endl;
	std::cout << "IntChar: " << sizeof(IntChar) << std::endl;
	std::cout << "CharIntChar: " << sizeof(CharIntChar) << std::endl;
	std::cout << "CharCharInt: " << sizeof(CharCharInt) << std::endl;
	std::cout << "ShortInt: " << sizeof(ShortInt) << std::endl;
	std::cout << "IntShort: " << sizeof(IntShort) << std::endl;
	return EXIT_SUCCESS;
}
