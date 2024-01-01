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
#include <unistd.h>	// for reboot(2), syscall(2)
#include <linux/reboot.h>	// for reboot(2)
#include <sys/syscall.h>// for syscall(2), SYS_xxx definitions
#include <err_utils.h>	// for CHECK_NOT_M1()

/*
 * Example of how to reboot the system
 * You need to have the right capabilities or you have to be running as root in order
 * to run this program. If you have the right permissions then this program will reboot
 * your machine, so you better watch out...
 */

int main(int argc, char** argv, char** envp) {
	CHECK_NOT_M1(syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, LINUX_REBOOT_CMD_RESTART, NULL));
	return EXIT_SUCCESS;
}
