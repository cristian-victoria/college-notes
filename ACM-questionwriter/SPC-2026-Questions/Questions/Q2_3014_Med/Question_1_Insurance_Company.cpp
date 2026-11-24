#include <vector>
#include <string>
#include <cstdio>
#include <random>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <algorithm>

static std::vector<int> fusing_numbers(std::vector<int> whole_list, int size, int pair_size) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> range(48, 90);
	std::string temp_string;
	int temp_convert = 0;
	
	std::vector<int> temp_pairs(pair_size);

	whole_list.resize(size);

	for (int i = 0; i < size; i++) {
		
		for (int j = 0; j < pair_size; j++) {
			temp_pairs[j] = range(gen);
			temp_string += std::to_string(temp_pairs[j]);
		}
		temp_convert = std::stoi(temp_string);
		whole_list[i] = temp_convert;
		temp_string = " ";
	}

	return whole_list;
}

static void file_assignment(std::vector<int>& arr2, int size) {
	
	std::vector<std::string> even;
	std::vector<int> temp;
	std::vector<std::string> temp_chars;
	int length_of_string = 0;
	std::string pairs_substrs;
	std::vector<std::string> pairs;

	even.resize(size);
	pairs.resize(size);
	temp.resize(size);
	temp_chars.resize(size);

	for (int i = 0; i < size; i++) {

		// add another vector for every two letters to separate them in characters, find their ASCII codes
		// and match them with the 4-digit number given from the random generator. When generating the characters, 
		// take an even number of ASCII character codes, and exclude 1.

		even[i] = std::to_string(arr2[i]);
		length_of_string = even[i].length();

		int j = 0;

		while (j < length_of_string) {

			pairs_substrs = even[i].substr(j, 2);
			j = j + 2;
			temp[j] = std::stoi(pairs_substrs);
			temp_chars[j] += static_cast<char>(temp[j]);

			std::cout << temp_chars[j];
		}
		std::cout << '\n';
	}
}

static std::vector<int>& bubble_sort(std::vector<int>& arr, int size) {

	for (int i = 0; i < size - 1; ++i) {
		for (long long j = 0; j < size - i - 1; ++j) {
			if (arr[j] < arr[j + 1]) {
				int temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}

	return arr;
}

int main() {

	int n = 0; 
	int pair_n = 0;
	std::vector<int> file_list_pairs;
	std::vector<int> whole_file_number_list;
	std::vector<std::string> file_label;
	std::vector<std::string> temp_pairs;
	
	std::cout << "Enter list size: ";
	std::cin >> n;

	// Function call

	while (n <= 0 || n >= 100) {
		std::cout << "Try again... ";
		std::cin.clear();
		std::cin >> n;
	}

	std::cout << "Enter number size in pairs (no more than 4 since it can only take 8 bytes): ";
	std::cin >> pair_n;

	while (pair_n >= 4) {
		std::cout << "The program crashes when you input more than 4 in size, try again:" << '\n';
		std::cin >> pair_n;
	}

	file_list_pairs.resize(n);
	whole_file_number_list.resize(n);
	whole_file_number_list = fusing_numbers(file_list_pairs, n, pair_n);
	whole_file_number_list = bubble_sort(whole_file_number_list, n);
	std::cout << "These are for character assigning: " << '\n';
	file_assignment(whole_file_number_list, n);
}
		for (int i = 0; i < n; i++) {
			std::cout << final_file_list[i] << '\n';
		}
	}
}

