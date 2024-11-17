# Note about commands in Doomemacs
# Installation problems
## doom sync, upgrade on windows in any shell
Try passing it to emacs manually. E.g.
- `doom sync` -> `emacs -q --no-site-file --script bin/doom -- sync`
- `doom upgrade` -> `emacs -q --no-site-file --script bin/doom -- upgrade` (`doom upgrade` won't be able to restart and run `doom sync` afterwards, by itself, when used this way, so you'll have to `doom sync -u` manually after upgrading)
## Formatter for your language strange behavior
Add next to your Doomemacs .config. Or read about it in (format +on-save)
```elisp
(setq +format-with-lsp nil)
```
## CMake Presets not working
Add to configuration
```elisp
(setq projectile-enable-cmake-presets t)
or
(projectile-enable-cmake-presets t)
```
## Quit without acknolidge
```elisp
(setq confirm-kill-emacs nil)
```
## Maximize on startup
To maximize or fullscreen Emacs at startup, add one of the following to ~/.doom.d/config.el:
```elisp
(add-to-list 'initial-frame-alist '(fullscreen . maximized))
(add-hook 'window-setup-hook #'toggle-frame-maximized)
(add-hook 'window-setup-hook #'toggle-frame-fullscreen)
```
Each method has slightly different effects that vary from OS to OS. You’ll have to decide for yourself which you prefer.
## Font on my install 
```elisp
(setq doom-font (font-spec :family "JetBrains Mono" :size 15 :weight 'semi-light)
      doom-variable-pitch-font (font-spec :family "JetBrains Mono" :size 15))
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
example (filter lines with names): locate sed.mo | rg -v flat | rg ru
                                                       ^ - invert (filter) regexp
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
### How to use Doomemacs as calculator?
1. go to *scratch* buffer
2. type `(+ 1 1)` and leave cursor at last `)`
3. execute `C+x C+e` see result in echo area
4. if you got error `guile` version mismatch
5. go to variable `SPC+h+v` and type like `guile-binary`
6. edit to your guile binary name like guile3.0
## How to see buffer encoding and EOL(end of line)
1. open doom config (SPC+f+P+config)
2. add line:
```emacs
;; Whether display the buffer encoding.
(setq doom-modeline-buffer-encoding t)
```
3. result will look like: CRLF UTF-8
### How to use multiple dictionaries for spell checking in Doomemacs?
Just see: https://emacs.stackexchange.com/questions/21378/spell-check-with-multiple-dictionaries
or copy code and paste into doom/config.el (C+f+P+config)
```elisp
(with-eval-after-load "ispell"
  (setq ispell-program-name "hunspell")
  (setq ispell-dictionary "en_US,ru_RU")
  ;; ispell-set-spellchecker-params has to be called
  ;; before ispell-hunspell-add-multi-dic will work
  (ispell-set-spellchecker-params)
  (ispell-hunspell-add-multi-dic "en_US,ru_RU"))
```
### How to debug Python code
Install *debugpy*
```bash
pip install "debugpy"
```
Add to your doomemacs config next code:
```elisp
(require 'dap-python)
(after! dap-mode
  (setq dap-python-debugger 'debugpy))
```
next *doom sync*
And now you should see Python::Run Configuration on
dap-debug-edit-template
For simple one file debugging you can skip most params see example:
```elisp
(dap-register-debug-template
  "Python :: Run file (buffer) my"
  (list :type "python"
        :args ""
        :cwd "/home/leo/om/00-basic-prog/31-python/hello"
        :module nil
        :program nil
        :request "launch"
        :name "Python :: Run file (buffer) my"))
```
next *dap-hydra* to show key to debug
