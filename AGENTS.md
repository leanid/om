# AGENTS.md — OM Project

Instructions for AI coding agents working in this repository.

## Project Overview

**OM** (🕉) is a monorepo of ~160 self-contained tutorials for modern C++ game development.
CMake project name: `rgd`. Language: **C++23** with experimental `import std`
(`CMAKE_CXX_MODULE_STD=1` set globally in root `CMakeLists.txt`).

Stack: SDL3, Boost, GLM, OpenGL ES 3.0, Vulkan 1.3, Slang shaders, web backends (Boost.Beast / httplib + Redis).

Docs and comments are often in **Russian**; code identifiers and agent instructions are in **English**.

## Repository Map

| Directory                              | Content                                                                      | In root CMake             |
|----------------------------------------|------------------------------------------------------------------------------|---------------------------|
| `00-basic-bash/`                       | Bash tutorials (2)                                                           | no                        |
| `00-basic-prog/`                       | Foundational C++ (44 examples: types, memory, threading, modules, test fwks) | yes (`OM_BASIC_PROG`)     |
| `01-basic-game-dev/`                   | Game-dev course (37): SDL3, software renderer, OpenGL, ImGui, DLL hot-reload | yes (`OM_BASIC_GAME_DEV`) |
| `02-opengl/`                           | LearnOpenGL-style GLES 3.0 (58 examples)                                     | yes (`OM_OPENGL`)         |
| `02-vulkan/`                           | Vulkan 1.3 + C++ modules + Slang (19 examples, `01-`…`17-`, some multi-part) | yes (`OM_VULKAN`)         |
| `03-network/`, `04-physics/`           | Placeholders                                                                 | no                        |
| `05-game/`                             | Experimental match3 engine (legacy SDL2/FetchContent)                        | OFF (`OM_GAME_EXAMPLE`)   |
| `06-latex/`, `07-octave/`, `09-elisp/` | Ancillary tutorials                                                          | no                        |
| `08-web/`                              | nginx/FastCGI, HTTP, Redis dashboard (3 examples)                            | yes (`OM_WEB`)            |
| `cmake/`                               | Shared packages: `om-common-functions`, `om-triplet-name`, `toolchain/`      |                           |
| `deps/`                                | Two-stage dependency build → `deps/prebuilt/<triplet>/`                      |                           |
| `support/`                             | Dev-environment docs (Emacs, Docker, Vulkan, Android)                        |                           |

## Build System

### Prerequisites

- **CMake 4.4+** (root requires `4.4.0`), Ninja
- Toolchain pinned in `mise.toml`: cmake 4.4.3, LLVM 21.1.8, ninja 1.13.2, python 3.14.5
- `gcc` must be on PATH **even for clang builds** — `cmake/toolchain/Linux.cmake` runs
  `gcc -print-libgcc-file-name` to locate crt/libgcc paths
- Vulkan SDK with `slangc` on `PATH` for `02-vulkan/`

### `import std` is gated by a CMake-version-locked UUID

- Root `CMakeLists.txt:14` sets `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` to
  `f35a9ac6-8463-4d38-8eec-5d6008153e7d` — valid **only for CMake 4.4.x**
  (verified against the mise-pinned 4.4.3 binaries).
- When bumping CMake, the UUID must be replaced with the new release's value
  (see `Help/dev/experimental.rst` in the CMake distribution) or configure fails.
- The `ninja-llvm` preset in `CMakePresets.json` carries a **different, stale UUID**
  (`451f2fe2-…`) as a cache variable. The root `set()` shadows it, so builds work —
  but never copy the preset value, and clean it up when touching this area.

### Two-Stage Build (default path)

Dependencies are **not** fetched at configure time; they must exist in `deps/prebuilt/<triplet>/`.

```sh
# Stage 1 — build deps (compiler must match the main preset)
cd deps/rules
CXX=clang++ cmake -P linux-build.cmake

# Stage 2 — configure and build
cmake . --preset ninja-llvm
cmake --build --preset ninja-llvm --config Debug
```

Triplet (e.g. `linux-clang21-x86_64`) is derived from compiler id + major version by
`cmake/om-triplet-name-config.cmake`. **If `deps/prebuilt/` is empty, configure fails**
at `find_package(SDL3)` etc.

### vcpkg preset — Boost only, not a full alternative

`vcpkg.json` provides **only Boost 1.90 + openssl + icu**. Every other dependency
(SDL3, GLM, Catch2, GTest, …) still comes from `deps/prebuilt/<triplet>`, so the
two-stage build is still required even with `--preset vcpkg`.

### Presets

- Recommended: `ninja-llvm` (clang + libc++ + LLD via `cmake/toolchain/Linux.cmake`,
  modules scanning ON). Others: `ninja`, `ninja-g++`, `ninja-multi`, `ninja-clang`,
  `vcpkg`, `vcpkg-msvc`, `vcpkg-llvm`, `xcode`, `vs2026-x64`, `mingw`.
- Build dir is always `build/<preset>`.
- A custom **`Profile`** config exists (RelWithDebInfo + `-p`, `_profile` postfix),
  added by `om_add_custom_build_types()`.

### Single target / tests

```sh
cmake --build build/ninja-llvm --config Debug --target 12-vk-zbuffer
ctest --test-dir build/ninja-llvm --output-on-failure
```

Section toggles at configure time: `-DOM_VULKAN=OFF` etc. (see root `CMakeLists.txt`).

## Coding Standards

Detailed rules live in `.cursor/rules/` (auto-applied). Summary:

