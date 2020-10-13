typedef enum {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP} State;

#define MSG_FLAG 0x7e

#define MSG_A_EMT 0x03
#define MSG_A_REC 0x01

#define MSG_CTRL_SET 0x03
#define MSG_CTRL_UA 0x07

#define MSG_SET_SIZE 5