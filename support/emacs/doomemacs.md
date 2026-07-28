# Note about commands in Doomemacs
# Installation problems
## doom sync, upgrade on windows in any shell
Try passing it to emacs manually. E.g.
- `doom sync` -> `emacs -q --no-site-file --script bin/doom -- sync`
- `doom upgrade` -> `emacs -q --no-site-file --script bin/doom -- upgrade` (`doom upgrade` won't be able to restart and run `doom sync` afterwards, by itself, so you'll have to `doom sync -u` manually after upgrading)
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
;; Использовать два словаря одновременно
(after! ispell
  ;; Multi-dictionary через hunspell backend
  (setq ispell-dictionary "en_US,ru_RU")

  (ispell-set-spellchecker-params)
  (ispell-hunspell-add-multi-dic "en_US,ru_RU"))
```
C+f+P+init.el
```elisp
  (spell +hunspell +flyspell +everywhere) ;
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
### How to use lates clang++ on Alt Linux?
0. simplest way is to use -L and -B options to say clang++ where to search
   libraries and object files from current system
   ```bash
   cmake . --preset ninja-llvm \
   -DCMAKE_CXX_FLAGS="-B/usr/lib64/gcc/x86_64-alt-linux/13/ -L/usr/lib64/gcc/x86_64-alt-linux/13/" --fresh
   ```
   if you need install llvm - use **mise tool** with **mise.toml** file in root dir
1. download & unpack llvm
2. you may need to add several static libs and ld scripts from your distro
   if clang++ can't find it
3. if you see error like no gcc gcc_s libunwind etc
4. create soft links like :
```bash
# inside $LLVM_ROOT/lib/x86_64-unknown-linux-gnu/
crtbeginS.o    -> /usr/lib64/gcc/x86_64-alt-linux/14/crtbeginS.o
crtendS.o      -> /usr/lib64/gcc/x86_64-alt-linux/14/crtendS.o
libgcc.a       -> /usr/lib64/gcc/x86_64-alt-linux/14/libgcc.a
libgcc_s.so    -> /usr/lib64/gcc/x86_64-alt-linux/14/libgcc_s.so
libstdc++exp.a -> /usr/lib64/gcc/x86_64-alt-linux/14/libstdc++exp.a
```
5. Add to `LD_PRELOAD_PATH` like:
```zsh
# insize ~/.zshrc
# ...
export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:\
/home/leo/LLVM-21.1.1-Linux-X64/lib/x86_64-unknown-linux-gnu"
```
6. now both g++ and clang++ works on same machine
### How to debug using Dape in C++
1. ```SPC+d+d```
2. put all your params and args in minibufer like:
```elisp
Run adapter: lldb-dap
:program "~/.build/ninja-g++/02-vulkan/09-vk-res/Debug/09-vk-res"
:args ["-v" "-l"]
:cwd "."
```
### How to visualize vertical wrap line?
Doom emacs: `SPC+t+c` Fill Column indicator
### How to format text block in ORG mode?
select block of text and `M-q`
if you need to set 80 characters `C-x f 80 RET`
### How to change file encodings and line endings?
```M+:``` and then: `set-buffer-file-coding-system`
### How to prepare doomemacs for l18n (localization) PO po-mode
1. install system package with `po-mode.el` file
2. search it on Fedora like `dnf provides "*/po-mode.el"`
3. on Fedora it is: `emacs-gettext-0.22.5-6.fc41.noarch`
4. now you restart emacs `SPC+q+R` or exit emacs `doom sync`
5. open emacs and `M+:` and then `po-mode` - should be found
### How to use online translation in emacs buffer
1. add package gt (go-translate) to doom/packages
```elisp
    (package! gt) ;; go-translate
```
2. add config to doom/config like:
```elisp
    (after! gt
      (setq gt-langs '(en ru))
      (setq gt-default-translator (gt-translator :engines (gt-google-engine)
                                                 :render (gt-buffer-render)
                                                 ))
    )
```
3. doom sync
4. select word then `M+: gt-translate`
## How to use LLM in Doomeemacs
### Ollama use in Doom
1. install Ollama: `curl -fsSL https://ollama.com/install.sh | sh`
2. run your model (RTX 4080 works with): `ollama run deepseek-coder:33b`
3. test model is working
4. exit with: `/exit`
5. go to Doom config and add:
```elisp
;; --------------------- Ollama
(use-package! gptel
  :config
  (setq gptel-model 'deepseek-coder:33b) ;; 'deepseek-coder:33b ;;'qwen2.5-coder:7b
  (setq! gptel-backend (gptel-make-ollama "Ollama"
                         :host "localhost:11434"
                         :stream t
                         :models '(deepseek-coder:33b))))
;; ---------------------- end Ollama
```
6. go to Doom init and uncomment:
```
llm
```
7. now exit Doom and then: `doom sync` and `doom doc`
### Aider use in Doom
1. install aider:
```bash
python -m pip install aider-install
aider-install
```
2. add to packages.el
```elisp
(package! aidermacs)
```
3. add to config.el
```elisp
(use-package! aidermacs
  :defer t
  :config
  ;; Задаем адрес локальной Ollama для Aider
  (setenv "OLLAMA_API_BASE" "http://127.0.0.1:11434")
  ;; Указываем Aider использовать локальную Ollama и конкретную модель
  (setq aidermacs-args '("--model" "ollama/qwen2.5-coder:7b")))

;; Привязываем клавиши глобально через стандартный механизм Doom
(map! :leader
      (:prefix-map ("a" . "AI/Aider")
       :desc "Run Aider"               "m" #'aidermacs-transient-menu
       :desc "Aider add current file"  "f" #'aidermacs-add-current-file
       :desc "Aider reset context"     "r" #'aidermacs-reset))
```
4. start play with it!

