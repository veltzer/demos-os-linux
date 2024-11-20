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
#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <random>
#include <chrono>

using namespace std;

void benchmark_sort_policies() {
	// Create a large vector with random numbers
	const size_t size = 10'000'000;
	vector<int> vec_seq(size);
	vector<int> vec_par(size);
	vector<int> vec_unseq(size);
	vector<int> vec_par_unseq(size);

	// Fill vectors with random numbers
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1, 1000000);

	for(size_t i = 0; i < size; ++i) {
		int value = dis(gen);
		vec_seq[i] = value;
		vec_par[i] = value;
		vec_unseq[i] = value;
		vec_par_unseq[i] = value;
	}

	// Helper lambda to time and sort with different policies
	auto time_sort = [](auto policy, vector<int>& vec, const string& policy_name) {
		auto start = chrono::high_resolution_clock::now();
		sort(policy, vec.begin(), vec.end());
		auto end = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
		cout << policy_name << " took " << duration.count() << "ms\n";
	};

	// Test different execution policies
	time_sort(execution::seq, vec_seq, "Sequential");
	time_sort(execution::par, vec_par, "Parallel");
	time_sort(execution::unseq, vec_unseq, "Unsequenced");
	time_sort(execution::par_unseq, vec_par_unseq, "Parallel+Unsequenced");
}

int main() {
	cout << "Benchmarking different execution policies for sorting...\n";
	benchmark_sort_policies();

	// Example of transform with different execution policies
	vector<int> numbers(1000, 1);

	// Sequential transform
	transform(execution::seq,
				numbers.begin(), numbers.end(),
				numbers.begin(),
				[](int n) { return n * 2; });

	// Parallel transform
	transform(execution::par,
				numbers.begin(), numbers.end(),
				numbers.begin(),
				[](int n) { return n * 3; });

	// Parallel unsequenced transform
	transform(execution::par_unseq,
				numbers.begin(), numbers.end(),
				numbers.begin(),
				[](int n) { return n * 4; });

	return 0;
}
