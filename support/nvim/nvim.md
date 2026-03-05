*how to configure NVIM editor*

**options**

```lua
```
    vim.opt.number = true
    vim.opt.relativenumber = false

    vim.opt.shiftwidth = 4

    if vim.fn.has("win32") == 1 then
        LazyVim.terminal.setup("pwsh")
    end
```
```

**'mini.files' plugin for file system fast lookup**

```
:LazyExtras
```
```

or restart nvim and select x

j / k — перемещение вверх/вниз.
h / l — зайти в папку (вправо) или выйти на уровень выше (влево).
g q   — закрыть проводник.
=     — применить изменения (создание файлов, переименование), которые вы напечатали текстом.
