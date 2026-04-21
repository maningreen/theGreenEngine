struct Bar {
    int _3;
};

namespace Foo {

template <typename T>
class Test {
  public:
    virtual void testingFunction(float delta);

    int testingItem;
    int* testingptr;
    char testingChar;

    virtual ~Test();
    unsigned fizzbuzzFunc(int);

    Test();
    Test(int i);
};

class Test2 : public Test<Bar> {
    void testingFunction(float delta) override;
    Test2();
    ~Test2();
};

float sum(float a, float b);

};  // namespace Foo
