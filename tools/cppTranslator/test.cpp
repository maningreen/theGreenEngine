#include <iostream>

class Test {
  public:
    virtual void testingFunction(float delta) {}

    int testingItem;
    int* testingptr;
    char testingChar;

    Test() : testingItem(3) {}
    virtual ~Test() {}
    void test() { std::cout << "WOOO\n"; }
};

class Test2 : public Test {
    virtual void test3(float delta) {}
    Test2() {}
    ~Test2() {}
};

float sum(float a, float b) {
    return a + b;
}
