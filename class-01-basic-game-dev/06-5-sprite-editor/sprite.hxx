#pragma once

#include "engine.hxx"

struct rect
{
    om::vec2 pos;
    om::vec2 size;
};

class sprite
{
public:
    sprite();
    sprite(om::texture* tex, const rect& rect_on_texture, const om::vec2& pos,
           const om::vec2& size, const om::vec2& center_pos);

    void draw(om::engine& render);

    om::texture* texture() const;
    void         texture(om::texture* t);

    om::vec2 pos() const;
    void     pos(const om::vec2& p);

    om::vec2 size() const;
    void     size(const om::vec2& s);

    om::vec2 center() const;
    void     center(const om::vec2& c);

    float rotation() const;
    void  rotation(const float r);

private:
    om::texture* texture_ = nullptr;
    rect         tex_rect_;
    om::vec2     pos_;
    om::vec2     size_;
    om::vec2     center_;
    float        rotation_ = 0;
};
