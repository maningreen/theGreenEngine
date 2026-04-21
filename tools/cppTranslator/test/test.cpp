#include "test.hpp"

#include <iostream>

using namespace Foo;

void Test::testingFunction(float delta) {
    std::cout << delta << ' ' << testingChar << '\n';
}

Test::Test() {
    std::cout << "Wooo\n";
}

Test::Test(int i) {
    std::cout << "test alt\n";
}

void Test::test() const {
    std::cout << "SD:LFKJDSF\n";
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
