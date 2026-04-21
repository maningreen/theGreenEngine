#include "test.hpp"

#include <iostream>

using namespace Foo;

template <typename T>
void Test<T>::testingFunction(float delta) {
    std::cout << delta << ' ' << testingChar << '\n';
}

template <typename T>
Test<T>::Test() {
    std::cout << "Wooo\n";
}

template <typename T>
Test<T>::Test(int i) {
    std::cout << "test alt\n";
}

template <typename T>
void Test<T>::test() const {
    std::cout << "SD:LFKJDSF\n";
}

template <typename T>
Test<T>::~Test() {}

void Test2::testingFunction(float delta) {
    std::cout << delta << '\n';
}

Test2::Test2() {}
Test2::~Test2() {}

float sum(float a, float b) {
    return a + b;
}

using TestInt = Test<int>;
