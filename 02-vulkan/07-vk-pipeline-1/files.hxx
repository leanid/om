#include <filesystem>
#include <memory>
#include <string_view>

namespace om::files
{
struct content_t
{
    std::unique_ptr<std::byte[]> memory;
    std::size_t                  size{};

    std::string_view as_string_view() const;
};

content_t read_file(const std::filesystem::path& path);
} // namespace om::files
