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
           const om::vec2& size, const float angle);

    void draw(om::engine& render);

    om::texture* texture() const;
    void         texture(om::texture* t);

    const rect& uv_rect() const;
    void        uv_rect(const rect& r);

    /// center of sprite in world coordinates
    om::vec2 pos() const;
    void     pos(const om::vec2& p);

    /// width of sprite in world coordinates
    om::vec2 size() const;
    void     size(const om::vec2& s);

    /// angle of sprite in radians
    float rotation() const;
    void  rotation(const float r);

private:
    om::texture* texture_ = nullptr;
    rect         uv_rect_;
    om::vec2     pos_;
    om::vec2     size_;
    float        rotation_ = 0; // radian
};
