// Cristian Victoria
// COP 4530 - Recitation 5

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <iomanip>

using namespace std;

// Function to check if single character is a letter
bool is_letter(char ch) {
    return isalpha(ch);
}

// Function to check if single character is a number
bool is_number(char ch) {
    return isdigit(ch);
}

// Function to convert a string to lowercase
void to_lowercase(string &word) {
    transform(word.begin(), word.end(), word.begin(), ::tolower);
}

// Class Template type T function to find and update or insert an element in the vector
template<typename T>
void increment_count(vector<pair<T, int>> &vec, const T &key) {
    // Loops over each element in vec
    for (auto &item : vec) {
        // If key matches with existing element in vec (item.first) then update counter (item.second)
        if (item.first == key) {
            item.second++;
            return;
        }
    }
    // If key is not found, insert a new vec pair
    vec.emplace_back(key, 1);
}

// Class Template type T function to print the top 10 items
template<typename T>
void print_top_10(const vector<pair<T, int>> &count_vec, const string &label) {
    vector<pair<T, int>> sorted_items = count_vec;

    // Sort by count descending, with ties broken by the value itself
    sort(sorted_items.begin(), sorted_items.end(), [](const auto &a, const auto &b) {
        // Sort in descending order
        // If two elements have the same count, sort by ascending order
        return (a.second > b.second) || (a.second == b.second && a.first < b.first);
    });

    cout << label << ":\n";
    int count = 0;
    for (const auto &[key, value] : sorted_items) {
        if (count == 10) break;
        cout << setw(10) << key << "  " << value << "\n";
        count++;
    }
}

int main() {
    // input will take in string line & token will take in substring of line
    string input, token;

    // Vector pairs with int to represent the count of each instance of string/char element
    vector<pair<string, int>> word_count;
    vector<pair<string, int>> number_count;
    vector<pair<char, int>> char_count;

    while (getline(cin, input)) {
        string current_token;
        bool is_in_word = false, is_in_number = false;

        for (char ch : input) {
            // Count characters in the line
            if (!isspace(ch)) { // Check if the characrer is not a whitespace
                increment_count(char_count, ch);
            }

            // If the character is a letter:
            if (is_letter(ch)) {
                // If it was processing a number then that number has ended
                if (is_in_number) { 
                    if (!current_token.empty()) {
                        // The current number is added to the count
                        increment_count(number_count, current_token);
                        current_token.clear();
                    }
                    // A word is now being constructed
                    is_in_number = false;
                }
                current_token += ch;
                is_in_word = true;
            }
            // If the character is a number:
            else if (is_number(ch)) {
                // If it was processing a word then that word has ended
                if (is_in_word) {
                    if (!current_token.empty()) {
                        to_lowercase(current_token);
                        // The current word is added to the count
                        increment_count(word_count, current_token);
                        current_token.clear();
                    }
                    // A number is now being constructed
                    is_in_word = false;
                }
                current_token += ch;
                is_in_number = true;
            }

            // If the character is not a letter nor a character:
            else {
                // If a word was being constructed, it is now completed
                if (is_in_word) {
                    to_lowercase(current_token);
                    increment_count(word_count, current_token);
                // If a number was being constructed, it is now completed
                } else if (is_in_number) {
                    increment_count(number_count, current_token);
                }
                is_in_word = is_in_number = false;
                current_token.clear();
            }
        }
        
        // After the string line, a word/number may still be unfinished
        if (is_in_word) {
            to_lowercase(current_token);
            increment_count(word_count, current_token);
        } else if (is_in_number) {
            increment_count(number_count, current_token);
        }
    }

    // Prints the results using the following labels
    print_top_10(char_count, "Top 10 Characters");
    print_top_10(word_count, "Top 10 Words");
    print_top_10(number_count, "Top 10 Numbers");

    return 0;
}

