# AGENTS.md — OM Project

Instructions for AI coding agents working in this repository.

## Project Overview

**OM** (🕉) is a monorepo of ~155 self-contained tutorials for modern C++ game development.
CMake project name: `rgd` (Real Game Dev). Primary language: **C++23** with experimental `import std`.

Stack: SDL3, Boost, GLM, OpenGL ES 3.0, Vulkan 1.3, Slang shaders, optional web backends (Boost.Beast / httplib + Redis).

Documentation and comments are often in **Russian**; code identifiers and agent instructions are in **English**.

## Repository Map

| Directory                              | Content                                                                             | Default build            |
|----------------------------------------|-------------------------------------------------------------------------------------|--------------------------|
| `00-basic-prog/`                       | Foundational C++ (types, memory, threading, modules, crypto, test frameworks)       | ON (`OM_BASIC_PROG`)     |
| `01-basic-game-dev/`                   | Game-dev course: SDL3, software renderer, OpenGL, ImGui, engine/game DLL hot-reload | ON (`OM_BASIC_GAME_DEV`) |
| `02-opengl/`                           | LearnOpenGL-style GLES 3.0 tutorials (58 examples)                                  | ON (`OM_OPENGL`)         |
| `02-vulkan/`                           | Vulkan 1.3 tutorials with C++ modules and Slang shaders                             | ON (`OM_VULKAN`)         |
| `03-network/`, `04-physics/`           | Placeholders for future courses                                                     | —                        |
| `05-game/`                             | Experimental full game engine (`match3`)                                            | OFF (`OM_GAME_EXAMPLE`)  |
| `06-latex/`, `07-octave/`, `09-elisp/` | Ancillary tutorials (LaTeX, Octave, Emacs Lisp)                                     | not in root CMake        |
| `08-web/`                              | nginx/FastCGI, HTTP, Redis web dashboard                                            | ON (`OM_WEB`)            |
| `cmake/`                               | Shared CMake packages (`om-common-functions`, `om-triplet-name`, toolchains)        |
| `deps/`                                | Two-stage dependency build → `deps/prebuilt/<triplet>/`                             |
| `support/`                             | Dev environment docs (Emacs, Docker, Vulkan, Android, cxx utilities)                |

### Example Layout

Each tutorial is a numbered subdirectory, typically:

```
NN-topic-name/
├── CMakeLists.txt
├── main.cxx              # entry point
├── *.hxx                 # headers (when needed)
├── shaders/              # .glsl (OpenGL) or .slang (Vulkan)
└── readme.md             # per-example notes (common in 02-opengl)
```

Later Vulkan examples (09+) split into module libraries (`log.cxx`, `vulkan/`, `sdl/`).

## Build System

### Prerequisites

- **CMake 4.0+** (root requires 4.2)
- **Ninja** (recommended)
- Tool versions via `mise.toml`: cmake 4.3, LLVM 21, ninja 1.13, python 3.14
- Vulkan SDK with `slangc` on `PATH` (or `$VULKAN_SDK/bin`) for Vulkan tutorials

### Two-Stage Build (default path)

Dependencies are **not** fetched at configure time. They must exist in `deps/prebuilt/<triplet>/`.

```sh
# Stage 1 — build deps (compiler must match the main preset)
cd deps/rules
CXX=clang++ cmake -P linux-build.cmake

# Stage 2 — configure and build
cmake . --preset ninja-llvm
cmake --build --preset ninja-llvm --config Debug
```

Triplet example: `linux-clang21-x86_64`. CMake resolves it via `om-triplet-name`.

**If `deps/prebuilt/` is empty, configure fails** — SDL3, Boost, GLM, etc. will not be found.

### vcpkg Alternative

```sh
cmake . --preset vcpkg
cmake --build --preset vcpkg
```

No two-stage build; dependencies come from vcpkg (`vcpkg.json`).

### Build a Single Target

After configuring:

```sh
cmake --build build/ninja-llvm --config Debug --target 12-vk-zbuffer
```

### Run Tests

```sh
ctest --test-dir build/ninja-llvm --output-on-failure
```

### Toggle Sections

Pass CMake options at configure time, e.g. `-DOM_VULKAN=OFF` to skip Vulkan.

## Coding Standards

Detailed rules live in `.cursor/rules/`. Summary:

### Naming and Namespace

- **snake_case** for all identifiers (variables, functions, classes, namespaces)
- **UPPER_SNAKE_CASE** only for macros
- All code in `namespace om` (or `om::detail`); `main()` is the only global exception

### C++ Style

- Prefer modern C++ (C++20/23): `auto`, smart pointers, `std::span`, ranges, `std::print`
- New Vulkan and advanced code: **`import std`** instead of `#include <iostream>`
- C++ modules: `export module name;` in `.cxx` files, registered via `FILE_SET CXX_MODULES` in CMake
- 80-column limit, 4-space indent, Allman braces (see `.clang-format`, `.editorconfig`)
- File extensions: **`.cxx`** / **`.hxx`** (not `.cpp` / `.h`)

### Formatting (mandatory after edits)

```sh
clang-format -i path/to/modified_file.cxx
```

Run on every C++ file you touch. Config: `.clang-format`. Optional lint: `-DOM_CLANG_TIDY=ON`.

### Warnings

Many targets use `-Wall -Wextra -pedantic -Werror` (GCC/Clang) or `/W4 /WX` (MSVC). Fix all warnings.

