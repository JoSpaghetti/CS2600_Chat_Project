<<<<<<< Updated upstream
#include <stdio.h>

=======
#include <sys.h>
#include <socket.h>i

int main() {
  //fd for the socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) report ("socket", 1);

  //gets the address of the host
  struct hostent* hptr = gethostbyname("127.0.0.1");
  if (!htpr) report ("gethostbyname", 1);
  if (htpr->h_addrtype != AF_INET)
    report("bad address family", 1);

  struct socaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr)
  
}
>>>>>>> Stashed changes
