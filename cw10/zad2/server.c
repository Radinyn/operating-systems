#include "common.h"
#include "message.h"
#include <pthread.h>

#define MAX_CONN 16
#define PING_INTERVAL 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

union addr {
  struct sockaddr_un uni;
  struct sockaddr_in web;
};
typedef struct sockaddr* sa;

int epoll_fd;

struct client {
  union addr addr;
  int sock, addrlen;
  enum client_state { empty = 0, waiting, playing } state;
  struct client* peer;
  char nickname[16];
  bool responding;
} clients[MAX_CONN], *waiting_client = NULL;
typedef struct client client;

typedef struct event_data {
  enum event_type { socket_event, client_event } type;
  union payload { client* client; int socket; } payload;
} event_data;

void delete_client(client* client) {
  printf("Deleting %s\n", client->nickname);
  message msg = { .type = msg_disconnect };
  sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
  memset(&client->addr, 0, sizeof client->addr);
  client->state = empty;
  client->sock = 0;
  client->nickname[0] = 0;
}

void send_msg(client* client, message_type type, char text[MSG_LEN]) {
  message msg;
  msg.type = type;
  memcpy(&msg.text, text, MSG_LEN*sizeof(char));
  sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
}

void on_client_message(client* client, message* msg_ptr) {
  message msg = *msg_ptr;

  printf("Got msg %i %s\n", (int)msg.type, msg.text);

  if (msg.type == msg_ping) {
    pthread_mutex_lock(&mutex);
    printf("pong %s\n", client->nickname);
    client->responding = true;
    pthread_mutex_unlock(&mutex);
  } else if (msg.type == msg_disconnect || msg.type == msg_stop) {
    pthread_mutex_lock(&mutex);
    delete_client(client);
    pthread_mutex_unlock(&mutex);
  } else if (msg.type == msg_tall) {
    char out[256] = "";
    strcat(out, client->nickname);
    strcat(out, ": ");
    strcat(out, msg.text);

    for (int i = 0; i < MAX_CONN; i++) {
      if (clients[i].state != empty)
        send_msg(clients+i, msg_get, out);
    }
  } else if (msg.type == msg_list) {
    for (int i = 0; i < MAX_CONN; i++) {
      if (clients[i].state != empty)
        send_msg(client, msg_get, clients[i].nickname);
    }
  } else if (msg.type == msg_tone) {
    char out[256] = "";
    strcat(out, client->nickname);
    strcat(out, ": ");
    strcat(out, msg.text);

    for (int i = 0; i < MAX_CONN; i++) {
      if (clients[i].state != empty) {
        if (strcmp(clients[i].nickname, msg.other_nickname) == 0) {
          send_msg(clients+i, msg_get, out);
        }
      }
    }
  } 
}

void init_socket(int socket, void* addr, int addr_size) {
  safe (bind(socket, (struct sockaddr*) addr, addr_size));
  struct epoll_event event = { 
    .events = EPOLLIN | EPOLLPRI, 
    .data = { .fd = socket } 
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event);
}

void new_client(union addr* addr, socklen_t addrlen, int sock, char* nickname) {
  pthread_mutex_lock(&mutex);
  int empty_index = -1;
  for (int i = 0; i < MAX_CONN; i++) {
    if (clients[i].state == empty) empty_index = i;
    else if (strncmp(nickname, clients[i].nickname, sizeof clients->nickname) == 0) {
      pthread_mutex_unlock(&mutex);
      message msg = {.type = msg_username_taken };
      printf("Nickname %s already taken\n", nickname);
      sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
      return;
    }
  }
  if (empty_index == -1) {
    pthread_mutex_unlock(&mutex);
    printf("Server is full\n");
    message msg = { .type = msg_server_full };
    sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
    return;
  }
  printf("New client %s\n", nickname);
  client* client = &clients[empty_index];
  memcpy(&client->addr, addr, addrlen);
  client->addrlen = addrlen;
  client->state = waiting;
  client->responding = true;
  client->sock = sock;

  memset(client->nickname, 0, sizeof client->nickname);
  strncpy(client->nickname, nickname, sizeof client->nickname - 1);

  pthread_mutex_unlock(&mutex);
}


void* ping(void* _) {
  const static message msg = { .type = msg_ping };
  loop {
    sleep(PING_INTERVAL);
    pthread_mutex_lock(&mutex);
    printf("Pinging clients\n");
    for (int i = 0; i < MAX_CONN; i++) {
      if (clients[i].state != empty) {
        if (clients[i].responding) {
          clients[i].responding = false;
          sendto(clients[i].sock, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addrlen);
        }
        else delete_client(&clients[i]);
      }
    }
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    print("Usage [port] [path]\n");
    exit(0);
  }
  int port = atoi(argv[1]);
  char* socket_path = argv[2];

  epoll_fd = safe (epoll_create1(0));

  struct sockaddr_un local_addr = { .sun_family = AF_UNIX };
  strncpy(local_addr.sun_path, socket_path, sizeof local_addr.sun_path);

  struct sockaddr_in web_addr = {
    .sin_family = AF_INET, .sin_port = htons(port),
    .sin_addr = { .s_addr = htonl(INADDR_ANY) },
  };

  unlink(socket_path);
  int local_sock = safe (socket(AF_UNIX, SOCK_DGRAM, 0));
  init_socket(local_sock, &local_addr, sizeof local_addr);

  int web_sock = safe (socket(AF_INET, SOCK_DGRAM, 0));
  init_socket(web_sock, &web_addr, sizeof web_addr);

  pthread_t ping_thread;
  pthread_create(&ping_thread, NULL, ping, NULL);

  struct epoll_event events[10];
  loop {
    int nread = safe (epoll_wait(epoll_fd, events, 10, -1));
    repeat (nread) {
      int sock = events[i].data.fd;
      message msg;
      union addr addr;
      socklen_t addrlen = sizeof addr;
      recvfrom(sock, &msg, sizeof msg, 0, (sa) &addr, &addrlen);
      if (msg.type == msg_connect) {
        new_client(&addr, addrlen, sock, msg.nickname);
      } else {
        int i = find(int i = 0; i < MAX_CONN; i++, memcmp(&clients[i].addr, &addr, addrlen) == 0);
        on_client_message(&clients[i], &msg);
      }
    }
  }
}