## Section-Specific Guidance

### `00-basic-prog/`

- Standalone mini-projects; C++ standard varies (C++20–23) per example
- Testing tutorials: doctest (`34-`), Catch2 (`35-`), GTest/GMock (`36-`)
- Match the C++ standard declared in the example's `CMakeLists.txt`

### `01-basic-game-dev/`

- Progressive course with homework (`readme.md` describes curriculum)
- Teaches SDL3 dynamic/static linking, software rendering, engine↔game DLL boundary
- `create_game(om::engine&)` pattern for hot-reloadable game libraries
- Older examples may use C++20; do not upgrade unless asked

### `02-opengl/`

- GLES 3.0 via SDL3 + glad; shaders in `shaders/*.glsl`
- Some examples have `android-gradle/` subprojects
- Copy patterns from the nearest numbered sibling (lighting, shadows, instancing groups)

### `02-vulkan/`

- Vulkan 1.3, SDL3 for surfaces, Boost `program_options` for CLI
- Shaders: **Slang** (`.slang`) → SPIR-V via `om_add_slang_shader_target()`
- UBO/SSBO structs: use `alignas(16)` to match Slang/Vulkan layout rules
- Later tutorials (09–12): C++23 modules heavily — follow `12-vk-zbuffer/` as reference
- Requires `slangc` from Vulkan SDK

### `08-web/`

- `03-redis-web` has two variants: `main.cxx` (httplib) and `main_boost.cxx` (Boost.Beast coroutines)
- External services (nginx, Redis) needed at runtime — check per-example `readme.md`

### `05-game/`

- OFF by default; uses older FetchContent SDL2 — treat as legacy/experimental
- Architecture: `05-game/docs/architecture.md` (engine exe + game DLL, test-first)

## Adding or Modifying Examples

1. **Copy the nearest sibling** — numbering, CMake structure, and naming conventions matter
2. **Keep examples self-contained** — one `CMakeLists.txt` per tutorial, minimal cross-example deps
3. **Target name = directory name** (e.g. `12-vk-zbuffer` executable in `12-vk-zbuffer/`)
4. **Register in section `CMakeLists.txt`** via `add_subdirectory()`
5. **Do not edit `deps/prebuilt/`** or generated `.spv` files — they are gitignored
6. **Do not reformat vendored/third-party code** (ImGui, glad, etc.)
7. **Minimize scope** — tutorials are teaching artifacts; avoid over-abstraction

## Agent Constraints

### Git — read-only

- **Never** `git add`, `git commit`, `git push`, `git rm`, or other state-changing git commands
- Read-only commands (`git status`, `git diff`, `git log`) are fine
- User handles all commits manually (see `.cursor/rules/no-git-commits.mdc`)

### Files to avoid committing

- `build/`, `deps/prebuilt/`, `compile_commands.json`, `*.spv`, Android build artifacts
- Secrets, `.env`, credentials

### Do not

- Rewrite unrelated examples when fixing one tutorial
- Bulk-rename identifiers across the monorepo
- Change global CMake options defaults without explicit request
- Add dependencies to root `CMakeLists.txt` without updating `deps/rules/` too
- Edit `readme.md` badges or CI configs unless asked

## Common Pitfalls

| Problem                     | Cause                          | Fix                                                                |
|-----------------------------|--------------------------------|--------------------------------------------------------------------|
| `find_package(SDL3)` fails  | Empty `deps/prebuilt/`         | Run `deps/rules/linux-build.cmake` first, or use vcpkg preset      |
| `slangc` not found          | Vulkan SDK not on PATH         | `source $VULKAN_SDK/setup-env.sh`                                  |
| Module import errors        | Wrong compiler vs deps triplet | Rebuild deps with same compiler as preset (`ninja-llvm` → clang++) |
| SPIR-V missing at runtime   | Shader target not built        | Ensure `add_dependencies(target generate_spirv_N)`                 |
| libc++ link errors on Linux | GCC used with llvm preset      | Use `ninja-llvm` preset toolchains consistently                    |
| Windows SDL3 DLL missing    | Post-build copy not run        | Check `if(WIN32)` copy block in example CMakeLists                 |

## Key References

| File                                     | Purpose                                          |
|------------------------------------------|--------------------------------------------------|
| `readme.md`                              | Install hints, two-stage build (RU/EN)           |
| `CMakeLists.txt`                         | Root options, `import std`, section toggles      |
| `CMakePresets.json`                      | `ninja-llvm` (recommended), vcpkg variants       |
| `vcpkg.json`                             | Alternative dependency manifest                  |
| `cmake/om-common-functions-config.cmake` | `om_add_slang_shader_target`, clang-tidy helpers |
| `.cursor/rules/cpp-style-guide.mdc`      | snake_case, `om` namespace                       |
| `.cursor/rules/cpp-clang-format.mdc`     | Post-edit formatting                             |
| `support/emacs/doomemacs.md`             | Primary author IDE setup                         |
| `01-basic-game-dev/readme.md`            | Game-dev course curriculum                       |
| `05-game/docs/architecture.md`           | Engine architecture (experimental)               |

## Quick Verification Checklist

After making C++ changes:

- [ ] `clang-format -i` on all modified `.cxx`/`.hxx` files
- [ ] Build the specific target: `cmake --build build/ninja-llvm --config Debug --target <name>`
- [ ] Run relevant test if the example has `add_test`
- [ ] No new compiler warnings (project often treats warnings as errors)
