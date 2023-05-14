/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2023 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <stdio.h>	// for printf(3)
#include <printf.h>	// for register_printf_function(3), struct printf_info
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This demo shows how to customize printf and is taken directly from the glibc info
 * pages.
 *
 * TODO:
 * - this example fails compilation if compiled as C++. It seems like the glibc
 * folk have not paid attention to making the printf.h file C++ safe. This does
 * not work even if you surround the "#include <printf.h>" statement with an
 * "extern "C"" declaration.
 * - I have to turn off warning in a lot of sections here because both the
 * 'register_printf_function' is deprecated and the compiler doesn't work
 * well with printf(3) extensions and so issues warnings about format strings
 * that don't match the arguments sent.
 */
typedef struct _Widget {
	const char *name;
} Widget;

int print_widget (FILE *stream, const struct printf_info *info, const void *const *args) {
	const Widget *w;
	char *buffer;
	int len;

	/* Format the output into a string. */
	w = *((const Widget **) (args[0]));
	len = asprintf (&buffer, "<Widget %p: %s>", (void*)w, w->name);
	if (len == -1)
		return -1;
	/* Pad to the minimum field width and print to the stream. */
	len = fprintf (stream, "%*s", (info->left ? -info->width : info->width), buffer);

	/* Clean up and return. */
	free (buffer);
	return len;
}

int print_widget_arginfo (const struct printf_info *info, size_t n, int *argtypes) {
	/* We always take exactly one argument and this is a pointer to the
	 * structure.. */
	if (n > 0)
		argtypes[0] = PA_POINTER;
	return 1;
}

int main(int argc, char** argv, char** envp) {
	/* Make a widget to print. */
	Widget mywidget;
	mywidget.name = "mywidget";

	/* Register the print function for widgets. */
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	register_printf_function ('W', print_widget, print_widget_arginfo);
	#pragma GCC diagnostic pop

	/* Now print the widget. */
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wformat"
	#pragma GCC diagnostic ignored "-Wformat-extra-args"
	printf ("|%W|\n", &mywidget);
	printf ("|%35W|\n", &mywidget);
	printf ("|%-35W|\n", &mywidget);
	#pragma GCC diagnostic pop
	return EXIT_SUCCESS;
}
