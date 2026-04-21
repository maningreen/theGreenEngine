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
inline unsigned Test<T>::fizzbuzzFunc(int i) {
    const unsigned f = i / 5;
    const unsigned b = i / 5;
    return f + b;
}

template <typename T>
Test<T>::~Test() {}

void Test2::testingFunction(float delta) {
    std::cout << fizzbuzzFunc(delta)<< '\n';
    
}

Test2::Test2() {}
Test2::~Test2() {}

float sum(float a, float b) {
    return a + b;
}

Test<int> t;
