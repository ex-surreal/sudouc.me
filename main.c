#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>

#ifndef BACKLOG
#define BACKLOG 8
#endif

#ifndef DUMB_RESPONSE
// Show me some love, I could Rick Roll you!
#define DUMB_RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nHi there! I am a dumb web server\r\n"
#endif

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

  // Maximum length for the queue of pending connections = BACKLOG; ref listen(2)
  if (listen(sd, BACKLOG)) return errno;

  // Dumb response can stay in stack
  char response[] = DUMB_RESPONSE;

  // Not gonna log anything
  while (1) {
    // Open up incoming descriptor; ref: accept(2)
    int cd = accept(sd, NULL, NULL);
    if (cd < 0) continue;

    // You know what?
    // I'm not gonna listen to whatever you are saying!

    // dumb response here I come
    write(cd, response, sizeof(response));

    // Good boy stuff
    close(cd);
  }
  return 0;
}
