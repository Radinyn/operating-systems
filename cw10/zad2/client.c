#include "common.h"
#include "message.h"

int _;

int connect_unix(char* path, char* user) {
  struct sockaddr_un addr, bind_addr;
  memset(&addr, 0, sizeof(addr));
  bind_addr.sun_family = AF_UNIX;
  addr.sun_family = AF_UNIX;
  snprintf(bind_addr.sun_path, sizeof bind_addr.sun_path, "/tmp/%s%ld", user, time(NULL));
  strncpy(addr.sun_path, path, sizeof addr.sun_path);

  int sock = safe (socket(AF_UNIX, SOCK_DGRAM, 0));	
  safe (bind(sock, (void*) &bind_addr, sizeof addr));
  safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));

  return sock;
}

int connect_web(char* ipv4, int port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
    print("Invalid address\n");
    exit(0);
  }

  int sock = safe (socket(AF_INET, SOCK_DGRAM, 0));
  safe (connect(sock, (struct sockaddr*) &addr, sizeof addr));
   
  return sock;
}

int sock;
void on_SIGINT(int _) {
  message msg = { .type = msg_disconnect };
  sendto(sock, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
  exit(0);
}

int main(int argc, char** argv) {
  char* nickname;
  if (argc > 2) nickname = argv[1];
  if (argc == 5 && strcmp(argv[2], "web") == 0) sock = connect_web(argv[3], atoi(argv[4]));
  else if (argc == 4 && strcmp(argv[2], "unix") == 0) sock = connect_unix(argv[3], nickname);
  else {
    print("Usage [nick] [web|unix] [ip port|path]\n");
    exit(0);
  }

  signal(SIGINT, on_SIGINT);

  message msg = { .type = msg_connect };
  strncpy(msg.nickname, nickname, sizeof msg.nickname);
  send(sock, &msg, sizeof msg, 0);
  
  int epoll_fd = safe (epoll_create1(0));
  
  struct epoll_event stdin_event = { 
    .events = EPOLLIN | EPOLLPRI,
    .data = { .fd = STDIN_FILENO }
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

  struct epoll_event socket_event = { 
    .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
    .data = { .fd = sock }
  };
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &socket_event);

  struct epoll_event events[2];
  loop {
    int nread = safe (epoll_wait(epoll_fd, events, 2, 1));
    repeat(nread) {
      if (events[i].data.fd == STDIN_FILENO) {
        // puts("got stdin");
        // fflush(NULL);

        char buffer[512] = {};

        // POWAŻNY PARSER

        int x = read(STDIN_FILENO, &buffer, 512);
        buffer[x] = 0;

        char korektor[] = " \t\n";
        char *token;
        int idx = 0;
        token = strtok( buffer, korektor );

        message_type type = -1;
        char other_nickname[MSG_LEN] = {};
        char text[MSG_LEN] = {};

        bool broke = false;

        if (token == NULL)
          continue;

        while( token != NULL ) {
            // puts("TOKEN:");
            // puts(token);
            // fflush(NULL);
            
            switch (idx) {
              case 0:
                if (strcmp(token, "LIST") == 0) type = msg_list;
                else if (strcmp(token, "2ALL") == 0) type = msg_tall;
                else if (strcmp(token, "2ONE") == 0) type = msg_tone;
                else if (strcmp(token, "STOP") == 0) type = msg_stop;
                else {
                  broke = true;
                  puts("Invalid command");
                  type = -1;
                }
                break;
              case 1:
                memcpy(text, token, strlen(token)*sizeof(char));
                text[strlen(token)] = 0;
                break;
              case 2:
                memcpy(other_nickname, token, strlen(token)*sizeof(char));
                other_nickname[strlen(token)] = 0;
                break;
              case 3:
                broke = true;
                break;
            }

            if (broke)
              break;

            idx++;
            token = strtok( NULL, korektor );
        }
        if (broke)
          continue;

        // puts("parsed");
        // fflush(NULL);

        message msg;
        msg.type = type;
        memcpy(&msg.other_nickname, other_nickname, strlen(other_nickname)+1);
        memcpy(&msg.text, text, strlen(text)+1);

        sendto(sock, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
        ///



      } else { // TODO(rad)
        // puts("got msg");
        // fflush(NULL);
        message msg;
        recvfrom(sock, &msg, sizeof msg, 0, NULL, NULL);

        if (msg.type == msg_username_taken) {
          puts("This username is already taken\n");
          close(sock);
          exit(0);
        } else if (msg.type == msg_server_full) {
          puts("Server is full\n");
          close(sock);
          exit(0);
        } else if (events[i].events & EPOLLHUP) {
          puts("Disconnected\n");
          exit(0);
        } else if (msg.type == msg_ping) {
          sendto(sock, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
        } else if (msg.type == msg_stop) {
          puts("Stopping\n");
          close(sock);
          exit(0);
        } else if (msg.type == msg_get) {
          puts(msg.text);
        }
      }


    }
  }
}