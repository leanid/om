#include <om/engine.hxx>
#include <om/game.hxx>

class tic_tac_toe final : public om::game
{
public:
    explicit tic_tac_toe(om::engine&);

    void initialize() final;
    void proccess_input(om::event& e) final;
    void update(om::milliseconds frame_delta) final;
    void draw() const final;
    bool is_closed() const final;

private:
    om::engine& e;
};

std::unique_ptr<om::game> create_game(om::engine& e)
{
    return std::make_unique<tic_tac_toe>(e);
}

tic_tac_toe::tic_tac_toe(om::engine& e_)
    : e(e_)
{
    om::engine::params params;

    params.title           = "tic_tac_toe game example";
    params.wnd_mode.width  = 1024;
    params.wnd_mode.height = 768;

    e.initialize(params);
}

void tic_tac_toe::initialize()
{
}

void tic_tac_toe::proccess_input(om::event&)
{
}

void tic_tac_toe::update(om::milliseconds)
{
}

void tic_tac_toe::draw() const
{
}

bool tic_tac_toe::is_closed() const
{
    return false;
}
