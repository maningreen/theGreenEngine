class Test {
  public:
    virtual void testingFunction() {}

    int testingItem;
    int* testingptr;
    char testingChar;

    Test() : testingItem(3) {}
    virtual ~Test() {}
};

class Test2 : public Test {
    virtual void test3() {}
    Test2() {}
    ~Test2() {}
};
