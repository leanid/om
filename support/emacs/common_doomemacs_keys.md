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
