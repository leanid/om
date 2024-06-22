#include <filesystem>
#include <memory>
#include <span>
#include <string_view>
#include <utility>

namespace om::io
{
struct content_t
{
    std::unique_ptr<std::byte[]> memory;
    std::size_t                  size{};

    content_t(const content_t& other)            = delete;
    content_t& operator=(const content_t& other) = delete;

    content_t() noexcept
        : memory{}
        , size{}
    {
    }
    content_t(content_t&& other) noexcept
        : memory{ std::move(other.memory) }
        , size{ std::exchange(other.size, 0) }
    {
    }

    content_t& operator=(content_t&& other) noexcept
    {
        memory = std::move(other.memory);
        size   = std::exchange(other.size, 0);
        return *this;
    }

    std::string_view as_string_view() const noexcept
    {
        return { reinterpret_cast<char*>(memory.get()), size };
    }
    std::span<std::byte> as_span() const noexcept
    {
        return std::span{ memory.get(), size };
    }
};

content_t read_file(const std::filesystem::path& path);
} // namespace om::io
