#pragma once

#include <iosfwd>
#include <vector>

/// ultra light Wayfront.obj format file parser
/// only for minimal play with morphing example
class loader_obj final
{
public:
    explicit loader_obj(std::istream& byte_stream);

    struct vertex
    {
        float x;
        float y;
        float z;
        float w;
    };

    const std::vector<vertex>& vertexes() const;

    struct texture_coord
    {
        float u;
        float v;
        float w;
    };

    const std::vector<texture_coord>& texture_coords() const;

    struct normal
    {
        float x;
        float y;
        float z;
    };

    const std::vector<normal>& normals() const;

    enum class point_type
    {
        only_vertex_index,
        vetex_and_texture_indexes,
        vetex_and_normal_indexes,
        vetex_texture_normal_indexes
    };

    struct only_v
    {
        int v;
    };

    struct v_vt
    {
        int v;
        int vt;
    };

    struct v_vn
    {
        int v;
        int vn;
    };

    struct v_vt_vn
    {
        int v;
        int vt;
        int vn;
    };

    struct point
    {
        union {
            only_v  v;
            v_vt    vt;
            v_vn    vn;
            v_vt_vn vtn;
        };
        point_type type;
    };

    struct face
    {
        point p0;
        point p1;
        point p2;
    };

    const std::vector<face>& faces() const;

private:
    std::vector<vertex>        vertexes_;
    std::vector<texture_coord> texture_coords_;
    std::vector<normal>        normals_;
    std::vector<face>          faces_;
};

const std::vector<loader_obj::vertex>& loader_obj::vertexes() const
{
    return vertexes_;
}

const std::vector<loader_obj::texture_coord>& loader_obj::texture_coords() const
{
    return texture_coords_;
}

const std::vector<loader_obj::normal>& loader_obj::normals() const
{
    return normals_;
}

const std::vector<loader_obj::face>& loader_obj::faces() const
{
    return faces_;
}
