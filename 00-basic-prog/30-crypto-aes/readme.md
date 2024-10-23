# Пример синхронного шифрования с помощью OpenSSL
## Берем файлик и с помощью OpenSSL шифруем и дешифруем

``` bash
openssl enc -salt -S "cc6d4b31bc04f5820f4e27bc3ddbff72" -saltlen 16 \
-aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml -out ru.yaml.enc.ctr
```

Тут важно обратить внимание, на 3 вещи:
1. мы используем парольную фразу: -pass pass:leanid
2. используем -pbkdf2 алгоритм генерации хэша 
3. используем соль на 16 байт в HEX формате строки
4. используем -aes-128-ctr алгоритм шифрования

Что бы расшифровать нужно просто выбрать нужный файлик и к прошлым
командам добавить флаг -d пример:

``` bash
openssl enc -d -salt -S "cc6d4b31bc04f5820f4e27bc3ddbff72" -saltlen 16 \
-aes-128-ctr -pass pass:leanid -pbkdf2 -in ru.yaml.enc.ctr -out ru.yaml.enc.ctr.dec
```

## Повторяем тоже самое только сами

У нас есть 3 режима:
1. сгенерироваь соль - gen_salt
2. зашифровать - enc
3. расшифровать - dec
4. если забыли - вызовите --help

Примеры:

``` bash
aes128 gen_salt
b56c299fff844e6b477bd983a5bb5a1c
```

``` bash
aes128 enc --pass leanid --salt "b56c299fff844e6b477bd983a5bb5a1c" -i some.file -o some.file.enc
aes128 dec --pass leanid --salt "b56c299fff844e6b477bd983a5bb5a1c" -i some.file.enc -o some.file.dec
diff some.file some.file.dec # should be same
```

## Заметки
1. Если входной файл был пустым - то выходной тоже будет пустым
2. выходной файл будет выровнен по 16 байт, т.к. алгоритм aes так работает
