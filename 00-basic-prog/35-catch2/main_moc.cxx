#include <catch_amalgamated.hpp>
#include <fakeit.hpp>

namespace om
{
class Turtle
{
public:
    virtual ~Turtle()                               = default;
    virtual void              PenUp()               = 0;
    virtual void              PenDown()             = 0;
    virtual void              Forward(int distance) = 0;
    virtual void              Turn(int degrees)     = 0;
    virtual void              GoTo(int x, int y)    = 0;
    [[nodiscard]] virtual int GetX() const          = 0;
    [[nodiscard]] virtual int GetY() const          = 0;
};

TEST_CASE("PainterTest, CanDrawSomething")
{
    using namespace fakeit;
    Mock<Turtle> mock;

    Fake(Method(mock, PenDown));
    Fake(Method(mock, Forward));
    Fake(Method(mock, Turn));
    When(Method(mock, GetX)).Return(42); // mock.GetX will return 42 once
    Fake(Method(mock, GoTo));
    Fake(Method(mock, PenUp));

    auto painter = [](Turtle* turtle)
    {
        turtle->PenDown();
        turtle->Forward(10);
        turtle->Turn(45);
        auto x_pos = turtle->GetX();
        turtle->GoTo(x_pos, 50);
        turtle->PenUp();
    };

    Turtle& turtle = mock.get();

    painter(&turtle);

    Verify(Method(mock, Forward).Using(10)); // was invoked with arg 10
    Verify(Method(mock, Turn).Using(45));
    Verify(Method(mock, GoTo).Using(42, 50));
    Verify(Method(mock, PenDown),
           Method(mock, PenUp)); // verify order of invocations
}
} // namespace om
