#pragma once

#define MSG_FLAG 0x7e
#define ESCAPE 0x7d

#define MSG_A_TRANS_COMMAND 0x03
#define MSG_A_RECV_RESPONSE 0x03
#define MSG_A_TRANS_RESPONSE 0x01
#define MSG_A_RECV_COMMAND 0x01

#define MSG_CTRL_SET 0x03
#define MSG_CTRL_UA 0x07
#define MSG_CTRL_RR0 0x05
#define MSG_CTRL_RR1 0x85
#define MSG_CTRL_REJ0 0x01
#define MSG_CTRL_REJ1 0x81
#define MSG_CTRL_DISC 0x0b
#define MSG_CTRL_S0 0x00
#define MSG_CTRL_S1 0x40


#define MSG_SET_SIZE 5

#define COM0 0
#define COM1 1
#define COM10 10
#define COM11 11

#define TRANSMITTER 0
#define RECEIVER 1