from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import os
import time
import threading
import json

app = Flask(__name__)
CORS(app)  # Enable CORS for frontend communication

# Global state management
operators = {}  # {id: {name, status, callTime, endTime}}
call_queue = []
credentials_file = "credentials.txt"
pending_file = "pending_requests.txt"
current_user = None

# Lock for thread-safe operations
state_lock = threading.Lock()

# ==================== FILE OPERATIONS ====================

def read_credentials(filename):
    """Read credentials from file"""
    users = {}
    if os.path.exists(filename):
        with open(filename, 'r') as f:
            for line in f:
                parts = line.strip().split()
                if len(parts) == 2:
                    users[parts[0]] = parts[1]
    return users

def write_credentials(filename, username, password):
    """Write credentials to file"""
    with open(filename, 'a') as f:
        f.write(f"{username} {password}\n")

def check_user_exists(username):
    """Check if user exists in credentials or pending"""
    creds = read_credentials(credentials_file)
    pending = read_credentials(pending_file)
    return username in creds or username in pending

# ==================== AUTHENTICATION ROUTES ====================

@app.route('/api/login', methods=['POST'])
def login():
    """Handle user login"""
    global current_user
    data = request.json
    username = data.get('username')
    password = data.get('password')
    
    if not username or not password:
        return jsonify({'success': False, 'message': 'Username and password required'}), 400
    
    # Check credentials
    users = read_credentials(credentials_file)
    
    if username in users and users[username] == password:
        current_user = username
        return jsonify({
            'success': True, 
            'message': f'Login successful. Welcome, {username}!',
            'username': username
        })
    else:
        return jsonify({
            'success': False, 
            'message': 'Invalid username or password, or user not approved'
        }), 401

@app.route('/api/register', methods=['POST'])
def register():
    """Handle user registration"""
    data = request.json
    username = data.get('username')
    password = data.get('password')
    confirm_password = data.get('confirmPassword')
    
    if not username or not password or not confirm_password:
        return jsonify({'success': False, 'message': 'All fields required'}), 400
    
    if password != confirm_password:
        return jsonify({'success': False, 'message': 'Passwords do not match'}), 400
    
    # Check if user already exists
    if check_user_exists(username):
        return jsonify({
            'success': False, 
            'message': 'Username already exists or is pending approval'
        }), 400
    
    # Add to pending file
    write_credentials(pending_file, username, password)
    
    return jsonify({
        'success': True, 
        'message': 'Registration successful. Your request is pending admin approval.'
    })

@app.route('/api/admin/login', methods=['POST'])
def admin_login():
    """Handle admin login"""
    data = request.json
    username = data.get('username')
    password = data.get('password')
    
    # Admin credentials from your C code
    if username == 'admin' and password == '123':
        return jsonify({'success': True, 'message': 'Admin access granted'})
    else:
        return jsonify({'success': False, 'message': 'Invalid admin credentials'}), 401

@app.route('/api/admin/pending', methods=['GET'])
def get_pending_users():
    """Get pending user registrations"""
    pending = read_credentials(pending_file)
    pending_list = [{'username': user} for user in pending.keys()]
    return jsonify({'success': True, 'pending': pending_list})

@app.route('/api/admin/approve', methods=['POST'])
def approve_user():
    """Approve a pending user"""
    data = request.json
    username = data.get('username')
    
    if not username:
        return jsonify({'success': False, 'message': 'Username required'}), 400
    
    # Read pending users
    pending = read_credentials(pending_file)
    
    if username not in pending:
        return jsonify({'success': False, 'message': 'User not found in pending list'}), 404
    
    # Move to credentials file
    write_credentials(credentials_file, username, pending[username])
    
    # Remove from pending file
    remaining_pending = {u: p for u, p in pending.items() if u != username}
    with open(pending_file, 'w') as f:
        for user, pwd in remaining_pending.items():
            f.write(f"{user} {pwd}\n")
    
    return jsonify({
        'success': True, 
        'message': f'User {username} approved and moved to credentials'
    })

@app.route('/api/admin/reject', methods=['POST'])
def reject_user():
    """Reject a pending user"""
    data = request.json
    username = data.get('username')
    
    if not username:
        return jsonify({'success': False, 'message': 'Username required'}), 400
    
    # Read pending users
    pending = read_credentials(pending_file)
    
    if username not in pending:
        return jsonify({'success': False, 'message': 'User not found in pending list'}), 404
    
    # Remove from pending file
    remaining_pending = {u: p for u, p in pending.items() if u != username}
    with open(pending_file, 'w') as f:
        for user, pwd in remaining_pending.items():
            f.write(f"{user} {pwd}\n")
    
    return jsonify({
        'success': True, 
        'message': f'User {username} rejected'
    })

