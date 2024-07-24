#include <list>
#include <stdint.h>

#include <esp_now.h>

#define MAC_LEN  16
#define TTL_LEN  1
#define NAME_LEN 16
#define TEXT_LEN ESP_NOW_MAX_DATA_LEN - (MAC_LEN + TTL_LEN + NAME_LEN)

typedef struct message_pkt {
    uint8_t ttl;
    uint8_t mac[MAC_LEN];
    char *username[NAME_LEN];
    char *text[TEXT_LEN];

} message_pkt_t;

