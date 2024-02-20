#include <iostream>
#include <random>
#include <set>
#include <vector>
#include "set.h"

#define COUNT_TESTS 50000
#define MAX_COUNT_REC 1000
#define MIN_COUNT_REC 10
#define MAX_REC 100
#define MIN_REC (-100)

std::mt19937 rnd(time(nullptr));

int main() {

    int test = 0;

    while (test != COUNT_TESTS) {

        ++test;

        ads::set<int> my_set;
        std::set<int> std_set;
        std::vector <std::pair <bool, int>> inp;

        int n = rnd() % MAX_COUNT_REC + MIN_COUNT_REC;

        while (n--) {
            bool is_insert = rnd() % 2;
            int rec = rnd() % MAX_REC + MIN_REC;
            if (is_insert) {
                my_set.insert(rec);
                std_set.insert(rec);
            } else {
                my_set.erase(rec);
                std_set.erase(rec);
            }
            inp.emplace_back(is_insert, rec);
        }

        bool is_success = true;

        auto fms = my_set.begin();
        auto fss = std_set.begin();

        while (fms != my_set.end() && fss != std_set.end()) {
            if (*fms != *fss) {
                is_success = false;
                break;
            }
            ++fms;
            ++fss;
        }

        if (is_success) {
            std::cout << "YES " << test << std::endl;
        } else {
            std::cout << "NO " << test << std::endl;
            for (auto &p : inp) {
                std::cout << p.first << " " << p.second << std::endl;
            }
            break;
        }

    }

    return 0;
}
