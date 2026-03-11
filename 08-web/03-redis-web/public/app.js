document.addEventListener('DOMContentLoaded', () => {
    const deviceSelect = document.getElementById('device-select');
    const streamSelect = document.getElementById('stream-select');
    const refreshBtn = document.getElementById('refresh-btn');
    const logsBody = document.getElementById('logs-body');

    // Форматирование времени из миллисекунд
    function formatTime(timestampMs) {
        if (!timestampMs) return 'Unknown';
        const date = new Date(parseInt(timestampMs));
        return date.toLocaleString();
    }

    // Получение списка устройств
    async function fetchDevices() {
        try {
            const response = await fetch('/api/devices');
            if (!response.ok) throw new Error('Failed to fetch devices');
            
            const devices = await response.json();
            
            deviceSelect.innerHTML = '<option value="">-- Select a device --</option>';
            
            if (devices.length === 0) {
                deviceSelect.innerHTML = '<option value="">No devices found</option>';
                return;
            }

            devices.forEach(device => {
                const option = document.createElement('option');
                option.value = device;
                option.textContent = device;
                deviceSelect.appendChild(option);
            });
        } catch (error) {
            console.error('Error fetching devices:', error);
            deviceSelect.innerHTML = '<option value="">Error loading devices</option>';
        }
    }

    // Получение списка стримов для выбранного устройства
    async function fetchStreams() {
        const device = deviceSelect.value;
        
        // Сбрасываем стримы и логи
        streamSelect.innerHTML = '<option value="">-- Select stream --</option>';
        streamSelect.disabled = true;
        
        if (eventSource) {
            eventSource.close();
            eventSource = null;
        }
        logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">Select a stream to view logs</td></tr>';

        if (!device) return;

        try {
            streamSelect.innerHTML = '<option value="">Loading streams...</option>';
            const response = await fetch(`/api/streams?device=${encodeURIComponent(device)}`);
            if (!response.ok) throw new Error('Failed to fetch streams');
            
            const streams = await response.json();
            
            streamSelect.innerHTML = '<option value="">-- Select a stream --</option>';
            
            if (streams.length === 0) {
                streamSelect.innerHTML = '<option value="">No streams found</option>';
                return;
            }

            // Сортируем стримы по имени (по убыванию, чтобы новые были сверху)
            streams.sort().reverse();

            streams.forEach(stream => {
                const option = document.createElement('option');
                option.value = stream;
                option.textContent = stream;
                streamSelect.appendChild(option);
            });
            
            streamSelect.disabled = false;
        } catch (error) {
            console.error('Error fetching streams:', error);
            streamSelect.innerHTML = '<option value="">Error loading streams</option>';
        }
    }

    // Получение логов для выбранного устройства и стрима (SSE)
    let eventSource = null;

    function fetchLogs() {
        const device = deviceSelect.value;
        const stream = streamSelect.value;
        
        // Закрываем предыдущее соединение, если оно было
        if (eventSource) {
            eventSource.close();
            eventSource = null;
        }

        if (!device || !stream) {
            logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">Select a device and stream to view logs</td></tr>';
            return;
        }

        logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">Connecting to stream...</td></tr>';
        
        // Открываем SSE соединение
        eventSource = new EventSource(`/api/logs/stream?device=${encodeURIComponent(device)}&stream=${encodeURIComponent(stream)}`);

        // Обработка начальных данных (все старые логи)
        eventSource.addEventListener('init', (event) => {
            const logs = JSON.parse(event.data);
            
            if (logs.length === 0) {
                logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">No logs found for this stream</td></tr>';
                return;
            }

            logsBody.innerHTML = '';
            
            // Сортируем логи по времени (новые сверху)
            logs.sort((a, b) => parseInt(b.timestamp || 0) - parseInt(a.timestamp || 0));

            logs.forEach(log => {
                const tr = createLogRow(log);
                logsBody.appendChild(tr);
            });
        });

        // Обработка новых логов в реальном времени
        eventSource.addEventListener('new_logs', (event) => {
            const logs = JSON.parse(event.data);
            
            // Убираем сообщение "No logs", если оно есть
            const emptyState = logsBody.querySelector('.empty-state');
            if (emptyState) {
                logsBody.innerHTML = '';
            }

            // Сортируем новые логи
            logs.sort((a, b) => parseInt(b.timestamp || 0) - parseInt(a.timestamp || 0));

            // Добавляем новые логи В НАЧАЛО таблицы
            for (let i = logs.length - 1; i >= 0; i--) {
                const tr = createLogRow(logs[i]);
                // Эффект подсветки для новых строк
                tr.style.backgroundColor = '#e8f5e9';
                setTimeout(() => { tr.style.backgroundColor = ''; }, 2000);
                
                logsBody.insertBefore(tr, logsBody.firstChild);
            }
        });

        eventSource.addEventListener('error', (event) => {
            console.error('SSE Error:', event);
            if (event.data) {
                logsBody.innerHTML = `<tr><td colspan="3" class="empty-state" style="color: red;">Error: ${event.data}</td></tr>`;
                eventSource.close();
            }
        });
    }

    function createLogRow(log) {
        const tr = document.createElement('tr');
        tr.style.transition = 'background-color 1s ease';
        
        const timeStr = formatTime(log.timestamp);
        const level = log.level || 'UNKNOWN';
        const message = log.message || '';

        tr.innerHTML = `
            <td>${timeStr}</td>
            <td><span class="level-badge level-${level}">${level}</span></td>
            <td>${message}</td>
        `;
        return tr;
    }

    // Инициализация
    fetchDevices();

    // Обработчики событий
    deviceSelect.addEventListener('change', fetchStreams);
    streamSelect.addEventListener('change', fetchLogs);
    refreshBtn.addEventListener('click', fetchLogs);
});
