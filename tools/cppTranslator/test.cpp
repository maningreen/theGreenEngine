#include <iostream>

class Test {
  public:
    virtual void testingFunction(float delta) {}

    int testingItem;
    int* testingptr;
    char testingChar;

    Test() {
        std::cout << "Init test\n";
        testingItem = 3;
        testingptr = nullptr;
        testingItem = 4;
        testingChar = 'a';
    }
    virtual ~Test() {}
    void test() const { std::cout << "WOOO\n"; }
};

class Test2 : public Test {
    virtual void test3(float delta) {}
    Test2() {}
    ~Test2() {}
};

float sum(float a, float b) {
    return a + b;
}
