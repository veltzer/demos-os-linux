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
#include <gtkmm/main.h>	// for Gtk::Main
#include <gtkmm/button.h>	// for Gtk::Button
#include <gtkmm/window.h>	// for Gtk::Window
#include <iostream>	// for std::cout, std::endl

/*
 * A stolen example from the gtkmm distribution. Slightly modified.
 *
 * EXTRA_COMPILE_CMD=pkg-config --cflags gtkmm-2.4 sigc++-2.0
 * EXTRA_LINK_CMD=pkg-config --libs gtkmm-2.4 sigc++-2.0
 * EXTRA_COMPILE_FLAGS=-Wno-deprecated-declarations
 */

class HelloWorld: public Gtk::Window {
public:
	HelloWorld(Gtk::Main&);
	virtual~HelloWorld();

protected:
	// Signal handlers:
	void on_button_clicked();
	// Member widgets:
	Gtk::Button m_button;
	// Application
	Gtk::Main& app;
};

HelloWorld::HelloWorld(Gtk::Main& iapp) :
	m_button("Exit"),
	app(iapp) {
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
	std::cout << "I was clicked" << std::endl;
	app.quit();
}

int main(int argc, char** argv) {
	Gtk::Main app(argc, argv);
	HelloWorld helloworld(app);
	app.run(helloworld);
	return EXIT_SUCCESS;
}
