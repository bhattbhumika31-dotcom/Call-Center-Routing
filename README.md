# 📞 Call Center Management System

A modern, full-stack call center routing and management platform built with C backend.

## 🌟 Features

- **User Authentication**: Secure login and registration system with admin approval workflow
- **Operator Management**: Add, remove, and monitor call center operators in real-time
- **Call Routing**: Intelligent call assignment to available operators
- **Queue Management**: Track waiting calls and manage call queues
- **Real-time Status**: Live operator status updates with call duration tracking
- **Admin Portal**: Approve or reject pending user registrations
- **Responsive Design**: Beautiful, modern UI that works on all devices

## 🏗️ Project Structure

```
.
├── main.c              # Core call center logic
├── routing.c/h         # Call routing algorithms
├── utils.c/h           # Utility functions
├── frontend/
│   ├── app.py          # Flask backend server
│   ├── index.html      # Main UI
│   ├── style.css       # Modern styling
│   └── script.js       # Frontend logic
└── README.md           # This file
```

## 🚀 Getting Started

### Prerequisites

- **C Compiler**: GCC or Clang
- **Python 3.7+**: For the Flask backend
- **pip**: Python package manager
- **Modern Web Browser**: Chrome, Firefox, Safari, or Edge

### Installation

1. **Clone or download the project**
   ```bash
   cd call-center-system
   ```

2. **Install Python dependencies**
   ```bash
   pip install flask flask-cors
   ```

3. **Compile C backend** (optional - for production use)
   ```bash
   gcc -o call_center main.c routing.c utils.c
   ```

### Running the Application

1. **Start the Flask backend server**
   ```bash
   cd frontend
   python app.py
   ```
   The server will start at `http://localhost:5000`

2. **Open in browser**
   - Navigate to `http://localhost:5000` in your web browser
   - The frontend will load automatically

## 📋 Usage Guide

### User Registration & Login

1. **Register a new account**
   - Click the "Register" tab
   - Enter username and password
   - Submit the form
   - Wait for admin approval

2. **Admin Approval**
   - Click "Admin Approval Portal"
   - Login with: `username: admin` | `password: 123`
   - Approve or reject pending registrations

3. **Login**
   - Use your approved credentials to login
   - Access the main dashboard

### Dashboard Operations

#### ➕ Add Operator
- Enter operator ID and name
- Click "Add" to register the operator
- Operator will appear in the status list

#### ➖ Remove Operator
- Enter the operator ID to remove
- Click "Remove" to delete the operator
- Operator will be removed from the system

#### 📊 Operator Status
- View all operators and their current status
- See real-time call duration for busy operators
- Status indicators: FREE (green) or BUSY (red)

#### 📞 Assign Call
- Enter call duration in seconds
- Click "Assign" to route the call
- System automatically assigns to available operator
- If all operators busy, call is queued

#### ⏳ Queue Status
- View all waiting calls in the queue
- See call durations for each queued call
- Calls are automatically assigned when operators become free

## 🎨 Design Features

- **Modern Gradient UI**: Beautiful purple-to-pink gradient theme
- **Smooth Animations**: Elegant transitions and slide-in effects
- **Responsive Layout**: Adapts perfectly to mobile, tablet, and desktop
- **Real-time Updates**: Auto-refresh operator and queue status every 2 seconds
- **Intuitive Controls**: Clear action buttons and organized sections
- **Professional Cards**: Operator status displayed in modern card layout

## 🔐 Security

- User credentials stored securely in files
- Admin approval required for new users
- Session-based authentication
- CORS enabled for frontend-backend communication

## 📊 API Endpoints

### Authentication
- `POST /api/login` - User login
- `POST /api/register` - User registration
- `POST /api/admin/login` - Admin login
- `GET /api/admin/pending` - Get pending registrations
- `POST /api/admin/approve` - Approve user
- `POST /api/admin/reject` - Reject user

### Operators
- `POST /api/operator/add` - Add new operator
- `POST /api/operator/remove` - Remove operator
- `GET /api/operator/status` - Get all operators status

### Calls
- `POST /api/call/assign` - Assign call to operator
- `GET /api/queue/status` - Get queue status

## 🛠️ Technology Stack

- **Backend**: C (core logic), Python Flask (API server)
- **Frontend**: HTML5, CSS3, Vanilla JavaScript
- **Database**: File-based storage (credentials, pending requests)
- **Communication**: REST API with JSON

## 📝 Default Credentials

**Admin Account:**
- Username: `admin`
- Password: `123`

## 🐛 Troubleshooting

### Server won't start
- Ensure port 5000 is not in use
- Check Python and Flask are installed: `pip list | grep flask`
- Try: `python app.py` from the frontend directory

### Frontend not loading
- Verify Flask server is running
- Check browser console for errors (F12)
- Clear browser cache and reload

### Operators not appearing
- Ensure you've added operators first
- Check browser console for API errors
- Verify Flask server is running

## 📱 Browser Compatibility

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## 🚀 Performance

- Real-time updates every 2 seconds
- Efficient call routing algorithm
- Responsive UI with smooth animations
- Optimized for up to 100+ concurrent operators

## 📄 License

This project is provided as-is for educational and commercial use.

## 👥 Support

For issues or questions:
1. Check the troubleshooting section
2. Review the API endpoints documentation
3. Check browser console for error messages

---

**Built with ❤️ for efficient call center management**
