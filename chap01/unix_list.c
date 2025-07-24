#if defined(__linux__)
#define _GNU_SOURCE /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <linux/if_link.h>
#elif defined(__APPLE__)
#include <net/if.h>
#include <net/if_dl.h>
#endif

#include <arpa/inet.h>
#include <ifaddrs.h>
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
#if defined(__linux__)
           (family == AF_PACKET)  ? "AF_PACKET" :
#elif defined(__APPLE__)
           (family == AF_LINK) ? "AF_LINK" :
#endif
           (family == AF_INET)    ? "AF_INET"
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

#if defined(__linux__)
    } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
      struct rtnl_link_stats *stats = ifa->ifa_data;

      printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
             "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
             stats->tx_packets, stats->rx_packets, stats->tx_bytes,
             stats->rx_bytes);
#elif defined(__APPLE__)
    } else if (family == AF_LINK && ifa->ifa_data != NULL) {
      struct if_data *stats = (struct if_data *)ifa->ifa_data;

      printf("\t\trx_packets = %10llu; tx_packets = %10llu\n"
             "\t\trx_bytes   = %10llu; tx_bytes   = %10llu\n",
             (unsigned long long)stats->ifi_ipackets,
             (unsigned long long)stats->ifi_opackets,
             (unsigned long long)stats->ifi_ibytes,
             (unsigned long long)stats->ifi_obytes);
#endif
    }
  }

  // free the addresses
  freeifaddrs(addrs);
  return 0;
}