# ==================== OPERATOR MANAGEMENT ====================

@app.route('/api/operator/add', methods=['POST'])
def add_operator():
    """Add a new operator"""
    data = request.json
    op_id = data.get('id')
    name = data.get('name')
    
    if not op_id or not name:
        return jsonify({'success': False, 'message': 'ID and name required'}), 400
    
    with state_lock:
        if op_id in operators:
            return jsonify({
                'success': False, 
                'message': f'Operator {op_id} already exists'
            }), 400
        
        operators[op_id] = {
            'id': op_id,
            'name': name,
            'status': 'FREE',
            'callTime': 0,
            'endTime': None
        }
    
    return jsonify({
        'success': True, 
        'message': f'Operator {op_id} ({name}) added successfully'
    })

@app.route('/api/operator/remove', methods=['POST'])
def remove_operator():
    """Remove an operator"""
    data = request.json
    op_id = data.get('id')
    
    if not op_id:
        return jsonify({'success': False, 'message': 'ID required'}), 400
    
    with state_lock:
        if op_id not in operators:
            return jsonify({
                'success': False, 
                'message': f'Operator {op_id} not found'
            }), 404
        
        removed = operators.pop(op_id)
    
    return jsonify({
        'success': True, 
        'message': f'Operator {op_id} ({removed["name"]}) removed successfully'
    })

@app.route('/api/operator/status', methods=['GET'])
def get_operator_status():
    """Get status of all operators"""
    with state_lock:
        # Update status based on time
        current_time = time.time()
        for op_id, op in operators.items():
            if op['status'] == 'BUSY' and op['endTime']:
                if current_time >= op['endTime']:
                    # Check if there's a call in queue
                    if call_queue:
                        duration = call_queue.pop(0)
                        op['callTime'] = duration
                        op['endTime'] = current_time + duration
                        op['status'] = 'BUSY'
                    else:
                        op['status'] = 'FREE'
                        op['callTime'] = 0
                        op['endTime'] = None
                else:
                    # Update remaining time
                    op['callTime'] = int(op['endTime'] - current_time)
        
        operator_list = list(operators.values())
    
    return jsonify({'success': True, 'operators': operator_list})

# ==================== CALL MANAGEMENT ====================

@app.route('/api/call/assign', methods=['POST'])
def assign_call():
    """Assign a call to an operator"""
    data = request.json
    duration = data.get('duration')
    
    if not duration or duration <= 0:
        return jsonify({'success': False, 'message': 'Valid duration required'}), 400
    
    with state_lock:
        # Find a free operator
        free_operator = None
        for op_id, op in operators.items():
            if op['status'] == 'FREE':
                free_operator = op
                break
        
        if free_operator:
            # Assign call to free operator
            current_time = time.time()
            free_operator['status'] = 'BUSY'
            free_operator['callTime'] = duration
            free_operator['endTime'] = current_time + duration
            
            return jsonify({
                'success': True,
                'message': f'Call assigned to Operator {free_operator["id"]} ({free_operator["name"]}) for {duration}s',
                'operator': free_operator
            })
        else:
            # Add to queue
            call_queue.append(duration)
            return jsonify({
                'success': True,
                'message': f'All operators busy. Call duration {duration}s added to queue.',
                'queued': True,
                'queueLength': len(call_queue)
            })

@app.route('/api/queue/status', methods=['GET'])
def get_queue_status():
    """Get status of call queue"""
    with state_lock:
        queue_copy = list(call_queue)
    
    return jsonify({
        'success': True,
        'queue': queue_copy,
        'length': len(queue_copy)
    })

# ==================== STATIC FILES ====================

@app.route('/')
def serve_frontend():
    """Serve the frontend HTML"""
    return send_from_directory('.', 'index.html')

@app.route('/<path:path>')
def serve_static(path):
    """Serve static files"""
    return send_from_directory('.', path)

# ==================== RUN SERVER ====================

if __name__ == '__main__':
    # Create credential files if they don't exist
    if not os.path.exists(credentials_file):
        # Add default admin (optional)
        write_credentials(credentials_file, 'admin', '123')
    
    if not os.path.exists(pending_file):
        open(pending_file, 'w').close()
    
    print("=" * 50)
    print("🚀 Call Center Backend Server Starting...")
    print("=" * 50)
    print("📡 Server running at: http://localhost:5000")
    print("🔐 Default admin: username='admin', password='123'")
    print("=" * 50)
    
    # Run Flask server
    app.run(debug=True, host='0.0.0.0', port=5000)