/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2025 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <cstdlib>

using namespace std;

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

class Empty{
};
class Char{
	char c;
};
class Short{
	short s;
};
class TwoChars{
	char c1;
	char c2;
};
class CharInt{
	char c;
	int i;
};
class IntChar{
	int i;
	char c;
};
class CharIntChar{
	char c1;
	int i;
	char c2;
};
class CharCharInt{
	char c1;
	char c2;
	int i;
};
class ShortInt{
	short s;
	int i;
};
class IntShort{
	int i;
	short s;
};

int main() {
	cout << "Empty: " << sizeof(Empty) << endl;
	cout << "Char: " << sizeof(Char) << endl;
	cout << "Short: " << sizeof(Short) << endl;
	cout << "TwoChars: " << sizeof(TwoChars) << endl;
	cout << "CharInt: " << sizeof(CharInt) << endl;
	cout << "IntChar: " << sizeof(IntChar) << endl;
	cout << "CharIntChar: " << sizeof(CharIntChar) << endl;
	cout << "CharCharInt: " << sizeof(CharCharInt) << endl;
	cout << "ShortInt: " << sizeof(ShortInt) << endl;
	cout << "IntShort: " << sizeof(IntShort) << endl;
	return EXIT_SUCCESS;
}
