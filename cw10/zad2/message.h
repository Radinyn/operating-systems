#define MSG_LEN 256

typedef enum {
    msg_ping,
    msg_username_taken,
    msg_server_full,
    msg_disconnect,
    msg_get,
    msg_init,
    msg_list,
    msg_tone,
    msg_tall,
    msg_stop,
    msg_connect,
  } message_type;

typedef struct {
  message_type type;
  char text[MSG_LEN]; // INCLUDES NICKNAME
  char nickname[MSG_LEN];
  char other_nickname[MSG_LEN];
} message;
