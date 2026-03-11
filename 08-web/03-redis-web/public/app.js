document.addEventListener('DOMContentLoaded', () => {
    const deviceSelect = document.getElementById('device-select');
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

    // Получение логов для выбранного устройства
    async function fetchLogs() {
        const device = deviceSelect.value;
        if (!device) {
            logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">Select a device to view logs</td></tr>';
            return;
        }

        try {
            logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">Loading logs...</td></tr>';
            
            const response = await fetch(`/api/logs?device=${encodeURIComponent(device)}`);
            if (!response.ok) throw new Error('Failed to fetch logs');
            
            const logs = await response.json();
            
            if (logs.length === 0) {
                logsBody.innerHTML = '<tr><td colspan="3" class="empty-state">No logs found for this device</td></tr>';
                return;
            }

            logsBody.innerHTML = '';
            
            // Сортируем логи по времени (новые сверху)
            logs.sort((a, b) => parseInt(b.timestamp || 0) - parseInt(a.timestamp || 0));

            logs.forEach(log => {
                const tr = document.createElement('tr');
                
                const timeStr = formatTime(log.timestamp);
                const level = log.level || 'UNKNOWN';
                const message = log.message || '';

                tr.innerHTML = `
                    <td>${timeStr}</td>
                    <td><span class="level-badge level-${level}">${level}</span></td>
                    <td>${message}</td>
                `;
                
                logsBody.appendChild(tr);
            });
        } catch (error) {
            console.error('Error fetching logs:', error);
            logsBody.innerHTML = '<tr><td colspan="3" class="empty-state" style="color: red;">Error loading logs</td></tr>';
        }
    }

    // Инициализация
    fetchDevices();

    // Обработчики событий
    deviceSelect.addEventListener('change', fetchLogs);
    refreshBtn.addEventListener('click', fetchLogs);
});
