# Device Log Viewer — Redis-based Real-Time Log System

---

## Slide 1: Обзор проекта

**Цель:** Создать систему сбора и просмотра логов с устройств в реальном времени.

**Компоненты:**
- **Клиент** (C++23) — отправляет логи с устройства в Redis
- **Сервер** (C++23) — HTTP/SSE сервер, отдаёт логи в браузер
- **Web UI** (HTML/CSS/JS) — интерфейс просмотра логов

**Стек:**
- Redis Streams / Hash / Sets
- redis-plus-plus (C++ Redis клиент)
- cpp-httplib (HTTP сервер)
- nlohmann_json (JSON)
- Server-Sent Events (SSE)

---

## Slide 2: Архитектура

```
┌─────────────┐     ┌───────────┐     ┌──────────────┐     ┌─────────┐
│  Device #1  │────▶│           │◀───▶│  Web Server  │◀───▶│ Browser │
│  (C++ клиент)│     │   Redis   │     │  (C++ SSE)   │     │  (JS)   │
├─────────────┤     │           │     │              │     ├─────────┤
│  Device #2  │────▶│  Streams  │     │  httplib +   │     │ Device  │
│             │     │  Hashes   │     │  redis++     │     │  List   │
├─────────────┤     │  Sets     │     │              │     │ Log View│
│  Device #N  │────▶│           │     │  Keyspace    │     │ Filters │
└─────────────┘     └───────────┘     │  Notify      │     │ Download│
                                      └──────────────┘     └─────────┘
```

---

## Slide 3: Структура данных в Redis

| Ключ                         | Тип    | Описание                        |
|------------------------------|--------|---------------------------------|
| `all_devices`                | HASH   | device_name → platform          |
| `log_names:<device>`         | SET    | Имена лог-файлов устройства     |
| `log:<device>:<log_name>`    | STREAM | Записи лога (поле `message`)    |

- Все ключи имеют **TTL = 3 суток**
- Keyspace Notifications (`KEA`) для real-time обновлений

---

## Slide 4: Клиент — отправка логов

```cpp
// Регистрация устройства (1 round-trip вместо N+1)
redis_client.hset("all_devices", device_name, platform);
redis_client.expire("all_devices", om::key_ttl);  // 72 часа

// Запись лога в стрим
redis_client.xadd(stream_key, "*",
    {std::make_pair("message", formatted_message)});
```

**Ключевые решения:**
- `HSET all_devices` вместо `SADD` + отдельных `HASH`-ей — устранён N+1 запрос
- Потокобезопасное время: `localtime_r` / `localtime_s` вместо `std::localtime`
- Форматирование через `std::format` (C++20)

---

## Slide 5: Web UI

```
┌──────────────────────────────────────────────────────┐
│ 📝 Device Log Viewer                                 │
├──────────┬──────────┬────────────────────────────────┤
│ Devices  │ Log files│ Log Output              ⬇️     │
│          │          │                                │
│ 🔍filter │ log_13.. │ [10:15:32.001] INFO Started    │
│ ☑ MacOS  │ log_12.. │ [10:15:32.045] DEBUG Init...   │
│ ☑ Android│ log_11.. │ [10:15:33.102] WARN Config..   │
│          │          │ [10:15:34.567] ERROR Failed..   │
│ ▸ device1│          │                                │
│   device2│          │            ☑ Auto-scroll        │
└──────────┴──────────┴────────────────────────────────┘
```

**Возможности:**
- Фильтр устройств по платформе (чекбоксы) и по имени (поиск)
- Подсветка уровней: INFO / WARNING / ERROR / DEBUG
- Auto-scroll, скачивание лога как текстовый файл
- XSS-защита: `escapeHtml()` перед `innerHTML`

---

## Slide 6: SSE — единое соединение

**Проблема:** Браузер ограничивает 6 TCP-соединений на домен (HTTP/1.1).
3 SSE-эндпоинта × 3 вкладки = 9 соединений → зависание.

**Решение:** Один SSE-эндпоинт `/api/stream` мультиплексирует все данные:

```
GET /api/stream?device=phone_01&log_name=log_13_03.txt

← event: devices_init     (список устройств)
← event: log_names_init   (список логов)
← event: logs_init         (первая порция логов)
← event: logs_new          (новые записи)
← event: devices_update    (устройство добавлено/удалено)
← event: log_names_update  (новый лог-файл)
← :                         (keepalive каждые 5 сек)
```

---

## Slide 7: Батчевая загрузка логов

**Проблема:** `XRANGE key "-" "+"` загружает ВСЕ записи в память.
500K записей → OOM на сервере, зависание браузера.

**Решение:** Пагинация на сервере, прозрачная для клиента:

```cpp
while (true) {
    redis_client_->xrange(stream_key_, start, "+",
                          batch_size_,              // 1000 записей
                          std::back_inserter(batch));
    if (batch.empty()) break;

    auto event = first_batch ? "logs_init" : "logs_new";
    send_sse(sink, event, items_to_json(batch), last_id);

    if (batch.size() < batch_size_) break;
}
```

