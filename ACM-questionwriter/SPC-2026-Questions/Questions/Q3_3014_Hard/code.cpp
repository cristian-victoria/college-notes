#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <set>

struct Point {
    long long x, y;
};

long long getDistance(const Point one, const Point two) {
    return (one.x - two.x) * (one.x - two.x) + (one.y - two.y) * (one.y - two.y);
}

bool checkForSquare(const Point cornerOne, const Point cornerTwo, const Point cornerThree, const Point cornerFour) {
    long long d[6];
    d[0] = getDistance(cornerOne, cornerTwo); // side 1
    d[1] = getDistance(cornerTwo, cornerThree); // side 2
    d[2] = getDistance(cornerThree, cornerFour); // side 3
    d[3] = getDistance(cornerFour, cornerOne); // side 4
    d[4] = getDistance(cornerOne, cornerThree); // diag 1
    d[5] = getDistance(cornerTwo, cornerFour); // diag 2

    std::sort(d,d+6); // no way someone is actually writing a sorting algorithm

    const bool sides = d[0] > 0 && d[0] == d[1] && d[1] == d[2] && d[2] == d[3];
    const bool diagonal = d[4] == d[5] && d[0] + d[1] == d[4];
    return sides && diagonal;
}

int main() {
    int size = 0;
    std::cin >> size;
    std::vector<std::pair<Point,Point>> mappings;
    for (int i = 0; i < size; ++i) {
        std::pair<Point,Point> temp{};
        std::cin >> temp.first.x >> temp.first.y;
        std::cin >> temp.second.x >> temp.second.y;
        mappings.push_back(temp);
    }

    size = 0;
    std::cin >> size;
    std::vector<Point> coords;
    std::vector<Point> mappedcoords;
    for (int i = 0; i < size; ++i) {
        Point temp{};
        std::cin >> temp.x >> temp.y;
        coords.push_back(temp);
        mappedcoords.push_back(temp);
    }

    for(int i = 0; i < mappings.size(); i++) {
        for(int j = 0; j < coords.size(); j++) {
            bool negativeX = false;
            bool negativeY = false;
            if(coords.at(j).x < 0) {
                mappedcoords.at(j).x *= -1;
                negativeX = true;
            }
            if(coords.at(j).y < 0) {
                mappedcoords.at(j).y *= -1;
                negativeY = true;
            }
            if(mappedcoords.at(j).x >= mappings.at(i).first.x && mappedcoords.at(j).x <= mappings.at(i).first.y) {
                const long long offsetX = mappings.at(i).second.x - mappings.at(i).first.x;
                mappedcoords.at(j).x += offsetX;
            }
            if(mappedcoords.at(j).y >= mappings.at(i).first.x && mappedcoords.at(j).y <= mappings.at(i).first.y) {
                const long long offsetY = mappings.at(i).second.x - mappings.at(i).first.x;
                mappedcoords.at(j).y += offsetY;
            }
            if(negativeX) mappedcoords.at(j).x *= -1;
            if(negativeY) mappedcoords.at(j).y *= -1;
            // debug
            //std::cout << "Mapped Coordinate X: " << mappedcoords.at(j).x << " Mapped Coordinate Y: " << mappedcoords.at(j).y << "\n";
        }
    }

    std::vector<Point> output{};

    // O(N^4) ????????
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            for (int k = j + 1; k < size; ++k) {
                for (int l = k + 1; l < size; ++l) {
                    if (checkForSquare(mappedcoords[i], mappedcoords[j], mappedcoords[k], mappedcoords[l])) {
                        bool cornerOneUnique = true, cornerTwoUnique = true, cornerThreeUnique = true, cornerFourUnique = true;
                        for(int m = 0; m < output.size(); m++) {
                                if(output.at(m).x == coords[i].x && output.at(m).y == coords[i].y) cornerOneUnique = false;
                                if(output.at(m).x == coords[j].x && output.at(m).y == coords[j].y) cornerTwoUnique = false;
                                if(output.at(m).x == coords[k].x && output.at(m).y == coords[k].y) cornerThreeUnique = false;
                                if(output.at(m).x == coords[l].x && output.at(m).y == coords[l].y) cornerFourUnique = false;
                        }
                        if(cornerOneUnique) output.push_back(coords[i]);
                        if(cornerTwoUnique) output.push_back(coords[j]);
                        if(cornerThreeUnique) output.push_back(coords[k]);
                        if(cornerFourUnique) output.push_back(coords[l]);
                        // debug
                        // std::cout << "SQUARE FOUND WITH POINTS " << i << " " << j << " " << k << " " << l << std::endl;
                    }
                }
            }
        }
    }

    bool first = true;
    for(int i = 0; i < output.size(); ++i) {
        if(!first) std::cout << " ";
        std::cout << output.at(i).x << " " << output.at(i).y;
        first = false;
    }

    return 0;
}
