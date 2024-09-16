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
#include <atomic>
#include <thread>
#include <assert.h>

using namespace std;

atomic<bool> x, y;
int z;

void write_x_then_y()
{
	x.store(true, memory_order_seq_cst);
	y.store(true, memory_order_seq_cst);
}

void read_x_then_y()
{
	while (!x.load(memory_order_seq_cst)); // Wait for x to be true
	if (y.load(memory_order_seq_cst)) { // Then check y
		++z;
	}
}

void read_y_then_x()
{
	while (!y.load(memory_order_seq_cst)); // Wait for y to be true
	if (x.load(memory_order_seq_cst)) { // Then check x
		++z;
	}
}

int main()
{
	x = false;
	y = false;
	z = 0;

	thread a(write_x_then_y);
	thread b(read_x_then_y);
	thread c(read_y_then_x);

	a.join();
	b.join();
	c.join();

	assert(z == 2); // This will always pass
}
