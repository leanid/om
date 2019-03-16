#include "sprite.hxx"

sprite::sprite() {}

sprite::sprite(om::texture* tex, const rect& rect_on_texture,
               const om::vec2& pos, const om::vec2& size, const float angle)
    : texture_(tex)
    , uv_rect_(rect_on_texture)
    , pos_(pos)
    , size_(size)
    , rotation_(angle)
{
}

void sprite::draw(om::engine& render)
{
    if (texture_ == nullptr)
    {
        return; // sprite is empty nothing to do
    }

    ///   0            1
    ///   *------------*
    ///   |           /|
    ///   |         /  |
    ///   |      P/    |  // P - pos_ or center of sprite
    ///   |     /      |
    ///   |   /        |
    ///   | /          |
    ///   *------------*
    ///   3            2
    ///

    using namespace om;

    v2 vertexes[4];
    /// remember in OpenGL texture lower left angle is (0, 0) coordinate
    vertexes[0].uv = uv_rect_.pos + vec2(0.f, uv_rect_.size.y);
    vertexes[1].uv = uv_rect_.pos + vec2(uv_rect_.size.x, uv_rect_.size.y);
    vertexes[2].uv = uv_rect_.pos + vec2(uv_rect_.size.x, 0.f);
    vertexes[3].uv = uv_rect_.pos;

    float half_width  = size_.x / 2;
    float half_height = size_.y / 2;

    vertexes[0].pos = pos_ + vec2(-half_width, half_height);
    vertexes[1].pos = pos_ + vec2(half_width, half_height);
    vertexes[2].pos = pos_ + vec2(half_width, -half_height);
    vertexes[3].pos = pos_ + vec2(-half_width, -half_height);

    color white{ 1, 1, 1, 1 };

    vertexes[0].c = white;
    vertexes[1].c = white;
    vertexes[2].c = white;
    vertexes[3].c = white;

    // build 2 triangles to render sprite
    tri2 tr0;
    tr0.v[0] = vertexes[0];
    tr0.v[1] = vertexes[1];
    tr0.v[2] = vertexes[3];

    tri2 tr1;
    tr1.v[0] = vertexes[1];
    tr1.v[1] = vertexes[2];
    tr1.v[2] = vertexes[3];

    mat2x3 world_transform; // identity for now

    render.render(tr0, texture_, world_transform);
    render.render(tr1, texture_, world_transform);
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

float sprite::rotation() const
{
    return rotation_;
}

void sprite::rotation(const float r)
{
    rotation_ = r;
}
