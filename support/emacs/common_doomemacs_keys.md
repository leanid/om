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

### Search Project File by type



