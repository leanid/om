#include <catch2/catch_all.hpp>

#include <algorithm>
#include <filesystem>
#include <string_view>

#include "read_file.hxx"
// NOLINTBEGIN(*)
TEST_CASE("check file reading", "io::read_file")
{
    std::filesystem::path path =
        "./02-vulkan/07-vk-pipeline-1/io/read_file_test.cxx";

    constexpr size_t this_file_num_of_lines = 40;

    om::io::content content = om::io::read_file(path);
    auto            str     = content.as_string_view();
    auto            bytes   = content.as_span();

    // ms = 1/1'000 second (milli)
    // us = 1/1'000'000 second (micro)
    // ns = 1/1'000'000'000 second (nano)
    BENCHMARK("how fast is second read same file")
    {
        return om::io::read_file(path);
    };

    SECTION("view file content as std::string_view")
    {
        REQUIRE(str.find("TEST_CASE") != std::string_view::npos);
        REQUIRE(this_file_num_of_lines == std::ranges::count(str, '\n'));
    }

    SECTION("view file content as std::span<std::byte>")
    {
        REQUIRE(str.size() == bytes.size());
        REQUIRE(this_file_num_of_lines ==
                std::ranges::count(bytes, std::byte{ '\n' }));
    }
}
// NOLINTEND(*)
