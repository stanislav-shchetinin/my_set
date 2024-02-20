#include <iostream>
#include "set.h"
int main() {

    ads::set<int> a;
    a.insert(6);
    //a.erase(6);
    auto t = a.find(6);
    if (t == a.end()) {

    } else {
        std::cout << *t << std::endl;
    }

    return 0;
}
