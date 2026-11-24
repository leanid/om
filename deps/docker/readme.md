# Docker образ для сборки проекта (Linux)

Образ `ghcr.io/leanid/om-deps:linux-clang23` — это среда, максимально близкая
к рабочей машине автора (ALT Linux 11.5, ветка p11), со всем необходимым для
сборки проекта без этапа «установи зависимости руками»: компилятор LLVM 23,
GCC 13 (нужен clang/LLD для поиска crt/libgcc), все собранные зависимости
проекта в `/deps/linux-clang23-x86_64`.

Предназначен в первую очередь для GitHub Actions, но годится и для локальной
сборки проекта «одной командой».

## Состав образа

| Компонент        | Версия / источник                                             |
|------------------|---------------------------------------------------------------|
| Базовый образ    | `alt:p11` (ALT Container, ветка p11 = ALT 11.x)               |
| GCC              | 13 (пакеты p11: `gcc`, `gcc-c++`, `libstdc++-devel-static`)   |
| LLVM (clang/lld) | 23.1.2, официальный tarball с GitHub, минимальный набор       |
| libc++           | из того же tarball (со sources для `import std`)              |
| ICU              | 70.1, собран в `/opt/icu70` (нужен `lld` из tarball'а LLVM)   |
| CMake            | 4.5.1, официальный бинарник (в p11 только 3.31 < 4.5.1)       |
| Ninja            | 1.13.2, официальный бинарник                                  |
| Vulkan SDK       | 1.4.335.0 (`slangc`, `glslangValidator`, headers, loader)     |
| Зависимости OM   | собраны внутри образа → `/deps/linux-clang23-x86_64`          |

В LLVM намеренно оставлен только необходимый минимум: `clang-23`, `lld`,
libc++/libc++abi/libunwind, compiler-rt и `share/libc++/v1/*.cppm`
(без mlir/flang/lldb/opt и статических `.a` LLVM — это экономит ~10 ГБ).

Зависимости проекта собираются **внутри образа** тем же скриптом, что и на
хосте (`deps/rules/linux-build.cmake`), поэтому набор и флаги совпадают с
локальной сборкой. CMake-конфиги после переноса в `/deps/...` поправляются
(absolute prefix rewrite), так что `find_package` работает.

## Требования для сборки образа

- Docker 23+ (BuildKit включён по умолчанию) или Podman
- ~25 ГБ свободного места (tarball LLVM ~1.2 ГБ, промежуточная распаковка
  ~12 ГБ, git-клоны зависимостей, сам образ)
- ~1 час времени (зависит от CPU и сети: Boost, SDL3, assimp, KTX, ICU)
- Сеть: образ качает с github.com (llvm, cmake, ninja, icu, все deps)
  и sdk.lunarg.com (Vulkan SDK)

Установка Docker на ALT p11 (если ещё не установлен):

```sh
sudo apt-get update && sudo apt-get install docker-ce
sudo systemctl enable --now docker
sudo usermod -aG docker "$USER"   # затем перелогиниться
```

## Сборка образа

Из **корня репозитория** (контекст сборки — корень; Dockerfile сам ограничивает
его через `deps/docker/Dockerfile.dockerignore`, лишнее не копируется):

```sh
docker build -f deps/docker/Dockerfile -t ghcr.io/leanid/om-deps:linux-clang23 .
```

Стадии Dockerfile:

1. **toolchain** — `alt:p11` + пакеты p11 + ICU 70.1 + LLVM 23 + CMake/Ninja +
   Vulkan SDK + smoke-тест всей цепочки: версии (cmake 4.5.x, ninja 1.13.x,
   clang 23.x, slangc, glslangValidator), LLD+ICU, компиляция и запуск
   программы с `libc++` голым `clang++`, а также мини-проект
   `deps/docker/toolchain-check/` с `import std`, собранный через
   **Ninja Multi-Config** с `cmake/toolchain/Linux.cmake` (ровно как пресет
   `ninja-llvm`). Если что-то сломано — сборка образа упадёт здесь;
2. **deps** — сборка всех зависимостей (`CXX=clang++ cmake -P linux-build.cmake`);
3. **final** — toolchain + `/deps/linux-clang23-x86_64` + `ENV`.

В образе выставлены:

- `PATH` — включает `/opt/llvm/bin`, `/opt/cmake/bin`, `/opt/vulkan/x86_64/bin`
- `VULKAN_SDK=/opt/vulkan/x86_64`
- `CMAKE_PREFIX_PATH=/deps/linux-clang23-x86_64` — пресет `ninja-llvm`
  находит зависимости без дополнительных флагов
- `OM_DEPS_DIR=/deps/linux-clang23-x86_64` — информационно

## Локальная проверка: сборка проекта в контейнере

Из корня репозитория:

```sh
docker run --rm -u "$(id -u):$(id -g)" -e HOME=/tmp \
    -v "$PWD":/src -w /src \
    ghcr.io/leanid/om-deps:linux-clang23 \
    bash -c 'cmake --preset ninja-llvm \
        && cmake --build --preset ninja-llvm --config RelWithDebInfo'
```

Замечания:

- `-u "$(id -u):$(id -g)"` — чтобы файлы в `./build` (и генерируемые `*.spv`
  в исходниках 02-vulkan) не создавались от root.
- Зависимости берутся из образа (`CMAKE_PREFIX_PATH`), хостовая папка
  `deps/prebuilt` не нужна; если она есть — проект предпочтёт её (корневой
  `CMakeLists.txt` ставит её первой), поэтому для чистой проверки образа
  её можно временно переименовать.
- Тесты: `ctest --test-dir build/ninja-llvm --output-on-failure`. Имейте
  в виду, что в контейнере нет дисплея/GPU — GUI-примеры (SDL/Vulkan/OpenGL)
  могут падать; консольные тесты должны проходить.

Просто зайти внутрь и осмотреться:

```sh
docker run --rm -it -v "$PWD":/src -w /src ghcr.io/leanid/om-deps:linux-clang23
```

## Публикация в GHCR

Нужен Personal Access Token с правом `write:packages`:

```sh
echo "$GITHUB_TOKEN" | docker login ghcr.io -u leanid --password-stdin
docker push ghcr.io/leanid/om-deps:linux-clang23
```

После пуша сделать пакет публичным (или выдать доступ репозиторию):
<https://github.com/users/leanid/packages> → пакет `om-deps` →
*Package settings → Change visibility*.

## Использование в GitHub Actions

Workflow добавим отдельным шагом после проверки образа. Минимальный пример:

```yaml
jobs:
  build:
    runs-on: ubuntu-latest
    container:
      image: ghcr.io/leanid/om-deps:linux-clang23
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake --preset ninja-llvm
      - name: Build
        run: cmake --build --preset ninja-llvm --config RelWithDebInfo
```

## Обновление версий

Все версии — `ARG` в начале Dockerfile (`LLVM_VERSION`, `CMAKE_VERSION`,
`NINJA_VERSION`, `VULKAN_SDK_VERSION`, `ICU_VERSION`, `ALT_TAG`). При смене
мажорной версии clang поменяется и триплет зависимостей
(`linux-clang23-x86_64` → `linux-clang24-x86_64` и т.п.) — не забудьте обновить
тег образа и путь `/deps/...` в `ENV`/`COPY` (и в этом readme).

Пересборка с нуля без кэша:

```sh
docker build --no-cache -f deps/docker/Dockerfile \
    -t ghcr.io/leanid/om-deps:linux-clang23 .
```

## Известные ограничения

- Только x86_64 Linux. Windows/Android/macOS вне scope этого образа.
- `lld` из tarball'а LLVM слинкован с `libicu*.so.70`, поэтому в образе
  собран ICU 70.1 (`/opt/icu70`); системный ICU (74 в p11) не используется.
- Пути к crt/libgcc от GCC вшиты в `/etc/clang/om.cfg` и подставляются через
  `--config` обёртками `/opt/llvm/bin/clang{,++}` (настоящие бинарники в
  `/opt/llvm/libexec`; официальный tarball LLVM игнорирует `/etc/clang` и
  прочие дефолтные конфиги — работает только `--config <file>`). Обёртки
  обязаны лежать внутри дерева LLVM: `clang-scan-deps` вычисляет пути к
  заголовкам libc++ от расположения компилятора, иначе ломается `import std`.
  Аналог того, что на хосте делает `cmake/toolchain/Linux.cmake`; голый
  `clang++` в контейнере линкует корректно.
- JACK-девел пакет в p11 отсутствует — SDL3 собирается без JACK.
- В `vulkan.cppm` внутри образа закомментирован `export import :video;`
  (sed в шаге 5) — в точности как на машине автора: пример
  `02-vulkan/09-vk-res-loading-1` компилирует `vulkan.cppm` без
  `vulkan_video.cppm`, и clang не находит partition `vulkan:video`. Если
  когда-нибудь примеры начнут собирать `vulkan_video.cppm`, этот sed можно
  убрать.
- Размер финального образа — порядка 4–5 ГБ (не сжатый).