### How to debug CMakeLists.txt with DAP(a) in Doomemacs
1. SPC + f + P - config
2. add config for cmake-debug:
```elisp
(after! dape
  ;;(setq dape-buffer-log-level 'io)

  ;; CMake DAP speaks only over a unix domain socket (--debugger-pipe is
  ;; mandatory). Dape supports stdio or TCP, not unix sockets, so we launch a
  ;; small socat bridge: ~/.config/doom/bin/cmake-dape-adapter
  ;;
  ;; Minibuffer overrides (same style as lldb/debugpy):
  ;;   cmake-debug :cwd "/path/to/src" :preset "ninja-llvm"
  ;;   cmake-debug :cwd "." :args ["-S" "." "-B" "build" "-DFOO=1"]
  ;;   cmake-debug :sourceDir "." :binaryDir "build" :preset "ninja-clang"
  (defun +dape-cmake-debug-config (config)
    "Build cmake CLI from :cwd/:preset/:args/:sourceDir/:binaryDir."
    (let* ((base-cwd (or (plist-get config 'command-cwd) default-directory))
           (cwd (plist-get config :cwd))
           (preset (plist-get config :preset))
           (source-dir (plist-get config :sourceDir))
           (binary-dir (plist-get config :binaryDir))
           (extra (append (plist-get config :args) nil))
           (args '()))
      (when (and (stringp cwd) (not (string-empty-p cwd)))
        (setq config (plist-put config 'command-cwd
                                (expand-file-name cwd base-cwd))))
      (when (and (stringp source-dir) (not (string-empty-p source-dir)))
        (setq args (append args (list "-S" source-dir))))
      (when (and (stringp binary-dir) (not (string-empty-p binary-dir)))
        (setq args (append args (list "-B" binary-dir))))
      (when (and (stringp preset) (not (string-empty-p preset)))
        (setq args (append args (list "--preset" preset))))
      (setq args (append args extra))
      (setq config (plist-put config 'command-args args))
      ;; These are only for building the cmake CLI; strip before DAP launch.
      (dolist (key '(:cwd :preset :sourceDir :binaryDir :args))
        (setq config (map-delete config key)))
      config))

  (add-to-list 'dape-configs
               `(cmake-debug
                 modes (cmake-mode cmake-ts-mode)
                 ensure dape-ensure-command
                 command-cwd dape-command-cwd
                 command ,(expand-file-name "bin/cmake-dape-adapter" doom-user-dir)
                 fn +dape-cmake-debug-config
                 :type "cmake"
                 :request "launch"
                 :cwd "."
                 :preset ""
                 :sourceDir ""
                 :binaryDir ""
                 :args [])))
```
3. copy into `~/.config/doom/bin/cmake-dape-adapter` next content file and make it executable:
```bash
#!/usr/bin/env bash
# Bridge CMake's unix-socket DAP (--debugger-pipe) to stdio for Emacs dape.
# Usage: cmake-dape-adapter [cmake args...]
# Adds --debugger --debugger-pipe automatically.
set -euo pipefail

if ! command -v socat >/dev/null 2>&1; then
  echo "cmake-dape-adapter: socat is required" >&2
  exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "cmake-dape-adapter: cmake not found in PATH" >&2
  exit 1
fi

sock="${XDG_RUNTIME_DIR:-/tmp}/cmake-dape-$$.sock"
rm -f "$sock"
cmake_pid=
socat_pid=

cleanup() {
  if [[ -n "${socat_pid}" ]] && kill -0 "$socat_pid" 2>/dev/null; then
    kill "$socat_pid" 2>/dev/null || true
    wait "$socat_pid" 2>/dev/null || true
  fi
  if [[ -n "${cmake_pid}" ]] && kill -0 "$cmake_pid" 2>/dev/null; then
    kill "$cmake_pid" 2>/dev/null || true
    wait "$cmake_pid" 2>/dev/null || true
  fi
  rm -f "$sock"
}
trap cleanup EXIT

# Keep cmake chatter on stderr so stdout stays a clean DAP channel for dape.
cmake "$@" --debugger --debugger-pipe "$sock" >&2 &
cmake_pid=$!

for _ in $(seq 1 200); do
  if [[ -S "$sock" ]]; then
    break
  fi
  if ! kill -0 "$cmake_pid" 2>/dev/null; then
    wait "$cmake_pid" || true
    echo "cmake-dape-adapter: cmake exited before creating debugger socket" >&2
    exit 1
  fi
  sleep 0.05
done

if [[ ! -S "$sock" ]]; then
  echo "cmake-dape-adapter: timed out waiting for $sock" >&2
  exit 1
fi

# Non-interactive bash redirects "&" stdin from /dev/null — keep real stdio via fd copies.
exec {dape_in}<&0 {dape_out}>&1
socat STDIO "UNIX-CONNECT:$sock" <&$dape_in >&$dape_out &
socat_pid=$!
exec {dape_in}<&- {dape_out}>&-

status=0
wait "$cmake_pid" || status=$?
kill "$socat_pid" 2>/dev/null || true
wait "$socat_pid" 2>/dev/null || true
socat_pid=
cmake_pid=
exit "$status"
```

