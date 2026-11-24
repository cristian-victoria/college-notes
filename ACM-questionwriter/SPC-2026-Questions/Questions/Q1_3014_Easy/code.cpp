#include <iostream>
#include <string>
using namespace std;

/*
22-Digit Tracking Numbers, wrote in reference to:
    https://postalpro.usps.com/pub199
    page 59 of the pdf, page #57

*/

int main() {
    /**
     * I plan on giving blocks of IDs and having the contestant output
     * whether the ID is true or false as they work through the block
     */
    string code{};
    while (getline(cin, code)) {
        int check_digit{code.back() - '0'};

        uint odd_sum{}, even_sum{};

        // Steps 2 -> 5:
        for (int j = code.length() - 2; j >= 0; --j) {
            uint digit{code[j] - '0'};

            if (j % 2 == 1) {
                odd_sum += digit;
            } else {
                even_sum += digit;
            }
        }

        uint total_sum{(odd_sum * 3) + even_sum};

        // Step 6: Finding the closest multiple to 10 with the given total
        // and seeing if the check_digit is valid
        cout << boolalpha;
        cout << (((10 - (total_sum % 10)) % 10) == check_digit) << endl;
    }

    return 0;
}
