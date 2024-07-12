# Note about commands in Doomemacs
# Installation problems
## Formatter for your language strange behavior
Add next to your Doomemacs .config. Or read about it in (format +on-save)
```lisp
(setq +format-with-lsp nil)
```
## CMake Presets not working
Add to configuration
```lisp
(projectile-enable-cmake-presets t)
```
## Search

### Search Project Text Globally
```SPC+/```

### Search current directory content (by text)
```emacs
SPC s d
```
works in treemacs and in any text buffer

### Search Project Text in files by type
```
SPC+/
example: opengl -- -tcmake
         ^^^^^^^^^^^^^^^^^ search "opengl" only in 
         files with "type" cmake (*.cmake, CMakeLists.txt)
```
        
### Search Project using case sensitive
```
SPC+/
example: OpenGL -- -tcmake -s
                            ^- case_sensitive
example: OpenGL -- -tcmake -s --multiline
                              ^^^^^^^^^^^-rg will do multiline search or use [-U]
example: OpenGL -- -tcmake -C3
                            ^- show [--context] around match
example: SPC\+/[[:space:]]*example -- -U -C3 -tmd
         ^^^^^^^^^^^^^^^^^^^^^^^^^ - emacs regexp example multiline and with context search only *.md
example: OpenGL -- -g *.rs
                   ^^^^^^^^^ - search only in Rust files if you need exacly file pattern
example: test -- --no-ignore
                 ^^^^^^^^^^^ - search ignore .gitignore file patterns
example: \(std::\)\|\(namespace\ std\) -- -tcpp --no-ignore
         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^1    ^^^2 ^^^^^^^^^^3
         1. regex to search "std::" or "namespace std"
         2. all c/c++ file types
         3. skip git-ignore rules (search everywhere)
example rg: rg --no-ignore --files -g "*pickling*"
                 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ will print only file path with names contains "pickling"
example (search only in file names): rg --files | rg "\w*.gradle$"
```
RipGrep manual here: 
[RipGrep User Guide](https://github.com/BurntSushi/ripgrep/blob/master/GUIDE.md)
Emacs regexp manual here:
[Emacs regexp](https://www.gnu.org/software/emacs/manual/html_node/emacs/Regexps.html)
Emacs regexp character classes:
[Character Classes](https://www.gnu.org/software/emacs/manual/html_node/elisp/Char-Classes.html)

### Search and replace in full project
TL;DR:
for ivy module users
```
SPC s p foo C-c C-e :%s/foo/bar/g RET Z Z
```
for Vertico module users (I use it most of the time)
```
SPC s p foo C-; E C-c C-p :%s/foo/bar/g RET Z Z
```
Entering those keys will replace “foo” with “bar” in your whole project. 

### How to disable format-on-save temporally in current buffer?
double use mode for reformating (will disable it)
```emacs
SPC+:  aphelia-mode
```
### How to show all keys and commands for some Doomemacs shortcuts
After you just started fire shortcut(prefix) and help not fit in help window and show you
(window 1 of 3)
like ```SPC+w``` and wait will show you (window 1 of 3) how to see all commands?
If you use ```vertico``` type ```?``` and you see all.


### How to toggle function signature to see arguments
```emacs
SPC c l h s
```
### How to toggle function overloaded versions
```emacs
M+n
M+p
```
### How to toggle list of all functions in current file
```emacs
SPC c S
q - to exit
```
### How to debug using Doom Emacs on Linux?
start debugging with
```emacs
SPC : gdb
```
load binary you wish to debug
```emacs
file ./../../build/llvm-ninja/02-vulkan/08-vk-framebuffer-cmd/08-vk-framebuffer
```
pass arguments to start like (r - run)
```emacs
gdb> r arg1 arg2 arg3
```
pass from terminal like
```emacs
>gdb --args executable_name arg1 arg2 arg3
```
show backtrace with command
```emacs
gdb>bt
```
stop on any exception throw 
```emacs
gdb>catch throw
```
