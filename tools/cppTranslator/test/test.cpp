#include "test.hpp"

#include <iostream>

void Test::testingFunction(float delta) {}

Test::Test() {
    std::cout << "Wooo\n";
}

Test::~Test() {}

void Test2::testingFunction(float delta) {
    std::cout << delta << '\n';
}

Test2::Test2() {}
Test2::~Test2() {}

float sum(float a, float b) {
    return a + b;
}