- **snake_case** for all identifiers; **UPPER_SNAKE_CASE** only for macros
- All code in `namespace om` (or nested); `main()` is the only global exception
- Modern C++: `auto`, smart pointers, `std::span`, ranges, `std::print`
- New Vulkan/advanced code: **`import std`** instead of `#include <iostream>`
- C++ modules: `export module name;` in `.cxx`, registered via `FILE_SET CXX_MODULES`
- 80 columns, 4-space indent, Allman braces (`.clang-format`, `.editorconfig`)
- Extensions: **`.cxx` / `.hxx`** (not `.cpp` / `.h`)
- **Mandatory after edits:** `clang-format -i <file>` on every touched C++ file
- Many targets use `-Wall -Wextra -pedantic -Werror` — fix all warnings

## Section-Specific Guidance

### `00-basic-prog/`

- Standalone mini-projects; C++ standard varies (C++20–23) — match the example's `CMakeLists.txt`
- Test-framework tutorials: doctest (`34-`), Catch2 (`35-`), GTest/GMock (`36-`)

### `01-basic-game-dev/`

- Progressive course (`readme.md` = curriculum); engine↔game DLL boundary,
  `create_game(om::engine&)` pattern for hot-reload
- Older examples use C++20; do not upgrade unless asked

### `02-opengl/`

- GLES 3.0 via SDL3 + glad; shaders in `shaders/*.glsl`; some `android-gradle/` subprojects
- Copy patterns from the nearest numbered sibling

### `02-vulkan/`

- SDL3 surfaces, Boost `program_options` CLI, Vulkan 1.3
- Shaders: **Slang** (`.slang`) → SPIR-V via `om_add_slang_shader_target(generate_spirv_N …)`;
  `.spv` files are generated **into the source dir** next to the shader (gitignored) —
  don't move/delete them, examples load them at runtime from there
- UBO/SSBO structs: `alignas(16)` to match Slang/Vulkan layout
- Examples 09–17 use C++23 modules heavily — follow `12-vk-zbuffer/` as reference

### `08-web/`

- `03-redis-web` has two variants: `main.cxx` (httplib) and `main_boost.cxx` (Boost.Beast coroutines)
- External services (nginx, Redis) needed at runtime — check per-example `readme.md`

## Adding or Modifying Examples

1. **Copy the nearest sibling** — numbering, CMake structure, naming all matter
2. One `CMakeLists.txt` per tutorial; keep examples self-contained
3. **Target name = directory name** (e.g. `12-vk-zbuffer`)
4. Register in the section `CMakeLists.txt` via `add_subdirectory()`
5. Do not reformat vendored code (ImGui, glad, etc.)
6. Minimize scope — tutorials are teaching artifacts; avoid over-abstraction

## Agent Constraints

### Git — read-only (from `.cursor/rules/no-git-commits.mdc`)

- **Never** `git add`, `git commit`, `git push`, `git rm`, or any git state change
- Read-only (`git status`, `git diff`, `git log`) is fine; the user commits manually

### Do not

- Rewrite unrelated examples when fixing one tutorial
- Bulk-rename identifiers across the monorepo
- Change global CMake option defaults without explicit request
- Add dependencies to root `CMakeLists.txt` without updating `deps/rules/` too
- Trust `.gitignore` surprises: it ignores `build/`, `deps/prebuilt/`, `*.spv`,
  `compile_commands.json` — and **all dotfiles** (`.*`)

## Common Pitfalls

| Problem                              | Cause                                     | Fix                                                                     |
|--------------------------------------|-------------------------------------------|-------------------------------------------------------------------------|
| `find_package(SDL3)` fails           | Empty `deps/prebuilt/`                    | Run `deps/rules/linux-build.cmake` first (vcpkg preset won't help)      |
| Invalid `CXX_IMPORT_STD` UUID error  | CMake upgraded, UUID stale                | Update root `CMakeLists.txt:14` UUID for the new CMake version          |
| `slangc` not found                   | Vulkan SDK not on PATH                    | `source $VULKAN_SDK/setup-env.sh`                                       |
| Module import errors                 | Deps built with different compiler        | Rebuild deps with same compiler as preset (`ninja-llvm` → clang++)      |
| SPIR-V missing at runtime            | Shader target not wired                   | `add_dependencies(<target> generate_spirv_N)` in example CMakeLists     |
| libc++ link errors on Linux          | GCC used with llvm preset                 | Use `ninja-llvm` toolchain consistently                                 |
| Toolchain fatal about GCC lib path   | `gcc` not installed for clang build       | Install gcc — `cmake/toolchain/Linux.cmake` needs it for crt paths      |
| CI looks broken                      | It is: all CI configs are stale           | `.github/workflows/*`, `appveyor.yml`, `bitbucket-pipelines.yml` reference old layouts / skip the deps stage — don't use as build recipes |

## Key References

| File                                     | Purpose                                          |
|------------------------------------------|--------------------------------------------------|
| `readme.md`                              | Install hints, two-stage build (RU/EN)           |
| `CMakeLists.txt`                         | Root options, `import std` UUID, section toggles |
| `CMakePresets.json`                      | Presets; note stale UUID in `ninja-llvm`         |
| `cmake/om-common-functions-config.cmake` | `om_add_slang_shader_target`, clang-tidy, `Profile` build type |
| `.cursor/rules/`                         | Style guide, clang-format rule, no-git-commits   |
| `01-basic-game-dev/readme.md`            | Game-dev course curriculum                       |

## Quick Verification Checklist

After making C++ changes:

- [ ] `clang-format -i` on all modified `.cxx`/`.hxx` files
- [ ] Build the specific target: `cmake --build build/ninja-llvm --config Debug --target <name>`
- [ ] Run relevant test if the example has `add_test`
- [ ] No new compiler warnings (warnings are errors on many targets)
