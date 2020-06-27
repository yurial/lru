#include "lru.hpp"
#include <iostream>
#include <iterator>
#include <stdlib.h>

using lru_type = LRU<int, std::string>;

namespace std
{
std::ostream& operator << (std::ostream& os, const lru_type::value_type& val)
    {
    return os << val.first << '=' << val.second;
    }
}

int main()
    {
    lru_type container;
    container.emplace_front(1, "1");
    container.emplace_back(2, "2");
    container[3] = "3";
    container.try_emplace_front(4, "4");
    container.try_emplace_back(5, "5");
    const auto it = container.find(2);
    if (it == container.end())
        abort();
    it->second = "22";
    container.bump_back(it);
    std::cout << "empty=" << container.empty() << std::endl;
    std::cout << "size=" << container.size() << std::endl;
    std::cout << "max_size=" << container.max_size() << std::endl;
    std::cout << "count(2)=" << container.count(2) << std::endl;
    std::cout << "conatins(3)=" << container.contains(3) << std::endl;
    std::cout << "at(4)=" << container.at(4) << std::endl;
    std::copy(container.cbegin(), container.cend(), std::ostream_iterator<lru_type::value_type>(std::cout, "\n"));
    container.clear();
    return EXIT_SUCCESS;
    }
