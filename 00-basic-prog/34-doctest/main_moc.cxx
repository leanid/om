#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <doctest/trompeloeil.hpp>

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

class TurtleMoc : public Turtle
{
public:
    MAKE_MOCK0(PenUp, void());
    MAKE_MOCK0(PenDown, void());
    MAKE_MOCK1(Forward, void(int));
    MAKE_MOCK1(Turn, void(int));
    MAKE_MOCK2(GoTo, void(int, int));
    MAKE_CONST_MOCK0(GetX, int());
    MAKE_CONST_MOCK0(GetY, int());
};

TEST_CASE("PainterTest, CanDrawSomething")
{
    auto painter = [](Turtle* turtle)
    {
        turtle->PenDown();
        turtle->Forward(10);
        turtle->Turn(45);
        auto x_pos = turtle->GetX();
        turtle->GoTo(x_pos, 50);
        turtle->PenUp();
    };

    TurtleMoc turtle;
    {
        trompeloeil::sequence seq;
        REQUIRE_CALL(turtle, PenDown()).IN_SEQUENCE(seq);
        REQUIRE_CALL(turtle, PenUp()).IN_SEQUENCE(seq);
        REQUIRE_CALL(turtle, Forward(10));
        REQUIRE_CALL(turtle, Turn(45));
        REQUIRE_CALL(turtle, GoTo(42, 50));
        REQUIRE_CALL(turtle, GetX()).RETURN(42);

        painter(&turtle);
    }
}
} // namespace om
