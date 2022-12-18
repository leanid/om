# Note about commands in Doomemacs
# Instalation problems
## Formater for your language strange behaviour
Add next to your doom emacs config. Or read about it in (format +on-save)
```lisp
(setq +format-with-lsp nil)
```
## CMake Presets not working
Add to configuration
```lisp
(projectile-enable-cmake-presets t)
```
## Search

### Search Project Text Globaly

SPC+/

### Search Project Text in files by type
SPC+/
example: opengl -- -tcmake
         ^^^^^^^^^^^^^^^^^ search "opengl" only in 
         files with "type" cmake (*.cmake, CMakeLists.txt)


