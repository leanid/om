#include <filesystem>
#include <memory>
#include <string_view>
#include <utility>

namespace om::files
{
struct content_t
{
    std::unique_ptr<std::byte[]> memory;
    std::size_t                  size{};

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

    content_t(const content_t& other) = delete;

    content_t& operator=(content_t&& other) noexcept
    {
        memory = std::move(other.memory);
        size   = std::exchange(other.size, 0);
        return *this;
    }

    content_t& operator=(const content_t& other) = delete;

    std::string_view as_string_view() const noexcept
    {
        return { reinterpret_cast<char*>(memory.get()), size };
    }
};

content_t read_file(const std::filesystem::path& path);
} // namespace om::files
