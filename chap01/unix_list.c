#define _GNU_SOURCE /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  // call getifaddrs to get addresses of local host

  struct ifaddrs *addrs;
  sa_family_t family;
  int s;
  char host[NI_MAXHOST];

  if (getifaddrs(&addrs) == -1) {
    printf("Bad\n");
    return 1;
  }

  for (struct ifaddrs *ifa = addrs; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;
    family = ifa->ifa_addr->sa_family;

    printf("--------------------------------\n");
    printf("%-8s %s (%d)\n", ifa->ifa_name,
           (family == AF_PACKET)  ? "AF_PACKET"
           : (family == AF_INET)  ? "AF_INET"
           : (family == AF_INET6) ? "AF_INET6"
                                  : "???",
           family);

    if (family == AF_INET || family == AF_INET6) {
      s = getnameinfo(ifa->ifa_addr,
                      (family == AF_INET) ? sizeof(struct sockaddr_in)
                                          : sizeof(struct sockaddr_in6),
                      host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (s != 0) {
        printf("getnameinfo() failed: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
      }

      printf("address: %s\n", host);

    } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
      struct rtnl_link_stats *stats = ifa->ifa_data;

      printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
             "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
             stats->tx_packets, stats->rx_packets, stats->tx_bytes,
             stats->rx_bytes);
    }
  }

  // free the addresses
  freeifaddrs(addrs);
  return 0;
}
