# 01-3-hello-mem — трилогия про память процесса (Linux-only)

Три самостоятельных примера, которые лучше смотреть по порядку:

| Папка    | Тема                                                                          | Таргет(ы) |
|----------|-------------------------------------------------------------------------------|-----------|
| `layout/` | Из чего состоит память процесса: text/rodata/data/bss, куча (`sbrk`), стек, `mmap`, `/proc/self/maps` — всё изнутри приложения | `01-3-mem-layout` |
| `mem/`    | Тотальный запрет `new`/`delete`/`malloc`/`free` + свой аллокатор на `mmap` + `std::pmr` поверх него | `01-3-hello-mem` |
| `libs/`   | Запрет new/malloc vs чужая библиотека: статическая, shared (load-time), `dlopen`. Какие сегменты добавляет `.so` в карту процесса | `01-3-mem-libs-{static,shared,dlopen}` |

## Сборка и запуск

```sh
cmake --build build/ninja-llvm --config Debug \
    --target 01-3-mem-layout 01-3-hello-mem 01-3-mem-libs-static \
    01-3-mem-libs-shared 01-3-mem-libs-dlopen
ctest --test-dir build/ninja-llvm -R "mem_layout|hello_mem|mem_libs" \
    --output-on-failure
```
