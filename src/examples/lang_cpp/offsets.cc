/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <iostream>	// for std::cout, std::endl
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * watch these macros and see if you can figure out what they do...
 * we use '1' instead of '0' in these next lines because g++ is smart
 * enough to understand that using 0 will mean that we use the NULL
 * object and not let us do anything with it including taking fields
 * or doing pointer arithmetic...
 */

#define CppOffsetOf(className, FieldName) ((char *)(&(((className *)1)->FieldName)) - (char *)1)
#define CastOffsetOf(className, castName) ((char *)((castName *)((className *)1)) - (char *)((className *)1))

// alternativly we could have used the builtin __builtin_offsetof(field,struct)

/*
 * TODO:
 * - We still need to show how the mechanism of accessing a child field from an overridden
 * method in the parent works when there is multiple inheritance involved...
 */

class NameEmpty {
};
class NameOnechar {
	char c;
};
class NameTwochar {
	char c;
	char c2;
};
class NameOnevirt {
	virtual void doit(void)=0;
};
class NameTwovirt {
	virtual void doother(void)=0;
	virtual void dothat(void)=0;
};
class NameSimpleInherit : public NameOnevirt {
};
class NameSimpleInherit2 : public NameTwovirt {
};
class NameMultInherit : public NameOnevirt, public NameTwovirt {
};
class NameOneVirtOneField {
public:
	virtual void doit(void)=0;

	int x;
};
class NameOneVirtTwoField {
public:
	virtual void doother(void)=0;

	int m;
	int y;
};
class NameFMultInherit : public NameOneVirtOneField, public NameOneVirtTwoField {
public:
	int z;
};

int main(int argc, char** argv, char** envp) {
	std::cout << "Hello, World!" << std::endl;
	// this will print out 1 just because an object needs to take at least
	// one byte of ram...
	std::cout << "sizeof(NameEmpty) is " << sizeof(NameEmpty) << std::endl;
	// this yet again proves the previous point by showing that if we have just
	// one single character the previous character goes away
	std::cout << "sizeof(NameOnechar) is " << sizeof(NameOnechar) << std::endl;
	// this shows that two characters are packed (=2)
	std::cout << "sizeof(NameTwochar) is " << sizeof(NameTwochar) << std::endl;
	// this shows that having virtual functions adds the pointer to the vtable
	// which is 4 bytes
	std::cout << "sizeof(NameOnevirt) is " << sizeof(NameOnevirt) << std::endl;
	// this shows that having multiple virtual functions does NOT increase the
	// size of your object (you only hold a pointer to the vtable...)
	std::cout << "sizeof(NameTwovirt) is " << sizeof(NameTwovirt) << std::endl;
	// this shows that even inheritance from a class which has virtual functions
	// does not add size
	std::cout << "sizeof(NameSimpleInherit) is " << sizeof(NameSimpleInherit) << std::endl;
	// same for inheriting from a class that has more than one virtual function...
	std::cout << "sizeof(NameSimpleInherit2) is " << sizeof(NameSimpleInherit2) << std::endl;
	// this shows that a derived object from two virtual objects holds TWO virtual table
	// pointers
	std::cout << "sizeof(NameMultInherit) is " << sizeof(NameMultInherit) << std::endl;
	// this shows the use of the CppOffsetOf macro we defined above and that the first
	// field comes right after the vtable pointer...
	std::cout << "CppOffsetOf(NameOneVirtOneField,x) is " << CppOffsetOf(NameOneVirtOneField, x) << std::endl;
	// now lets see what happends of we have more than one field...
	std::cout << "CppOffsetOf(NameOneVirtTwoField,y) is " << CppOffsetOf(NameOneVirtTwoField, y) << std::endl;
	// the next prints prove that the object is laid out this way: vtable for parent 1, data for
	// parent 1, vtable for parent 2, data for parent 2, ...

	std::cout << "CppOffsetOf(NameFMultInherit,x) is " << CppOffsetOf(NameFMultInherit, x) << std::endl;
	std::cout << "CppOffsetOf(NameFMultInherit,m) is " << CppOffsetOf(NameFMultInherit, m) << std::endl;
	std::cout << "CppOffsetOf(NameFMultInherit,y) is " << CppOffsetOf(NameFMultInherit, y) << std::endl;
	std::cout << "CppOffsetOf(NameFMultInherit,z) is " << CppOffsetOf(NameFMultInherit, z) << std::endl;
	std::cout << "CastOffsetOf(NameFMultInherit,NameOneVirtOneField) is " << CastOffsetOf(NameFMultInherit, NameOneVirtOneField) << std::endl;
	std::cout << "CastOffsetOf(NameFMultInherit,NameOneVirtTwoField) is " << CastOffsetOf(NameFMultInherit, NameOneVirtTwoField) << std::endl;
	return EXIT_SUCCESS;
}
