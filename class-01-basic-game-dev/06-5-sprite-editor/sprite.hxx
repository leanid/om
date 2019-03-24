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
    sprite(const sprite&) = default;
    sprite(const std::string_view id, om::texture* tex,
           const rect& rect_on_texture, const om::vec2& pos,
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

    /// angle of sprite in degrees
    float rotation() const;
    void  rotation(const float r);

    const std::string& id() const;
    void               id(std::string_view name);

private:
    std::string  id_;
    om::texture* texture_ = nullptr;
    rect         uv_rect_;
    om::vec2     pos_;
    om::vec2     size_;
    float        rotation_ = 0; // degrees
};

inline bool operator==(const rect& l, const rect& r)
{
    return l.pos == r.pos && l.size == r.size;
}

inline bool operator==(const sprite& l, const sprite& r)
{
    return l.id() == r.id() && l.texture() == r.texture() &&
           l.uv_rect() == r.uv_rect() && l.pos() == r.pos() &&
           l.size() == r.size() &&
           std::abs(l.rotation() - r.rotation()) <= 0.000001f;
}
