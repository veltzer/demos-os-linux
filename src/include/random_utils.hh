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

#ifndef __random_utils_hh
#define __random_utils_hh

/*
 * This is a collection of utils to help with randomization issues.
 * Currently it only includes a small random number generator from
 * the article referenced below.
 *
 * References:
 * http://preshing.com/20120515/memory-reordering-caught-in-the-act
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>

// -------------------------------------
// MersenneTwister
// A thread-safe random number generator with good randomness
// in a small number of instructions. We'll use it to introduce
// random timing delays.
// -------------------------------------
#define MT_IA 397
#define MT_LEN 624

class MersenneTwister {
	unsigned int m_buffer[MT_LEN];
	int m_index;

public:
	MersenneTwister(unsigned int seed);
	// Declare noinline so that the function call acts as a compiler barrier:
	unsigned int integer() __attribute__((noinline));
};

MersenneTwister::MersenneTwister(unsigned int seed) {
	// Initialize by filling with the seed, then iterating
	// the algorithm a bunch of times to shuffle things up.
	for(int i = 0; i < MT_LEN; i++)
		m_buffer[i] = seed;
	m_index = 0;
	for(int i = 0; i < MT_LEN * 100; i++)
		integer();
}

unsigned int MersenneTwister::integer() {
	// Indices
	int i = m_index;
	int i2 = m_index + 1; if (i2 >= MT_LEN) i2 = 0;	// wrap-around
	int j = m_index + MT_IA; if (j >= MT_LEN) j -= MT_LEN;	// wrap-around

	// Twist
	unsigned int s = (m_buffer[i] & 0x80000000) | (m_buffer[i2] & 0x7fffffff);
	unsigned int r = m_buffer[j] ^ (s >> 1) ^ ((s & 1) * 0x9908B0DF);
	m_buffer[m_index] = r;
	m_index = i2;

	// Swizzle
	r ^= (r >> 11);
	r ^= (r << 7) & 0x9d2c5680UL;
	r ^= (r << 15) & 0xefc60000UL;
	r ^= (r >> 18);
	return r;
}

#endif	/* !__random_utils_hh */
