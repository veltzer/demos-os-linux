/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <gtkmm/main.h>	// for Gtk::Main
#include <gtkmm/button.h>	// for Gtk::Button
#include <gtkmm/window.h>	// for Gtk::Window
#include <iostream>	// for std::cout, std::endl

/*
 * A stolen example from the gtkmm distribution. Slightly modified.
 *
 * EXTRA_COMPILE_CMDS=pkg-config --cflags gtkmm-2.4 sigc++-2.0 libgnomeui-2.0
 * EXTRA_LINK_CMDS=pkg-config --libs gtkmm-2.4 sigc++-2.0 libgnomeui-2.0
 */

class HelloWorld : public Gtk::Window {
public:
	HelloWorld();
	virtual ~HelloWorld();

protected:
	// Signal handlers:
	void on_button_clicked();
	// Member widgets:
	Gtk::Button m_button;
};

HelloWorld::HelloWorld() :
	m_button("Hello World")	// creates a new button with label "Hello World".
{
	// Sets the border width of the window.
	set_border_width(10);

	// When the button receives the "clicked" signal, it will call the
	// on_button_clicked() method defined below.
	m_button.signal_clicked().connect(sigc::mem_fun(*this, &HelloWorld::on_button_clicked));

	// This packs the button into the Window (a container).
	add(m_button);

	// The final step is to display this newly created widget...
	m_button.show();
}

HelloWorld::~HelloWorld() {
}

void HelloWorld::on_button_clicked() {
	std::cout << "Hello World" << std::endl;
}

int main(int argc, char** argv, char** envp) {
	Gtk::Main kit(argc, argv);
	HelloWorld helloworld;
	// Shows the window and returns when it is closed.
	Gtk::Main::run(helloworld);
	return EXIT_SUCCESS;
}
