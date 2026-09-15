# C++ Formatting with clang-format

After modifying any C++ file (`*.cxx`, `*.hxx`, `*.cpp`, `*.hpp`, `*.cc`, `*.h`),
run `clang-format -i <file>` on it before considering the task complete.

```bash
clang-format -i path/to/modified_file.cxx
```

- Apply to every C++ file you have edited in the current session.
- Run from the project root.
- Do this after all edits are done, before building or committing.
