#include <algorithm>
#include <charconv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

// Символы-границы сегментов, которые линкер (и GNU ld, и lld) добавляет
// в каждый исполняемый файл. Важны их АДРЕСА, поэтому объявлены как
// массивы неизвестного размера.
extern char etext[]; // конец .text (код)
extern char edata[]; // конец .data (инициализированные данные)
extern char end[];   // конец .bss; стартовое значение program break

extern char** environ; // окружение процесса - лежит наверху стека

namespace
{

int        g_initialized_data = 42;      // .data: есть инициализатор
int        g_uninitialized_bss;          // .bss: ядро обнулит при загрузке
const char g_initialized_rodata[] = "r"; // .rodata: только чтение

/// Читаем /proc/self/maps целиком (в этом примере куча разрешена).
std::string read_maps()
{
    std::ifstream      file{ "/proc/self/maps", std::ios::binary };
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

/// Находит строку maps, чей диапазон [start, end) содержит ptr.
std::string_view find_mapping(std::string_view maps, const void* ptr)
{
    const std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(ptr);
    std::string_view     rest = maps;
    while (!rest.empty())
    {
        const std::size_t      eol = rest.find('\n');
        const std::string_view line =
            rest.substr(0, eol == std::string_view::npos ? rest.size() : eol);

        // формат строки: "55f7a1b2c000-55f7a1b2d000 r-xp ..."
        std::uintptr_t start = 0;
        std::uintptr_t stop  = 0;
        const char*    p     = line.data();
        const char*    e     = p + line.size();
        const auto     r1    = std::from_chars(p, e, start, 16);
        if (r1.ec == std::errc{} && r1.ptr != e && *r1.ptr == '-')
        {
            const auto r2 = std::from_chars(r1.ptr + 1, e, stop, 16);
            if (r2.ec == std::errc{} && addr >= start && addr < stop)
            {
                return line;
            }
        }

        if (eol == std::string_view::npos)
        {
            break;
        }
        rest.remove_prefix(eol + 1);
    }
    return {};
}

void show_mapping(const char* label, const void* ptr, std::string_view maps)
{
    std::printf("  %-24s %p\n", label, ptr);
    const std::string_view line = find_mapping(maps, ptr);
    if (line.empty())
    {
        std::printf("  %-24s -> (в maps не найдено)\n", "");
    }
    else
    {
        std::printf("  %-24s -> %.*s\n",
                    "",
                    static_cast<int>(line.size()),
                    line.data());
    }
}

// noinline: иначе компилятор встроит функции и демонстрация переиспользования
// кадра стека сломается.
#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
void stack_writer()
{
    volatile unsigned char buf[128];
    for (int i = 0; i < 128; ++i)
    {
        buf[i] = 0xCD;
    }
}

#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
void stack_reader()
{
    // Тот же кадр стека, что только что занимал stack_writer: вызовы на
    // одной глубине из одного места в main.
    unsigned char buf[128];
    std::printf("  стек НЕ обнуляется: buf[64] = 0x%02x "
                "(остаток от предыдущего вызова)\n",
                buf[64]);
}

void show_stack_growth(int depth, const char* previous)
{
    char        local = 0;
    const char* trend = "";
    if (previous != nullptr)
    {
        trend = (&local < previous) ? " (ниже предыдущего)" : " (ВЫШЕ?!)";
    }
    std::printf("  глубина %d: &local = %p%s\n", depth, (void*)&local, trend);
    if (depth < 3)
    {
        show_stack_growth(depth + 1, &local);
    }
    (void)local;
}

/// Адрес начала сегмента из строки maps ("55...-55... r-xp ...").
std::uintptr_t mapping_start(std::string_view line)
{
    std::uintptr_t start = 0;
    std::from_chars(line.data(), line.data() + line.size(), start, 16);
    return start;
}

/// Строка maps, содержащая needle (например "[heap]" или "[vdso]").
std::string_view find_mapping_by_name(std::string_view maps,
                                      std::string_view needle)
{
    std::string_view rest = maps;
    while (!rest.empty())
    {
        const std::size_t      eol = rest.find('\n');
        const std::string_view line =
            rest.substr(0, eol == std::string_view::npos ? rest.size() : eol);
        if (line.find(needle) != std::string_view::npos)
        {
            return line;
        }
        if (eol == std::string_view::npos)
        {
            break;
        }
        rest.remove_prefix(eol + 1);
    }
    return {};
}

std::string hex_addr(std::uintptr_t addr)
{
    char       buffer[24];
    const auto result =
        std::to_chars(buffer, buffer + sizeof(buffer), addr, 16);
    return "0x" + std::string(buffer, result.ptr);
}

std::string addr_or_dash(std::string_view line)
{
    return line.empty() ? std::string("— (нет в maps)")
                        : hex_addr(mapping_start(line));
}

struct landmark
{
    char        letter;
    double      position; // 0..1, схематично (НЕ в масштабе!)
    std::string address;
    const char* title;
    std::string why;
};

/// Рисует всю адресную карту процесса одной горизонтальной прямой с
/// псевдографикой и легендой под ней. Порядок сегментов верный, но
/// расстояния условные: при 2^64 адресов честный масштаб бессмысленен -
/// вся "жизнь" сжата в крошечной доле диапазона.
void print_address_space_map(std::string_view maps,
                             const void*      code_ptr,
                             const void*      mmap_ptr,
                             const void*      libc_ptr)
{
    // ширина терминала (при перенаправлении в файл/pipe - 80)
    int     width = 80;
    winsize ws{};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col >= 60)
    {
        width = std::min<int>(ws.ws_col, 160);
    }

    const std::string_view exe_line   = find_mapping(maps, code_ptr);
    const std::string_view heap_line  = find_mapping_by_name(maps, "[heap]");
    const std::string_view stack_line = find_mapping_by_name(maps, "[stack]");
    const std::string_view vvar_line  = find_mapping_by_name(maps, "[vvar]");
    const std::string_view vdso_line  = find_mapping_by_name(maps, "[vdso]");
    const std::string_view vsyscall_line =
        find_mapping_by_name(maps, "[vsyscall]");

    const landmark marks[] = {
        { 'Z',
          0.00,
          "0x0",
          "zero page (первая страница и первые 64 КиБ)",
          "PROT_NONE: нельзя ни читать, ни писать, ни исполнять.\n"
          "      Ядро не даёт mmap ниже mmap_min_addr (65536), поэтому\n"
          "      разыменование nullptr падает мгновенно (SIGSEGV), а не\n"
          "      молча читает мусор." },
        { 'E',
          0.09,
          addr_or_dash(exe_line),
          "наш exe (PIE)",
          "4 сегмента: r--p (rodata), r-xp (код), r--p (relro),\n"
          "      rw-p (data+bss). База рандомизируется ASLR при каждом\n"
          "      запуске - сравни адреса между запусками." },
        { 'H',
          0.16,
          addr_or_dash(heap_line),
          "[heap]",
          "растёт ВВЕРХ через brk/sbrk (мы двигали break в п.3).\n"
          "      Большие malloc glibc обслуживает через mmap - сюда они\n"
          "      не попадают." },
        { 'M',
          0.50,
          hex_addr(reinterpret_cast<std::uintptr_t>(mmap_ptr)),
          "mmap-область",
          std::string("растёт ВНИЗ от верха user space: тут наши mmap, все\n"
                      "      .so (например libc: ")
              .append(hex_addr(reinterpret_cast<std::uintptr_t>(libc_ptr)))
              .append("), большие аллокации glibc.") },
        { 'V',
          0.62,
          addr_or_dash(vvar_line) + " / " + addr_or_dash(vdso_line),
          "[vvar] / [vdso]",
          "вспомогательные страницы ЯДРА в нашем адресном\n"
          "      пространстве: vvar - данные ядра (время и др.), читаемые\n"
          "      без syscall; vdso - код ядра (clock_gettime, getpid...),\n"
          "      исполняемый прямо в user mode, без перехода в ядро." },
        { 'S',
          0.70,
          addr_or_dash(stack_line),
          "[stack]",
          "растёт ВНИЗ (п.4); наверху лежат argv/environ.\n"
          "      Лимит ~8 МиБ (ulimit -s), переполнение = SIGSEGV." },
        { 'U',
          0.78,
          "0x7fffffffffff",
          "потолок user space (48 бит)",
          "4-уровневая трансляция страниц даёт 128 ТиБ\n"
          "      пользовательского пространства. Выше - только ядро." },
        { 'Y',
          0.86,
          addr_or_dash(vsyscall_line),
          "[vsyscall]",
          "legacy-страница на ФИКСИРОВАННОМ адресе 0xffffffffff600000\n"
          "      (без ASLR!): формально уже в половине ядра, но исполняема\n"
          "      из userspace. Деприкейтнута, оставлена для совместимости." },
        { 'K',
          0.98,
          "0xffff800000000000+",
          "память ядра",
          "старшая половина адресного пространства - само ядро.\n"
          "      Из user mode недоступна (обращение = SIGSEGV) и в maps\n"
          "      не показывается. Именно тут ядро хранит ВСЁ остальное о\n"
          "      нашем процессе: task_struct, таблицы страниц, файловые\n"
          "      дескрипторы..." },
    };

    // сама прямая: ячейки по символу на колонку (UTF-8 "─" - 3 байта,
    // поэтому считаем по дисплейным ячейкам, а не по байтам)
    std::vector<std::string> cells(static_cast<std::size_t>(width), "─");
    auto                     col_of = [&](char letter)
    {
        for (const landmark& m : marks)
        {
            if (m.letter == letter)
            {
                return static_cast<std::size_t>(m.position * (width - 1));
            }
        }
        return std::size_t{ 0 };
    };
    // гигантские дыры: zero page -> exe (низ адресного пространства
    // намеренно пуст - см. Q/A ниже), heap -> mmap-область и
    // потолок user space -> ядро
    for (std::size_t c = col_of('Z') + 1; c < col_of('E'); ++c)
    {
        cells[c] = "~";
    }
    for (std::size_t c = col_of('H') + 1; c < col_of('M'); ++c)
    {
        cells[c] = "~";
    }
    for (std::size_t c = col_of('U') + 1; c < col_of('K'); ++c)
    {
        cells[c] = "~";
    }
    for (const landmark& m : marks)
    {
        cells[col_of(m.letter)] = std::string(1, m.letter);
    }

    std::printf("схема (НЕ в масштабе; порядок верный, адреса - реальные,\n"
                "этого запуска):\n\n");
    std::printf("0x0%*s\n", width - 3, "0xffffffffffffffff");
    for (const auto& cell : cells)
    {
        std::printf("%s", cell.c_str());
    }
    std::printf("\n\n~~~~ = гигантские дыры (десятки ТиБ несмапленных "
                "адресов)\n\n");

    for (const landmark& m : marks)
    {
        std::printf(" [%c] %-26s %s\n", m.letter, m.title, m.address.c_str());
        std::printf("      %s\n\n", m.why.c_str());
    }

    // -- частый вопрос студента, глядящего на эту прямую --
    const std::uintptr_t exe_base = mapping_start(exe_line);
    const std::uintptr_t gap = exe_base - 0x10000; // минус 64 КиБ zero-зоны
    std::printf(
        "Q: почему между Z и E такое большое пространство?\n"
        "A: в этом запуске дыра Z->E = %s (%.1f ТиБ) намеренно пустой\n"
        "   памяти. Пустота бесплатна: несмапленные виртуальные адреса не\n"
        "   занимают физической памяти вообще - под них просто не\n"
        "   заведено записей в таблицах страниц. А низ держат пустым,\n"
        "   потому что:\n"
        "   1) wild pointer nullptr+N (даже с большим N) попадает в\n"
        "      несмапленное -> мгновенный SIGSEGV вместо чтения мусора;\n"
        "   2) это комната для ASLR: exe - это PIE, загрузчик кладёт его\n"
        "      на случайную базу при каждом запуске (сравни адрес [E]\n"
        "      между запусками!) - так атакующий не знает адреса кода;\n"
        "   3) упаковывать сегменты плотно незачем - user space это\n"
        "      128 ТиБ, места хватает всем. (Классический non-PIE exe\n"
        "      грузился на фиксированный 0x400000, и зазор был всего\n"
        "      4 МиБ.)\n\n",
        hex_addr(gap).c_str(),
        static_cast<double>(gap) / 1099511627776.0);
}

} // namespace

