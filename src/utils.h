#pragma once
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <iostream>

template<typename K, typename V>
std::vector<K> mapKeys(std::map<K, V> inputMap) {
    std::vector<K> keys;
    for (const std::pair<K, V> &pair : inputMap) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> splitString(std::string s, std::string separator) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, separator[0])) {
        result.push_back(token);
    }
    
    return result;
}

std::string joinString(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";

    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i != 0) {
            oss << delimiter;
        }
        oss << strings[i];
    }

    return oss.str();
}

template<typename T>
void printVector(const std::vector<T>& vec, std::string end = "\n") {
    std::cout << "{ ";
    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << "}" << end;
}

template<typename T>
void printVector(const std::vector<std::vector<T>>& vec, std::string end = "\n") {
    std::cout << "[";
    for (const auto& elem : vec) {
        printVector(elem, end="");
    }
    std::cout << "]" << "\n";
}
