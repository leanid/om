document.addEventListener('DOMContentLoaded', () => {
    const devicesList = document.getElementById('devices-list');
    const streamsList = document.getElementById('streams-list');
    const logsContainer = document.getElementById('logs-container');
    const platformFiltersContainer = document.getElementById('platform-filters');

    const currentDeviceLabel = document.getElementById('current-device-label');
    const currentStreamLabel = document.getElementById('current-stream-label');

    const downloadLogsBtn = document.getElementById('download-logs-btn');
    const autoscrollCb = document.getElementById('autoscroll-cb');
    const deviceSearchInput = document.getElementById('device-search');

    let allDevicesData = [];
    let activePlatforms = new Set();
    let searchQuery = '';
    let isFirstLoad = true;

    let currentDevice = null;
    let currentStream = null;

    let eventSource = null;

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

    function filterAndRenderDevices() {
        const filteredDevices = allDevicesData
            .filter(d => activePlatforms.has(d.platform))
            .filter(d => d.name.toLowerCase().includes(searchQuery.toLowerCase()))
            .map(d => d.name)
            .sort();

        renderList(devicesList, filteredDevices, selectDevice, currentDevice);
    }

    // --- Единое SSE-соединение ---

    function connectToStream() {
        if (eventSource) {
            eventSource.close();
        }

        let url = '/api/stream';
        const params = new URLSearchParams();
        if (currentDevice) params.set('device', currentDevice);
        if (currentDevice && currentStream) params.set('log_name', currentStream);
        const query = params.toString();
        if (query) url += '?' + query;

        eventSource = new EventSource(url);

        // Devices
        const handleDevicesData = (event) => {
            allDevicesData = JSON.parse(event.data);

            const platforms = new Set();
            allDevicesData.forEach(d => {
                if (!d.platform) d.platform = 'Unknown';
                platforms.add(d.platform);
            });

            if (isFirstLoad) {
                platforms.forEach(p => activePlatforms.add(p));
                isFirstLoad = false;
            } else {
                const existingCheckboxes = Array.from(
                    platformFiltersContainer.querySelectorAll('input[type="checkbox"]')
                ).map(cb => cb.value);
                platforms.forEach(p => {
                    if (!existingCheckboxes.includes(p)) {
                        activePlatforms.add(p);
                    }
                });
            }

            renderPlatformFilters(platforms);
            filterAndRenderDevices();
        };

        eventSource.addEventListener('devices_init', handleDevicesData);
        eventSource.addEventListener('devices_update', handleDevicesData);

        // Log names
        const handleLogNamesData = (event) => {
            const names = JSON.parse(event.data);
            names.sort().reverse();
            renderList(streamsList, names, selectStream, currentStream);
        };

        eventSource.addEventListener('log_names_init', handleLogNamesData);
        eventSource.addEventListener('log_names_update', handleLogNamesData);

        // Logs
        eventSource.addEventListener('logs_init', (event) => {
            const logs = JSON.parse(event.data);
            logsContainer.innerHTML = '';

            if (logs.length === 0) {
                logsContainer.innerHTML = '<div class="empty-state">Stream is empty</div>';
                return;
            }

            logs.forEach(log => {
                logsContainer.appendChild(createLogElement(log, false));
            });

            if (autoscrollCb.checked) {
                scrollToBottom();
            }
        });

        eventSource.addEventListener('logs_new', (event) => {
            const logs = JSON.parse(event.data);

            const emptyState = logsContainer.querySelector('.empty-state');
            if (emptyState) {
                logsContainer.innerHTML = '';
            }

            logs.forEach(log => {
                logsContainer.appendChild(createLogElement(log, true));
            });

            if (autoscrollCb.checked) {
                scrollToBottom();
            }
        });

        eventSource.addEventListener('error', (event) => {
            console.error('SSE Error:', event);
            if (event.data) {
                const errDiv = document.createElement('div');
                errDiv.className = 'empty-state';
                errDiv.style.color = '#f48771';
                errDiv.textContent = 'Error: ' + event.data;
                logsContainer.appendChild(errDiv);
            }
        });
    }

    // --- Selection ---

    function selectDevice(device) {
        if (currentDevice === device) return;

        currentDevice = device;
        currentDeviceLabel.textContent = `(${device})`;

        Array.from(devicesList.children).forEach(el => {
            if (el.classList.contains('list-item')) {
                el.classList.toggle('selected', el.textContent === device);
            }
        });

        currentStream = null;
        currentStreamLabel.textContent = '';
        downloadLogsBtn.style.display = 'none';
        streamsList.innerHTML = '<div class="empty-state">Loading...</div>';
        logsContainer.innerHTML = '<div class="empty-state">Select a log to view</div>';

        connectToStream();
    }

    function selectStream(stream) {
        if (currentStream === stream) return;

        currentStream = stream;
        currentStreamLabel.textContent = `(${stream})`;
        downloadLogsBtn.style.display = 'inline-block';

        Array.from(streamsList.children).forEach(el => {
            if (el.classList.contains('list-item')) {
                el.classList.toggle('selected', el.textContent === stream);
            }
        });

        logsContainer.innerHTML = '<div class="empty-state">Loading...</div>';
        connectToStream();
    }

    // --- Helpers ---

    function scrollToBottom() {
        requestAnimationFrame(() => {
            logsContainer.scrollTop = logsContainer.scrollHeight;
        });
    }

    function escapeHtml(text) {
        const el = document.createElement('span');
        el.textContent = text;
        return el.innerHTML;
    }

    function createLogElement(log, isNew) {
        const div = document.createElement('div');
        div.className = `log-line ${isNew ? 'new' : ''}`;

        let message = escapeHtml(log.message || '');

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

    // --- Init ---

    devicesList.innerHTML = '<div class="empty-state">Connecting...</div>';
    connectToStream();

    deviceSearchInput.addEventListener('input', (e) => {
        searchQuery = e.target.value;
        filterAndRenderDevices();
    });

    downloadLogsBtn.addEventListener('click', () => {
        if (!currentDevice || !currentStream) return;
        const url = `/api/logs/download?device=${encodeURIComponent(currentDevice)}&log_name=${encodeURIComponent(currentStream)}`;
        window.location.href = url;
    });
});
