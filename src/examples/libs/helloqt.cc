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
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qpushbutton.h>

/*
 * This is a simple Qt hello world program
 *
 * We need to add -fPIC to the compilation
 * see https://stackoverflow.com/questions/47958083/how-to-build-qt-with-reduce-relocations
 * These flags are for Qt6 which is on 22.10
 * XTRA_COMPILE_FLAGS=-fPIC
 * XTRA_COMPILE_CMD=pkg-config --cflags Qt6Widgets
 * XTRA_LINK_CMD=pkg-config --libs Qt6Widgets
 *
 * These flags are for Qt5 which is on 22.04
 * EXTRA_COMPILE_FLAGS=-fPIC
 * EXTRA_COMPILE_CMD=pkg-config --cflags Qt5Widgets
 * EXTRA_LINK_CMD=pkg-config --libs Qt5Widgets
 */

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	QPushButton hello("Push me to exit", 0);

	QObject::connect(&hello, SIGNAL(clicked()), &app, SLOT(quit()));
	hello.show();
	return app.exec();
}
