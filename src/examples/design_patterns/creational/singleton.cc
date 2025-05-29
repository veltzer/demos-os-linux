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

using namespace std;

class GlobalClass {
public:
	 int getValue() {
		 return value_; }
	 void setValue( int v ) {
		 value_ = v; }
	 static GlobalClass* inst() {
		 if ( ! globalObj_ )
			 globalObj_ = new GlobalClass;
		 return globalObj_; }
 protected:
	 GlobalClass( int v=0 ) {
		 value_ = v; }
	 ~GlobalClass() { }
 private:
	 int	 value_;
	 static GlobalClass* globalObj_;
 };

 // Allocating and initializing
 // GlobalClass's static data member
 // (the ptr, not a GlobalClass inst)
 GlobalClass*
	 GlobalClass::globalObj_ = 0;

 void foo( void ) {
	 GlobalClass::inst()->setValue( 1 );
	 cout << "foo: globalObj is " <<
		 GlobalClass::inst()->getValue()
		 << endl;
 }
 void bar( void ) {
	 GlobalClass::inst()->setValue( 2 );
	 cout << "bar: globalObj is " <<
		 GlobalClass::inst()->getValue()
		 << endl;
 }
 int main() {
	 cout << "main: globalObj is " <<
		 GlobalClass::inst()->getValue()
		 << endl;
	 foo();
	 bar();
	 return 0;
}
