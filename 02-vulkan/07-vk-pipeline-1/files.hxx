#include <filesystem>
#include <memory>
#include <string_view>

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
        , size{ other.size }
    {
        other.size = 0;
    }

    content_t& operator=(content_t&& other) noexcept
    {
        memory     = std::move(other.memory);
        size       = other.size;
        other.size = 0;
        return *this;
    }

    std::string_view as_string_view() const;
};

content_t read_file(const std::filesystem::path& path);
} // namespace om::files
