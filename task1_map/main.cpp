#include <iostream>
#include "Treap.tpp"

int main() {
    SearchingTree<int, int> map;
    map.insert(1, 1);
    map.insert(2, 2);
    map.insert(3, 6);
    map.insert(4, 24);
    for(const auto &[k, v] : map)
    {
        std::cout << k << " " << v << std::endl;
    }
    std::cout << std::endl;

    auto p = map.range(1, 3);
    std::cout << (*p.first).first << " " << (*p.first).second << std::endl;
    std::cout << (*p.second).first << " " << (*p.second).second << std::endl;
    std::cout << std::endl;

    for (const auto &[k, v] : map.range(1, 3)) {
        std::cout << k << " " << v << std::endl;
    }

    return 0;
}
