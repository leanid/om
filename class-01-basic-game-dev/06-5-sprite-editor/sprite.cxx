#include "sprite.hxx"

sprite::sprite() {}

sprite::sprite(om::texture* tex, const rect& rect_on_texture,
               const om::vec2& pos, const om::vec2& size,
               const om::vec2& center_pos)
    : texture_(tex)
    , tex_rect_(rect_on_texture)
    , pos_(pos)
    , size_(size)
    , center_(center_pos)
{
}

void sprite::draw(om::engine& /*render*/)
{
    om::v2 vertexes[4];

    vertexes[0].uv.x = tex_rect_.pos.x;
}

om::texture* sprite::texture() const
{
    return texture_;
}

void sprite::texture(om::texture* t)
{
    texture_ = t;
}

om::vec2 sprite::pos() const
{
    return pos_;
}

void sprite::pos(const om::vec2& p)
{
    pos_ = p;
}

om::vec2 sprite::size() const
{
    return size_;
}

void sprite::size(const om::vec2& s)
{
    size_ = s;
}

om::vec2 sprite::center() const
{
    return center_;
}

void sprite::center(const om::vec2& c)
{
    center_ = c;
}

float sprite::rotation() const
{
    return rotation_;
}

void sprite::rotation(const float r)
{
    rotation_ = r;
}
