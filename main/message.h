#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>

#include <espnow.h>

#define MAC_LEN     6
#define TTL_LEN     1
#define MSG_NUM_LEN 1
#define NAME_LEN    16
#define TEXT_LEN ESPNOW_DATA_LEN - (MAC_LEN + TTL_LEN + MSG_NUM_LEN + NAME_LEN)

typedef struct message_pkt {
    uint8_t ttl;
    uint8_t mac[MAC_LEN];
    uint8_t msg_num;
    char username[NAME_LEN];
    char text[TEXT_LEN];
} message_pkt_t;

#endif // _MESSAGE_H_