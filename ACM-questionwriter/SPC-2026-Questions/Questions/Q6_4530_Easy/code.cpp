#include <iostream>
#include <stack>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

// Struct to represent each tower, maintaining the current height, maximum height, and the blocks placed on it.
struct Tower {
    stack<int> blocks;
    long long currentHeight = 0;
    long long maxHeight = 0;
    long long currentStreak = 0;
    long long longestStreak = 0;

    // Function to place a block on the tower
    void place(int x) {
        blocks.push(x);
        currentHeight += x;
        maxHeight = max(maxHeight, currentHeight);
    }

    // Function to remove the top block from the tower
    void remove() {
        currentHeight -= blocks.top();
        blocks.pop();
    }

    void increase_streak(){
        currentStreak += 1;
        longestStreak = max(longestStreak, currentStreak);
    }

    void reset_streak(){
        currentStreak = 0;
    }
};

int main() {
    int N;
    cin >> N;   // N = number of commands

    map<char, Tower> towers;
    towers['K'];
    towers['D'];

    for (int i = 0; i < N; i++) {
        string cmd; // cmd = command type (PLACE or REMOVE)
        char t; // t = tower identifier (K or D)
        cin >> cmd >> t;    

        if (cmd == "PLACE") {
            int x;
            cin >> x;
            towers[t].place(x);
            towers[t].increase_streak();
        } else if (cmd == "REMOVE") {
            towers[t].remove();
            towers[t].reset_streak();
        }
    }

    cout << "Max Height K: " << towers['K'].maxHeight << endl;
    cout << "Max Height D: " << towers['D'].maxHeight << endl;
    cout << "Longest Streak K: " << towers['K'].longestStreak << endl;
    cout << "Longest Streak D: " << towers['D'].longestStreak << endl;

    return 0;
} 
