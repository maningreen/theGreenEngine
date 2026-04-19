

class Test {
  public:
    virtual void testingFunction(float delta);

    int testingItem;
    int* testingptr;
    char testingChar;

    Test();
    virtual ~Test();
    void test() const;
};

class Test2 : public Test {
    void testingFunction(float delta) override;
    Test2();
    ~Test2();
};

float sum(float a, float b);
