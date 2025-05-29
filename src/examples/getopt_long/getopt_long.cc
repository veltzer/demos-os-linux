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
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS
#include <unistd.h>	// for getopt_long(3)
#include <getopt.h>	// for struct option

/*
 * This example demos most of the features of getopt_long. Originally
 * stolen from the getopt_long documentation.
 */

int main(int argc, char** argv) {
	int c;
	int digit_optind=0;
	while(true) {
		int this_option_optind=optind ? optind:1;
		int option_index=0;
		static struct option long_options[]={
			{"add", required_argument, 0, 0},
			{"append", no_argument, 0, 0},
			{"delete", required_argument, 0, 0},
			{"verbose", no_argument, 0, 0},
			{"create", required_argument, 0, 'c'},
			{"file", required_argument, 0, 0},
			{0, 0, 0, 0}
		};
		c=getopt_long(argc, argv, "abc:d:012", long_options, &option_index);
		if (c==-1)
			break;
		switch (c) {
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			break;
		case '0':
		case '1':
		case '2':
			if (digit_optind!=0 && digit_optind!=this_option_optind)
				printf("digits occur in two different argv-elements.\n");
			digit_optind=this_option_optind;
			printf("option %c\n", c);
			break;
		case 'a':
			printf("option a\n");
			break;
		case 'b':
			printf("option b\n");
			break;
		case 'c':
			printf("option c with value '%s'\n", optarg);
			break;
		case 'd':
			printf("option d with value '%s'\n", optarg);
			break;
		case '?':
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
			break;
		}
	}
	if (optind < argc) {
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}
	return EXIT_SUCCESS;
}
