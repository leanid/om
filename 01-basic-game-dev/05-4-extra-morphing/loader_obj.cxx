#include "loader_obj.hxx"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>

std::vector<std::string_view> split_line_into_words(std::string_view line,
                                                    const char       delimeter)
{
    using namespace std;
    if (line.empty())
    {
        return {};
    }

    size_t count_spaces = std::count(begin(line), end(line), delimeter);

    std::vector<size_t> start_next_word;
    start_next_word.reserve(count_spaces + 1);
    start_next_word.push_back(0); // first index

    size_t i = 0;

    auto copy_space_index = [&start_next_word, &i, delimeter](char value)
    {
        ++i;
        if (value == delimeter)
        {
            start_next_word.push_back(i);
        }
    };

    for_each(begin(line), end(line), copy_space_index);

    vector<string_view> result;
    result.reserve(count_spaces + 1);

    transform(begin(start_next_word),
              --end(start_next_word),
              ++begin(start_next_word),
              back_inserter(result),
              [&line](size_t first_index, size_t last_index)
              { return line.substr(first_index, --last_index - first_index); });

    auto last_word = line.substr(start_next_word.back());
    result.push_back(last_word);

    return result;
}

float to_float(std::string_view word) noexcept(false)
{
    const char* first = word.data();
    char*       str_end;
    float       value = std::strtof(first, &str_end);
    // if no conversion can be performed 0 is returned and
    // *str_end is set to str
    if (str_end == first)
    {
        throw std::runtime_error(std::string("can't convert: ") + first);
    }
    return value;
}

int to_int(std::string_view word) noexcept(false)
{
    int  value = 0;
    auto err   = std::from_chars(word.data(), word.data() + word.size(), value);
    if (err.ec != std::errc())
    {
        throw std::runtime_error(std::string("can't convert value to int: ") +
                                 word.data());
    }
    return value;
}

loader_obj::point parse_point(std::string_view indexes_substr) noexcept(false)
{
    loader_obj::point p;
    auto              indexes_str = split_line_into_words(indexes_substr, '/');
    switch (indexes_str.size())
    {
        case 1:
        {
            int v  = to_int(indexes_str.at(0));
            p.type = loader_obj::point_type::only_vertex_index;
            p.v    = loader_obj::only_v{ v };
        }
        break;
        case 2:
        {
            int v  = to_int(indexes_str.at(0));
            int vt = to_int(indexes_str.at(1));
            p.type = loader_obj::point_type::vetex_and_texture_indexes;
            p.vt   = loader_obj::v_vt{ v, vt };
        }
        break;
        case 3:
        {
            int v  = to_int(indexes_str.at(0));
            int vt = to_int(indexes_str.at(1));
            int n  = to_int(indexes_str.at(2));
            p.type = loader_obj::point_type::vetex_texture_normal_indexes;
            p.vtn  = loader_obj::v_vt_vn{ v, vt, n };
        }
        break;
        default:
            throw std::runtime_error("bad indexes");
    } // end switch
    return p;
}

loader_obj::loader_obj(std::istream& byte_stream)
{
    using namespace std;
    string line;

    while (getline(byte_stream, line))
    {
        auto words = split_line_into_words(line, ' ');

        if (words.empty())
        {
            continue;
        }
        else if (words.front() == "#")
        {
            continue;
        }
        else if (words.front() == "v")
        {
            float x = to_float(words.at(1));
            float y = to_float(words.at(2));
            float z = to_float(words.at(3));
            vertexes_.push_back(vertex{ x, y, z, 1.f });
        }
        else if (words.front() == "vt")
        {
            float tu = to_float(words.at(1));
            float tv = to_float(words.at(2));
            texture_coords_.push_back(texture_coord{ tu, tv, 0.f });
        }
        else if (words.front() == "vn")
        {
            float vnx = to_float(words.at(1));
            float vny = to_float(words.at(2));
            float vnz = to_float(words.at(3));
            normals_.push_back(normal{ vnx, vny, vnz });
        }
        else if (words.front() == "f")
        {
            point p0 = parse_point(words.at(1));
            point p1 = parse_point(words.at(2));
            point p2 = parse_point(words.at(3));
            faces_.push_back({ p0, p1, p2 });
        }
        else if (words.front() == "mtllib")
        {
            continue;
        }
        else if (words.front() == "o")
        {
            object_name = words.at(1);
        }
        else if (words.front() == "usemtl")
        {
            continue;
        }
        else if (words.front() == "s")
        {
            continue;
        }
        else
        {
            throw std::runtime_error(std::string("unknown element: ") +
                                     words.front().data());
        }
    }
}
