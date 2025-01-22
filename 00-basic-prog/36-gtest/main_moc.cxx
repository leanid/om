#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

class MockTurtle : public Turtle
{
public:
    MOCK_METHOD(void, PenUp, (), (override));
    MOCK_METHOD(void, PenDown, (), (override));
    MOCK_METHOD(void, Forward, (int distance), (override));
    MOCK_METHOD(void, Turn, (int degrees), (override));
    MOCK_METHOD(void, GoTo, (int x, int y), (override));
    MOCK_METHOD(int, GetX, (), (const, override));
    MOCK_METHOD(int, GetY, (), (const, override));
};

TEST(PainterTest, CanDrawSomething)
{
    MockTurtle turtle;
    using namespace ::testing;
    Expectation pen_down = EXPECT_CALL(turtle, PenDown()).Times(AtLeast(1));
    EXPECT_CALL(turtle, Forward(10)).Times(AtLeast(1));
    EXPECT_CALL(turtle, Turn(45)).Times(AtLeast(1));
    EXPECT_CALL(turtle, GoTo(42, 50)).Times(AtLeast(1));
    EXPECT_CALL(turtle, PenUp()).Times(AtLeast(1)).After(pen_down);
    EXPECT_CALL(turtle, GetX()).Times(AtLeast(1)).WillOnce(Return(42));

    auto painter = [](Turtle* turtle)
    {
        turtle->PenDown();
        turtle->Forward(10);
        turtle->Turn(45);
        auto x_pos = turtle->GetX();
        turtle->GoTo(x_pos, 50);
        turtle->PenUp();
    };

    painter(&turtle);
}
} // namespace om
