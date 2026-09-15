#include "maps.hxx"

#include "banned.hxx"

#include <charconv>
#include <cstdint>

#include <fcntl.h>
#include <unistd.h>

namespace om
{

namespace
{

// Снимки /proc/self/maps храним в .bss - куча запрещена.
constexpr std::size_t maps_capacity = 256 * 1024;
char                  g_maps_storage[2][maps_capacity];

std::string_view next_line(std::string_view& text) noexcept
{
    const std::size_t eol = text.find('\n');
    if (eol == std::string_view::npos)
    {
        const std::string_view line = text;
        text                        = {};
        return line;
    }
    const std::string_view line = text.substr(0, eol);
    text.remove_prefix(eol + 1);
    return line;
}

} // namespace

std::string_view snapshot_maps(int slot) noexcept
{
    if (slot < 0 || slot > 1)
    {
        return {};
    }
    const int fd = ::open("/proc/self/maps", O_RDONLY);
    if (fd < 0)
    {
        return {};
    }
    char*       buf  = g_maps_storage[slot];
    std::size_t used = 0;
    for (;;)
    {
        const ssize_t n = ::read(fd, buf + used, maps_capacity - 1 - used);
        if (n <= 0)
        {
            break;
        }
        used += static_cast<std::size_t>(n);
        if (used >= maps_capacity - 1)
        {
            break;
        }
    }
    ::close(fd);
    buf[used] = '\0';
    return { buf, used };
}

std::size_t print_maps_containing(std::string_view maps,
                                  std::string_view needle) noexcept
{
    std::size_t      count = 0;
    std::string_view rest  = maps;
    while (!rest.empty())
    {
        const std::string_view line = next_line(rest);
        if (line.find(needle) != std::string_view::npos)
        {
            print("    ");
            print(line);
            print("\n");
            ++count;
        }
    }
    if (count == 0)
    {
        print("    (строк не найдено)\n");
    }
    return count;
}

void print_maps_added(std::string_view older, std::string_view newer) noexcept
{
    std::size_t      count = 0;
    std::string_view rest  = newer;
    while (!rest.empty())
    {
        const std::string_view line = next_line(rest);
        if (!line.empty() && older.find(line) == std::string_view::npos)
        {
            print("  + ");
            print(line);
            print("\n");
            ++count;
        }
    }
    if (count == 0)
    {
        print("  (различий нет)\n");
    }
}

void print_map_line_for(const void* ptr, std::string_view maps) noexcept
{
    const auto       addr = reinterpret_cast<std::uintptr_t>(ptr);
    std::string_view rest = maps;
    while (!rest.empty())
    {
        const std::string_view line = next_line(rest);

        // формат строки: "55f7a1b2c000-55f7a1b2d000 r-xp ..."
        std::uintptr_t start = 0;
        std::uintptr_t stop  = 0;
        const char*    p     = line.data();
        const char*    e     = p + line.size();
        const auto     r1    = std::from_chars(p, e, start, 16);
        if (r1.ec != std::errc{} || r1.ptr == e || *r1.ptr != '-')
        {
            continue;
        }
        const auto r2 = std::from_chars(r1.ptr + 1, e, stop, 16);
        if (r2.ec != std::errc{})
        {
            continue;
        }
        if (addr >= start && addr < stop)
        {
            print(line);
            print("\n");
            return;
        }
    }
    print("(сегмент не найден)\n");
}

} // namespace om