JS-клиент не менялся — `logs_init` и `logs_new` обрабатываются как раньше.

---

## Slide 8: Last-Event-Id — восстановление после обрыва

**Проблема:** При обрыве SSE — все логи загружаются с нуля.

**Решение:**
1. Сервер отправляет `id:` с каждым логовым событием (Redis stream ID)
2. `EventSource` автоматически шлёт `Last-Event-Id` при переподключении
3. Сервер пропускает батчевую загрузку и стартует с этого ID

```
← id: 1710270038000-0
← event: logs_new
← data: [...]

--- обрыв / reconnect ---

→ Last-Event-Id: 1710270038000-0
← event: logs_new   (только новые записи)
```

---

## Slide 9: Исправление busy loop — xread

**Проблема:** Сервер потреблял 200%+ CPU даже без активности.

**Причина:** Неправильная перегрузка `xread`:
```cpp
// ❌ Вызывает xread(key, id, count=5000, output) — неблокирующий!
redis_client_->xread(key, id, 5000, std::back_inserter(data));

// ✅ Вызывает xread(key, id, timeout, output) — блокирующий BLOCK 5000
redis_client_->xread(key, id, std::chrono::milliseconds(5000),
                     std::back_inserter(data));
```

**Результат:** CPU с ~200% до **0.2%** при 5 вкладках.

Баг маскировался пулом Redis из 1 соединения — потоки стояли
на мьютексе пула вместо busy loop.

---

## Slide 10: Оптимизация — Redis Connection Pool

**Проблема:** 1 Redis-соединение на всех → 80 SSE-потоков стоят в очереди.

**Решение:** Настраиваемый пул соединений:

```cpp
redis::ConnectionPoolOptions pool_opts;
pool_opts.size = config.redis_pool_size;  // 128

auto redis_client = std::make_shared<redis::Redis>(
    redis::Uri(config.redis_url).connection_options(), pool_opts);
```

- Соединения создаются **лениво** (не 128 сразу)
- 80+ пользователей работают параллельно без contention

---

## Slide 11: Оптимизация — CV вместо polling

**Было:** Каждый SSE-поток каждые 2 сек делает round-trip к Redis.

**Стало:** Когда лог не выбран — поток спит на `condition_variable`:

```cpp
if (!stream_key_.empty()) {
    // Блокирующий xread — просыпается при новых данных
    redis_client_->xread(key, id, poll_interval_, ...);
} else {
    // Спит на CV — просыпается мгновенно при изменениях
    notifier_->wait_for_any_change(any_ver, poll_interval_);
}
```

---

## Slide 12: Конфигурация

Все параметры вынесены в `config.json`:

```json
{
    "redis_url":            "tcp://user:pass@host:6379",
    "server_host":          "0.0.0.0",
    "server_port":          8080,
    "public_dir":           "./08-web/03-redis-web/public",
    "max_threads":          256,
    "log_batch_size":       1000,
    "redis_pool_size":      128,
    "sse_poll_interval_ms": 5000
}
```

Redis: `maxmemory 256mb`, `maxmemory-policy allkeys-ttl`, `notify-keyspace-events KEA`

---

## Slide 13: Безопасность

| Проблема              | Решение                                        |
|-----------------------|------------------------------------------------|
| XSS через innerHTML  | `escapeHtml()` перед вставкой лог-сообщений    |
| SO_REUSEPORT          | Отключен — нельзя случайно запустить 2 сервера |
| Redis без пароля      | Поддержка аутентификации в URL                 |
| `std::localtime`      | Заменён на `localtime_r`/`localtime_s`         |
| CONFIG SET без прав   | Warning вместо `std::terminate()`              |

---

## Slide 14: Кросс-компиляция (Android NDK)

**Проблема:** `find_package` не находит проектный CMake-пакет при Android-сборке.

**Причина:** NDK toolchain ставит `CMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY`.

**Решение:** Точечный `-Dom-triplet-name_DIR=...` вместо глобального `BOTH`.

```cmake
# Кросс-компиляция: прокидываем toolchain в каждый ExternalProject
set(OM_CROSS_COMPILE_ARGS)
if(CMAKE_TOOLCHAIN_FILE)
    list(APPEND OM_CROSS_COMPILE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
endif()
```

---

## Slide 15: Итоги и метрики

| Метрика                     | До           | После          |
|-----------------------------|--------------|----------------|
| CPU idle (5 вкладок)        | ~200%        | 0.2%           |
| Max одновременных юзеров    | 2-3          | 80+            |
| Redis round-trips (devices) | N+1          | 1 (HGETALL)    |
| Загрузка 500K логов         | Всё в память | Батчами по 1K  |
| Reconnect                   | Всё с нуля   | С Last-Event-Id|
| TTL данных                  | Вечно        | 3 суток        |

**Технологии:** C++23, Redis Streams, SSE, CMake 4, Android NDK

---
