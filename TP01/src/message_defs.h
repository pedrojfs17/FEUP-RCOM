#pragma once

#define MAX_PACKET_SIZE 256 // minimum 16
#define MAX_BUFFER_SIZE (MAX_PACKET_SIZE * 2 + 7)

#define BCC1_ERROR_PERCENTAGE 0
#define BCC2_ERROR_PERCENTAGE 0
#define T_PROP 0

#define NO_RESPONSE -1

#define MSG_FLAG 0x7e
#define ESCAPE 0x7d

#define MSG_A_TRANS_COMMAND 0x03
#define MSG_A_RECV_RESPONSE 0x03
#define MSG_A_TRANS_RESPONSE 0x01
#define MSG_A_RECV_COMMAND 0x01

#define MSG_CTRL_SET 0x03
#define MSG_CTRL_UA 0x07
#define MSG_CTRL_RR(r) ((r == 0) ? 0x05 : 0x85)
#define MSG_CTRL_REJ(r) ((r == 0) ? 0x01 : 0x81)
#define MSG_CTRL_DISC 0x0b
#define MSG_CTRL_S(r) ((r == 0) ? 0x00 : 0x40)

#define BCC(addr, ctrl) (addr^ctrl)

#define MSG_SET_SIZE 5

#define COM0 0
#define COM1 1
#define COM10 10
#define COM11 11

#define TRANSMITTER 0
#define RECEIVER 1