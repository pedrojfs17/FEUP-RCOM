// FTP Commands
#define USER "USER"
#define PASS "PASS"
#define PASV "PASV"
#define RETR "RETR"
#define QUIT "QUIT"

// Command Terminator
#define CRLF "\r\n"

// Socket Response
#define CMD_SOCKET_READY 220

// Login Responses
#define CMD_LOGIN_SUCCESS       230
#define CMD_USERNAME_OK         331
#define CMD_NOT_LOGGED_IN       530

// Pasv Response
#define CMD_PASV_MODE           227