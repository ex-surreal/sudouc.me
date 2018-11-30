#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifndef BACKLOG
#define BACKLOG 8
#endif

#ifndef DUMB_RESPONSE
// Show me some love, I could Rick Roll you!
#define DUMB_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHi there! I am a dumb web server\r\n"
#endif

#define MAX_HEADER_LEN 128
#define CONTENT_LENGHT_KEY "Content-Length: "

int parse_content_len(char * buf) {
  int s = strlen(buf);
  char key[] = CONTENT_LENGHT_KEY;
  int k = strlen(key);
  if (s < k) return 0;
  if (strncmp(key, buf, k) == 0) {
    int len = 0;
    for (int i = k; i < s && buf[i] >= '0' && buf[i] <= '9'; i ++) {
      len = len*10+buf[i]-'0';
    }
    return len;
  }
  return 0;
}

int read_header(int cd) {
  FILE *f = fdopen(cd, "w+");
  char * buf = (char*) malloc(MAX_HEADER_LEN * sizeof(char));
  int len = 0;
  while ((buf = fgets(buf, MAX_HEADER_LEN, f)) != NULL) {
    if (strcmp(buf, "\r\n")) break;
    len = parse_content_len(buf);
  }
  return len;
}

int main() {
  // Heroku needs to get port from env
  char * PORT = getenv("PORT");
  if (errno) return errno;
  if (PORT == NULL) return EINVAL;
  int port = atoi(PORT);
  if (errno) return errno;

  // Open up socket; ref: socket(2)
  int sd = socket(PF_INET, SOCK_STREAM, 0);
  if (sd < 0) return errno;

  // Initialize internet socket address
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  // Bind the socket with an address; ref: bind(2)
  if (bind(sd, (struct sockaddr*)&addr, sizeof(addr))) return errno;

  // maximum length for the queue of pending connections = BACKLOG; ref listen(2)
  if (listen(sd, BACKLOG)) return errno;

  // Dumb response can stay in stack
  char response[] = DUMB_RESPONSE;

  // Not gonna log anything
  while (1) {
    // Open up incoming descriptor; ref: accept(2)
    int cd = accept(sd, NULL, NULL);
    if (cd < 0) continue;

    // Some clients don't send EOF
    // So need to find content length
    int content_length = read_header(cd);

    // Ignore dat body
    for (char c; content_length -- > 0; read(cd, &c, 1));

    // dubm response here I come
    write(cd, response, sizeof(response));

    // Good boy stuff again
    close(cd);
  }
  return 0;
}
