#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>

#ifndef BACKLOG
#define BACKLOG 8
#endif

#ifndef REQUEST_MAX_SIZE
// Ain't nobody got time for listening to your bullshit!!
#define REQUEST_MAX_SIZE 128
#endif

#ifndef DUMB_RESPONSE
// Show me some love, I could Rick Roll you!
#define DUMB_RESPONSE "HTTP/1.1 200 OK\nContent-Type: text/html\n\nHi there! I am a dumb web server\n"
#endif

int main(int argc, char * argv[]) {
  // I need port
  if (argc <= 1) return EINVAL;
  int port = atoi(argv[1]);
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

    // You know what? I'm not gonna check if memory can be allocated or not
    char * req = (char *) malloc(sizeof(char)*REQUEST_MAX_SIZE);

    // Pfft... kid's stuff
    int req_len = read(cd, req, REQUEST_MAX_SIZE);

    // Good boy stuff
    free(req);

    // If you don't talk to me or I can't hear you,
    // this dumb motherfucker ain't gonna talk
    if (req_len > 0) write(cd, response, sizeof(response));

    // Good boy stuff again
    close(cd);
  }
  return 0;
}