int main()
{
    std::printf("=== 01-3-mem-layout: из чего состоит память процесса ===\n");
    std::printf("(адреса при каждом запуске другие - это ASLR/PIE)\n");
    std::printf("размер страницы: %ld байт\n\n", ::sysconf(_SC_PAGESIZE));

    // Гарантированно создаём [heap]: маленький malloc растёт main-арену
    // glibc через brk. Без него кучи могло бы ещё не быть.
    void* heap_probe = std::malloc(64);

    // mmap-кусок выделяем ДО снимка maps, чтобы он попал в карту.
    constexpr std::size_t mmap_size  = 4 * 1024 * 1024;
    void*                 mmap_chunk = ::mmap(nullptr,
                              mmap_size,
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1,
                              0);
    if (mmap_chunk == MAP_FAILED)
    {
        std::printf("mmap failed\n");
        return 1;
    }

    // sbrk(0) - текущий program break (конец кучи). &end[0] - начальный.
    void* current_break = ::sbrk(0);

    // Снимок карты делаем после всех выделений выше.
    const std::string maps = read_maps();

    std::printf("-- 1. где что лежит (адрес -> строка /proc/self/maps)\n");
    // Адрес функции: cast function->object pointer - POSIX-изм, на Linux
    // работает (на нём же построен и dlsym). &main брать нельзя - это
    // ill-formed по стандарту, поэтому показываем адрес обычной функции.
    show_mapping(
        "код (find_mapping)", reinterpret_cast<void*>(&find_mapping), maps);
    show_mapping("rodata (массив)", g_initialized_rodata, maps);
    show_mapping("rodata (литерал)", "string literal", maps);
    show_mapping("data (global int)", &g_initialized_data, maps);
    show_mapping("bss (global int)", &g_uninitialized_bss, maps);
    show_mapping("etext (конец .text)", etext, maps);
    show_mapping("edata (конец .data)", edata, maps);
    show_mapping("end (конец .bss)", end, maps);
    show_mapping(
        "куча (break - 1)", static_cast<char*>(current_break) - 1, maps);
    show_mapping("mmap (4 MiB)", mmap_chunk, maps);
    show_mapping("libc (printf)", reinterpret_cast<void*>(&std::printf), maps);
    {
        char stack_var = 0;
        show_mapping("стек (local)", &stack_var, maps);
    }
    show_mapping("environ (верх стека)", environ, maps);

    std::printf("\n-- 2. как инициализируется память\n");
    std::printf("  .bss ядро обнуляет при загрузке: g_uninitialized_bss = %d\n",
                g_uninitialized_bss);
    std::printf("  .data читается из файла:       g_initialized_data = %d\n",
                g_initialized_data);
    std::printf("  свежий mmap обнулён ядром:     mmap_chunk[0] = %d\n",
                static_cast<char*>(mmap_chunk)[0]);
    {
        // glibc вернёт тот же chunk из tcache: первые ~16 байт испорчены
        // служебными указателями tcache, дальше лежат наши старые данные.
        constexpr std::size_t n = 256;
        auto*                 a = static_cast<unsigned char*>(std::malloc(n));
        std::memset(a, 0xAB, n);
        std::free(a);
        auto* b = static_cast<unsigned char*>(std::malloc(n));
        std::printf("  переработанная куча НЕ обнулена: b[32] = 0x%02x "
                    "(записывали 0xAB)\n",
                    b[32]);
        std::free(b);
    }
    stack_writer();
    stack_reader();

    std::printf("\n-- 3. sbrk: ручное управление program break\n");
    {
        void*                 old_break = ::sbrk(0);
        constexpr std::size_t delta     = 1024 * 1024;
        void*                 grown     = ::sbrk(delta); // == old_break
        void*                 new_break = ::sbrk(0);
        std::printf("  break был %p, стал %p (+%lld байт)\n",
                    old_break,
                    new_break,
                    static_cast<long long>(static_cast<char*>(new_break) -
                                           static_cast<char*>(old_break)));
        // Свежие страницы от ядра всегда нулевые - иначе через них
        // утекали бы данные других процессов.
        const auto* fresh = static_cast<unsigned char*>(grown);
        std::printf("  свежая память от ядра обнулена: fresh[0] = %u\n",
                    fresh[0]);
        ::sbrk(-static_cast<std::intptr_t>(delta)); // вернуть как было
    }

    std::printf("\n-- 4. стек растёт вниз (к меньшим адресам)\n");
    show_stack_growth(0, nullptr);

    std::printf("\n-- 5. вся адресная карта на одной прямой\n\n");
    print_address_space_map(maps,
                            reinterpret_cast<void*>(&find_mapping),
                            mmap_chunk,
                            reinterpret_cast<void*>(&std::printf));

    std::free(heap_probe);
    ::munmap(mmap_chunk, mmap_size);
    std::printf("\n=== done ===\n");
    return 0;
}
