#include <filesystem>
#include <memory>
#include <span>
#include <string_view>
#include <utility>

namespace om::io
{
struct content
{
    std::unique_ptr<std::byte[]> memory;
    std::size_t                  size{};

    content(const content& other)            = delete;
    content& operator=(const content& other) = delete;

    content() noexcept
        : memory{}
        , size{}
    {
    }
    content(content&& other) noexcept
        : memory{ std::move(other.memory) }
        , size{ std::exchange(other.size, 0) }
    {
    }

    content& operator=(content&& other) noexcept
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

content read_file(const std::filesystem::path& path);
} // namespace om::io
