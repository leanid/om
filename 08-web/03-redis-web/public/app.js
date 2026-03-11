document.addEventListener('DOMContentLoaded', () => {
    const devicesList = document.getElementById('devices-list');
    const streamsList = document.getElementById('streams-list');
    const logsContainer = document.getElementById('logs-container');
    const platformFiltersContainer = document.getElementById('platform-filters');
    
    const currentDeviceLabel = document.getElementById('current-device-label');
    const currentStreamLabel = document.getElementById('current-stream-label');
    
    const refreshDevicesBtn = document.getElementById('refresh-devices-btn');
    const clearLogsBtn = document.getElementById('clear-logs-btn');
    const downloadLogsBtn = document.getElementById('download-logs-btn');

    let allDevicesData = []; // Храним полный список {name, platform}
    let activePlatforms = new Set(); // Какие платформы сейчас выбраны
    
    let currentDevice = null;
    let currentStream = null;
    let eventSource = null;

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
        // Фильтруем устройства по выбранным платформам
        const filteredDevices = allDevicesData
            .filter(d => activePlatforms.has(d.platform))
            .map(d => d.name)
            .sort(); // Алфавитная сортировка имен
            
        renderList(devicesList, filteredDevices, selectDevice, currentDevice);
    }

    // Получение списка устройств
    async function fetchDevices() {
        try {
            devicesList.innerHTML = '<div class="empty-state">Loading...</div>';
            const response = await fetch('/api/devices');
            if (!response.ok) throw new Error('Failed to fetch devices');
            
            allDevicesData = await response.json();
            
            // Собираем все уникальные платформы
            const platforms = new Set();
            allDevicesData.forEach(d => {
                if (!d.platform) d.platform = 'Unknown';
                platforms.add(d.platform);
            });

            // Если загружаем первый раз или появились новые платформы, включаем их по умолчанию
            platforms.forEach(p => activePlatforms.add(p));

            renderPlatformFilters(platforms);
            filterAndRenderDevices();
            
        } catch (error) {
            console.error('Error fetching devices:', error);
            devicesList.innerHTML = '<div class="empty-state" style="color: red;">Error loading devices</div>';
        }
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
        closeStream();
        logsContainer.innerHTML = '<div class="empty-state">Select a stream to view logs</div>';

        fetchStreams(device);
    }

    // Получение списка стримов
    async function fetchStreams(device) {
        try {
            streamsList.innerHTML = '<div class="empty-state">Loading...</div>';
            const response = await fetch(`/api/streams?device=${encodeURIComponent(device)}`);
            if (!response.ok) throw new Error('Failed to fetch streams');
            
            const streams = await response.json();
            // Сортируем по убыванию (новые сверху)
            streams.sort().reverse();
            
            renderList(streamsList, streams, selectStream, currentStream);
        } catch (error) {
            console.error('Error fetching streams:', error);
            streamsList.innerHTML = '<div class="empty-state" style="color: red;">Error loading streams</div>';
        }
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

    function closeStream() {
        if (eventSource) {
            eventSource.close();
            eventSource = null;
        }
    }

    // Подключение к SSE
    function connectToLogStream() {
        closeStream();
        logsContainer.innerHTML = '<div class="empty-state">Connecting...</div>';

        if (!currentDevice || !currentStream) return;

        eventSource = new EventSource(`/api/logs/stream?device=${encodeURIComponent(currentDevice)}&stream=${encodeURIComponent(currentStream)}`);

        eventSource.addEventListener('init', (event) => {
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
            
            // Прокручиваем вниз
            scrollToBottom();
        });

        eventSource.addEventListener('new_logs', (event) => {
            const logs = JSON.parse(event.data);
            
            const emptyState = logsContainer.querySelector('.empty-state');
            if (emptyState) {
                logsContainer.innerHTML = '';
            }

            // Проверяем, был ли скролл в самом низу до добавления новых логов
            const isScrolledToBottom = logsContainer.scrollHeight - logsContainer.clientHeight <= logsContainer.scrollTop + 10;

            // Добавляем новые логи В КОНЕЦ (снизу)
            logs.forEach(log => {
                logsContainer.appendChild(createLogElement(log, true));
            });
            
            // Если мы были внизу, прокручиваем дальше вниз
            if (isScrolledToBottom) {
                scrollToBottom();
            }
        });

        eventSource.addEventListener('error', (event) => {
            console.error('SSE Error:', event);
            if (event.data) {
                logsContainer.innerHTML += `<div class="empty-state" style="color: #f48771;">Error: ${event.data}</div>`;
                closeStream();
            }
        });
    }

    function scrollToBottom() {
        logsContainer.scrollTop = logsContainer.scrollHeight;
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
    fetchDevices();

    // Обработчики кнопок
    refreshDevicesBtn.addEventListener('click', fetchDevices);
    clearLogsBtn.addEventListener('click', () => {
        if (currentDevice && currentStream) {
            logsContainer.innerHTML = '<div class="empty-state">Logs cleared from view. Waiting for new logs...</div>';
        }
    });
    downloadLogsBtn.addEventListener('click', () => {
        if (!currentDevice || !currentStream) return;
        const url = `/api/logs/download?device=${encodeURIComponent(currentDevice)}&stream=${encodeURIComponent(currentStream)}`;
        window.location.href = url;
    });
});
