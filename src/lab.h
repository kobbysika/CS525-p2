#ifndef LAB_H
#define LAB_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define GBN_PAYLOAD_MAX 1024u
#define GBN_HEADER_SIZE 10u
#define GBN_PACKET_MAX (GBN_HEADER_SIZE + GBN_PAYLOAD_MAX)
#define GBN_WINDOW_MAX 64u
enum packet_type { PACKET_DATA = 0, PACKET_ACK = 1, PACKET_FIN = 2 };
struct packet { uint8_t type; uint32_t seq; uint16_t length; uint8_t payload[GBN_PAYLOAD_MAX]; };
uint16_t internet_checksum(const uint8_t *data, size_t length);
size_t packet_encode(const struct packet *packet, uint8_t *wire, size_t capacity);
bool packet_decode(const uint8_t *wire, size_t size, struct packet *packet);
struct sender {
    const uint8_t *data; size_t data_size; uint32_t data_packets;
    uint32_t base, next, window; uint64_t timeout_ms, deadline_ms;
    unsigned consecutive_timeouts; bool timer_running, failed;
};
void sender_init(struct sender *, const uint8_t *, size_t, uint32_t, uint64_t);
bool sender_next_packet(struct sender *, uint64_t, struct packet *);
void sender_ack(struct sender *, uint32_t, uint64_t);
size_t sender_timeout(struct sender *, uint64_t, struct packet *, size_t);
bool sender_done(const struct sender *);
uint64_t sender_deadline(const struct sender *);
struct receiver { uint32_t expected; bool finished; };
void receiver_init(struct receiver *);
bool receiver_packet(struct receiver *, const struct packet *, uint8_t *, size_t,
                     size_t *, struct packet *);
#endif
