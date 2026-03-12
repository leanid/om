document.addEventListener('DOMContentLoaded', () => {
    const devicesList = document.getElementById('devices-list');
    const streamsList = document.getElementById('streams-list');
    const logsContainer = document.getElementById('logs-container');
    const platformFiltersContainer = document.getElementById('platform-filters');
    
    const currentDeviceLabel = document.getElementById('current-device-label');
    const currentStreamLabel = document.getElementById('current-stream-label');
    
    const clearLogsBtn = document.getElementById('clear-logs-btn');
    const downloadLogsBtn = document.getElementById('download-logs-btn');
    const autoscrollCb = document.getElementById('autoscroll-cb');
    const deviceSearchInput = document.getElementById('device-search');

    let allDevicesData = []; // Храним полный список {name, platform}
    let activePlatforms = new Set(); // Какие платформы сейчас выбраны
    let searchQuery = ''; // Текущий поисковый запрос
    
    let currentDevice = null;
    let currentStream = null;
    
    let devicesEventSource = null;
    let streamsEventSource = null;
    let logsEventSource = null;

    // Форматирование времени из миллисекунд (только время для компактности)
    function formatTime(timestampMs) {
        if (!timestampMs) return '??:??:??';
        const date = new Date(parseInt(timestampMs));
        return date.toLocaleTimeString('en-US', { hour12: false }) + '.' + String(date.getMilliseconds()).padStart(3, '0');
    }

    // Вспомогательная функция для рендера списков
    function renderList(container, items, onSelect, activeItem) {
        container.innerHTML = '';
        if (items.length === 0) {
            container.innerHTML = '<div class="empty-state">No items found</div>';
            return;
        }

        items.forEach(item => {
            const div = document.createElement('div');
            div.className = `list-item ${item === activeItem ? 'selected' : ''}`;
            div.textContent = item;
            div.addEventListener('click', () => onSelect(item));
            container.appendChild(div);
        });
    }

    // Отрисовка чекбоксов платформ
    function renderPlatformFilters(platforms) {
        platformFiltersContainer.innerHTML = '';
        
        if (platforms.size === 0) {
            platformFiltersContainer.style.display = 'none';
            return;
        }
        
        platformFiltersContainer.style.display = 'flex';

        platforms.forEach(platform => {
            const label = document.createElement('label');
            label.className = 'filter-label';
            
            const checkbox = document.createElement('input');
            checkbox.type = 'checkbox';
            checkbox.value = platform;
            checkbox.checked = activePlatforms.has(platform);
            
            checkbox.addEventListener('change', (e) => {
                if (e.target.checked) {
                    activePlatforms.add(platform);
                } else {
                    activePlatforms.delete(platform);
                }
                filterAndRenderDevices();
            });

            label.appendChild(checkbox);
            label.appendChild(document.createTextNode(platform));
            platformFiltersContainer.appendChild(label);
        });
    }

    // Фильтрация и отрисовка устройств
    function filterAndRenderDevices() {
        // Фильтруем устройства по выбранным платформам и поисковому запросу
        const filteredDevices = allDevicesData
            .filter(d => activePlatforms.has(d.platform))
            .filter(d => d.name.toLowerCase().includes(searchQuery.toLowerCase()))
            .map(d => d.name)
            .sort(); // Алфавитная сортировка имен
            
        renderList(devicesList, filteredDevices, selectDevice, currentDevice);
    }

    // Получение списка устройств (SSE)
    function connectToDevicesStream() {
        if (devicesEventSource) {
            devicesEventSource.close();
        }

        devicesList.innerHTML = '<div class="empty-state">Connecting...</div>';
        devicesEventSource = new EventSource('/api/devices/stream');

        const handleDevicesData = (event) => {
            allDevicesData = JSON.parse(event.data);
            
            // Собираем все уникальные платформы
            const platforms = new Set();
            allDevicesData.forEach(d => {
                if (!d.platform) d.platform = 'Unknown';
                platforms.add(d.platform);
            });

            // Если появились новые платформы, которых не было раньше, включаем их по умолчанию
            platforms.forEach(p => {
                // Если мы только загрузились (activePlatforms пуст) или это реально новая платформа
                // (тут немного упрощенно: если мы сняли галочку, она удалилась из activePlatforms.
                // Чтобы не включать ее обратно при каждом апдейте, мы должны хранить "известные" платформы.
                // Но для простоты пока оставим так, или будем добавлять только если это первый инит).
            });
            
            // Лучше так: при первом 'init' добавляем все
            if (event.type === 'init') {
                platforms.forEach(p => activePlatforms.add(p));
            } else {
                // При апдейте добавляем только те, которых вообще нет в DOM
                const existingCheckboxes = Array.from(platformFiltersContainer.querySelectorAll('input[type="checkbox"]')).map(cb => cb.value);
                platforms.forEach(p => {
                    if (!existingCheckboxes.includes(p)) {
                        activePlatforms.add(p);
                    }
                });
            }

            renderPlatformFilters(platforms);
            filterAndRenderDevices();
        };

        devicesEventSource.addEventListener('init', handleDevicesData);
        devicesEventSource.addEventListener('update', handleDevicesData);

        devicesEventSource.addEventListener('error', (event) => {
            console.error('Devices SSE Error:', event);
        });
    }

    // Выбор устройства
    function selectDevice(device) {
        if (currentDevice === device) return;
        
        currentDevice = device;
        currentDeviceLabel.textContent = `(${device})`;
        
        // Обновляем выделение в списке
        Array.from(devicesList.children).forEach(el => {
            if (el.classList.contains('list-item')) {
                el.classList.toggle('selected', el.textContent === device);
            }
        });

        // Сбрасываем стрим
        currentStream = null;
        currentStreamLabel.textContent = '';
        downloadLogsBtn.style.display = 'none';
        closeLogStream();
        logsContainer.innerHTML = '<div class="empty-state">Select a stream to view logs</div>';

        connectToStreamsStream(device);
    }

    // Получение списка стримов (SSE)
    function connectToStreamsStream(device) {
        if (streamsEventSource) {
            streamsEventSource.close();
        }

        streamsList.innerHTML = '<div class="empty-state">Connecting...</div>';
        streamsEventSource = new EventSource(`/api/log_names/stream?device=${encodeURIComponent(device)}`);

        const handleStreamsData = (event) => {
            const streams = JSON.parse(event.data);
            // Сортируем по убыванию (новые сверху)
            streams.sort().reverse();
            
            renderList(streamsList, streams, selectStream, currentStream);
        };

        streamsEventSource.addEventListener('init', handleStreamsData);
        streamsEventSource.addEventListener('update', handleStreamsData);

        streamsEventSource.addEventListener('error', (event) => {
            console.error('Streams SSE Error:', event);
        });
    }

    // Выбор стрима
    function selectStream(stream) {
        if (currentStream === stream) return;
        
        currentStream = stream;
        currentStreamLabel.textContent = `(${stream})`;
        downloadLogsBtn.style.display = 'inline-block';
        
        // Обновляем выделение
        Array.from(streamsList.children).forEach(el => {
            if (el.classList.contains('list-item')) {
                el.classList.toggle('selected', el.textContent === stream);
            }
        });

        connectToLogStream();
    }

    function closeLogStream() {
        if (logsEventSource) {
            logsEventSource.close();
            logsEventSource = null;
        }
    }

    // Подключение к SSE
    function connectToLogStream() {
        closeLogStream();
        logsContainer.innerHTML = '<div class="empty-state">Connecting...</div>';

        if (!currentDevice || !currentStream) return;

        logsEventSource = new EventSource(`/api/logs/stream?device=${encodeURIComponent(currentDevice)}&log_name=${encodeURIComponent(currentStream)}`);

        logsEventSource.addEventListener('init', (event) => {
            const logs = JSON.parse(event.data);
            logsContainer.innerHTML = '';
            
            if (logs.length === 0) {
                logsContainer.innerHTML = '<div class="empty-state">Stream is empty</div>';
                return;
            }

            // Не сортируем логи, выводим в том порядке, в котором они пришли из Redis (от старых к новым)
            logs.forEach(log => {
                logsContainer.appendChild(createLogElement(log, false));
            });
            
            // Прокручиваем вниз, если включена автопрокрутка
            if (autoscrollCb.checked) {
                scrollToBottom();
            }
        });

        logsEventSource.addEventListener('new_logs', (event) => {
            const logs = JSON.parse(event.data);
            
            const emptyState = logsContainer.querySelector('.empty-state');
            if (emptyState) {
                logsContainer.innerHTML = '';
            }

            // Добавляем новые логи В КОНЕЦ (снизу)
            logs.forEach(log => {
                logsContainer.appendChild(createLogElement(log, true));
            });
            
            // Если включена автопрокрутка, прокручиваем вниз
            if (autoscrollCb.checked) {
                scrollToBottom();
            }
        });

        logsEventSource.addEventListener('error', (event) => {
            console.error('SSE Error:', event);
            if (event.data) {
                logsContainer.innerHTML += `<div class="empty-state" style="color: #f48771;">Error: ${event.data}</div>`;
                closeLogStream();
            }
        });
    }

    function scrollToBottom() {
        // Используем requestAnimationFrame, чтобы браузер успел отрендерить новые элементы перед скроллом
        requestAnimationFrame(() => {
            logsContainer.scrollTop = logsContainer.scrollHeight;
        });
    }

    // Создание DOM элемента для лога (текстовый формат)
    function createLogElement(log, isNew) {
        const div = document.createElement('div');
        div.className = `log-line ${isNew ? 'new' : ''}`;
        
        let message = log.message || '';
        
        // Простая раскраска уровней логов на клиенте для визуального удобства
        // Ищем паттерн уровня лога, например "] INFO "
        message = message.replace(/\] (INFO|WARNING|ERROR|DEBUG) /, (match, level) => {
            return `] <span style="color: ${getColorForLevel(level)}; font-weight: bold;">${level}</span> `;
        });

        div.innerHTML = message;
        
        return div;
    }
    
    function getColorForLevel(level) {
        switch(level) {
            case 'INFO': return '#4fc1ff';
            case 'WARNING': return '#cca700';
            case 'ERROR': return '#f48771';
            case 'DEBUG': return '#858585';
            default: return '#d4d4d4';
        }
    }

    // Инициализация
    connectToDevicesStream();

    // Обработчик поиска
    deviceSearchInput.addEventListener('input', (e) => {
        searchQuery = e.target.value;
        filterAndRenderDevices();
    });

    // Обработчики кнопок
    clearLogsBtn.addEventListener('click', () => {
        if (currentDevice && currentStream) {
            logsContainer.innerHTML = '<div class="empty-state">Logs cleared from view. Waiting for new logs...</div>';
        }
    });
    downloadLogsBtn.addEventListener('click', () => {
        if (!currentDevice || !currentStream) return;
        const url = `/api/logs/download?device=${encodeURIComponent(currentDevice)}&log_name=${encodeURIComponent(currentStream)}`;
        window.location.href = url;
    });
});
