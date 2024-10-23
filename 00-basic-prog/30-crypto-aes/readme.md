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
2. зашифровать
3. расшифровать

Примеры:

``` bash

```

