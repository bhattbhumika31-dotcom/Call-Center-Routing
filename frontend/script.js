const API_URL = 'http://localhost:5000/api';
        let currentUsername = '';
        let statusUpdateInterval = null;

        // ==================== TAB SWITCHING ====================
        function switchTab(tab) {
            const loginForm = document.getElementById('loginForm');
            const registerForm = document.getElementById('registerForm');
            const tabs = document.querySelectorAll('.tab-btn');

            tabs.forEach(t => t.classList.remove('active'));

            if (tab === 'login') {
                loginForm.classList.remove('hidden');
                registerForm.classList.add('hidden');
                tabs[0].classList.add('active');
            } else {
                loginForm.classList.add('hidden');
                registerForm.classList.remove('hidden');
                tabs[1].classList.add('active');
            }
        }

        // ==================== AUTHENTICATION ====================
        async function handleLogin(event) {
            event.preventDefault();
            const btn = document.getElementById('loginBtn');
            btn.disabled = true;
            btn.textContent = 'Logging in...';

            const username = document.getElementById('loginUsername').value;
            const password = document.getElementById('loginPassword').value;

            try {
                const response = await fetch(`${API_URL}/login`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({username, password})
                });

                const data = await response.json();

                if (data.success) {
                    currentUsername = username;
                    document.getElementById('currentUser').textContent = username;
                    document.getElementById('loginPage').style.display = 'none';
                    document.getElementById('dashboard').classList.add('active');
                    
                    // Start auto-refresh for operator status
                    startStatusUpdates();
                } else {
                    alert(data.message);
                }
            } catch (error) {
                alert('Error connecting to server. Make sure the Flask backend is running.');
                console.error(error);
            } finally {
                btn.disabled = false;
                btn.textContent = 'Login';
            }
        }

        async function handleRegister(event) {
            event.preventDefault();
            const btn = document.getElementById('registerBtn');
            btn.disabled = true;
            btn.textContent = 'Registering...';

            const username = document.getElementById('regUsername').value;
            const password = document.getElementById('regPassword').value;
            const confirmPassword = document.getElementById('regConfirmPassword').value;

            try {
                const response = await fetch(`${API_URL}/register`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({username, password, confirmPassword})
                });

                const data = await response.json();
                alert(data.message);

                if (data.success) {
                    switchTab('login');
                    document.getElementById('registerForm').reset();
                }
            } catch (error) {
                alert('Error connecting to server. Make sure the Flask backend is running.');
                console.error(error);
            } finally {
                btn.disabled = false;
                btn.textContent = 'Register';
            }
        }

        function logout() {
            stopStatusUpdates();
            document.getElementById('dashboard').classList.remove('active');
            document.getElementById('loginPage').style.display = 'block';
            document.getElementById('loginForm').reset();
            currentUsername = '';
        }

        // ==================== ADMIN MODAL ====================
        function showAdminModal() {
            document.getElementById('adminModal').classList.add('active');
            document.getElementById('adminLoginSection').classList.remove('hidden');
            document.getElementById('adminPanelSection').classList.add('hidden');
        }

        function closeAdminModal() {
            document.getElementById('adminModal').classList.remove('active');
            document.getElementById('adminUsername').value = '';
            document.getElementById('adminPassword').value = '';
        }

        async function handleAdminLogin(event) {
            event.preventDefault();
            
            const username = document.getElementById('adminUsername').value;
            const password = document.getElementById('adminPassword').value;

            try {
                const response = await fetch(`${API_URL}/admin/login`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({username, password})
                });

                const data = await response.json();

                if (data.success) {
                    document.getElementById('adminLoginSection').classList.add('hidden');
                    document.getElementById('adminPanelSection').classList.remove('hidden');
                    loadPendingUsers();
                } else {
                    alert(data.message);
                }
            } catch (error) {
                alert('Error connecting to server.');
                console.error(error);
            }
        }

        async function loadPendingUsers() {
            try {
                const response = await fetch(`${API_URL}/admin/pending`);
                const data = await response.json();

                const listDiv = document.getElementById('pendingUsersList');
                
                if (data.pending.length === 0) {
                    listDiv.innerHTML = '<p style="text-align: center; color: #666;">No pending registrations</p>';
                    return;
                }

                listDiv.innerHTML = data.pending.map(user => `
                    <div class="pending-user">
                        <span class="pending-user-name">👤 ${user.username}</span>
                        <div class="pending-actions">
                            <button class="approve-btn" onclick="approveUser('${user.username}')">✓ Approve</button>
                            <button class="reject-btn" onclick="rejectUser('${user.username}')">✗ Reject</button>
                        </div>
                    </div>
                `).join('');
            } catch (error) {
                console.error('Error loading pending users:', error);
            }
        }

        async function approveUser(username) {
            try {
                const response = await fetch(`${API_URL}/admin/approve`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({username})
                });

                const data = await response.json();
                alert(data.message);
                loadPendingUsers();
            } catch (error) {
                alert('Error approving user');
                console.error(error);
            }
        }

        async function rejectUser(username) {
            try {
                const response = await fetch(`${API_URL}/admin/reject`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({username})
                });

                const data = await response.json();
                alert(data.message);
                loadPendingUsers();
            } catch (error) {
                alert('Error rejecting user');
                console.error(error);
            }
        }

        // ==================== SECTION MANAGEMENT ====================
        function showSection(sectionId) {
            const sections = document.querySelectorAll('.content-section');
            sections.forEach(s => s.classList.remove('active'));
            document.getElementById(sectionId).classList.add('active');
        }

        // ==================== OPERATOR MANAGEMENT ====================
        async function addOperator() {
            const id = document.getElementById('addOpId').value;
            const name = document.getElementById('addOpName').value;
            const messageDiv = document.getElementById('addOpMessage');

            if (!id || !name) {
                showMessage(messageDiv, 'Please fill all fields!', 'error');
                return;
            }

            try {
                const response = await fetch(`${API_URL}/operator/add`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({id, name})
                });

                const data = await response.json();
                showMessage(messageDiv, data.message, data.success ? 'success' : 'error');

                if (data.success) {
                    document.getElementById('addOpId').value = '';
                    document.getElementById('addOpName').value = '';
                }
            } catch (error) {
                showMessage(messageDiv, 'Error connecting to server', 'error');
                console.error(error);
            }
        }

        async function removeOperator() {
            const id = document.getElementById('removeOpId').value;
            const messageDiv = document.getElementById('removeOpMessage');

            if (!id) {
                showMessage(messageDiv, 'Please enter operator ID!', 'error');
                return;
            }

            try {
                const response = await fetch(`${API_URL}/operator/remove`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({id})
                });

                const data = await response.json();
                showMessage(messageDiv, data.message, data.success ? 'success' : 'error');

                if (data.success) {
                    document.getElementById('removeOpId').value = '';
                }
            } catch (error) {
                showMessage(messageDiv, 'Error connecting to server', 'error');
                console.error(error);
            }
        }

        async function showOperatorStatus() {
            showSection('operatorStatus');
            await updateOperatorStatus();
        }

        async function updateOperatorStatus() {
            try {
                const response = await fetch(`${API_URL}/operator/status`);
                const data = await response.json();

                const listDiv = document.getElementById('operatorList');

                if (data.operators.length === 0) {
                    listDiv.innerHTML = '<p style="text-align: center; color: #666;">No operators added yet.</p>';
                    return;
                }

                listDiv.innerHTML = data.operators.map(op => `
                    <div class="operator-card">
                        <div class="operator-header">
                            <span class="operator-id">#${op.id}</span>
                            <span class="status-badge status-${op.status.toLowerCase()}">${op.status}</span>
                        </div>
                        <div class="operator-name">👤 ${op.name}</div>
                        ${op.status === 'BUSY' ? `<div class="call-time">⏱️ ${op.callTime}s remaining</div>` : ''}
                    </div>
                `).join('');
            } catch (error) {
                console.error('Error fetching operator status:', error);
            }
        }

        // ==================== CALL MANAGEMENT ====================
        async function assignCall() {
            const duration = document.getElementById('callDuration').value;
            const messageDiv = document.getElementById('assignCallMessage');

            if (!duration || duration <= 0) {
                showMessage(messageDiv, 'Please enter valid call duration!', 'error');
                return;
            }

            try {
                const response = await fetch(`${API_URL}/call/assign`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({duration: parseInt(duration)})
                });

                const data = await response.json();
                showMessage(messageDiv, data.message, 'success');

                if (data.success) {
                    document.getElementById('callDuration').value = '';
                }
            } catch (error) {
                showMessage(messageDiv, 'Error connecting to server', 'error');
                console.error(error);
            }
        }

        async function showQueueStatus() {
            showSection('queueStatus');
            await updateQueueStatus();
        }

        async function updateQueueStatus() {
            try {
                const response = await fetch(`${API_URL}/queue/status`);
                const data = await response.json();

                const queueDiv = document.getElementById('queueItems');

                if (data.queue.length === 0) {
                    queueDiv.innerHTML = '<p style="text-align: center; color: #666; width: 100%;">Queue is empty.</p>';
                    return;
                }

                queueDiv.innerHTML = data.queue.map((duration, index) => 
                    `<div class="queue-item">Call #${index + 1}: ${duration}s</div>`
                ).join('');
            } catch (error) {
                console.error('Error fetching queue status:', error);
            }
        }

        // ==================== AUTO-UPDATE ====================
        function startStatusUpdates() {
            // Update status every 2 seconds
            statusUpdateInterval = setInterval(() => {
                const activeSection = document.querySelector('.content-section.active');
                if (activeSection) {
                    if (activeSection.id === 'operatorStatus') {
                        updateOperatorStatus();
                    } else if (activeSection.id === 'queueStatus') {
                        updateQueueStatus();
                    }
                }
            }, 2000);
        }

        function stopStatusUpdates() {
            if (statusUpdateInterval) {
                clearInterval(statusUpdateInterval);
                statusUpdateInterval = null;
            }
        }

        // ==================== UTILITIES ====================
        function showMessage(element, text, type) {
            element.innerHTML = `<div class="message message-${type}">${text}</div>`;
            setTimeout(() => element.innerHTML = '', 5000);
        }

        // Close modal when clicking outside
        window.onclick = function(event) {
            const modal = document.getElementById('adminModal');
            if (event.target === modal) {
                closeAdminModal();
            }
        }