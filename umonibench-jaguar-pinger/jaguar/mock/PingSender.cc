//
// Created by blackwood on 12/11/16.
//
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <jaguar/common/IcmpSockets.h>
#include <muduo/net/Endian.h>
#include <muduo/base/Logging.h>


char *ping_buffer = 0;
size_t ping_pkt_size;

int open_ping_socket_ipv4();

void init_ping_buffer_ipv4(size_t ping_data_size);

void errno_crash_and_burn(char *message);

int socket_sendto_ping_ipv4(int s, struct sockaddr *saddr, socklen_t saddr_len, uint16_t icmp_seq_nr,
                            uint16_t icmp_id_nr);


unsigned short calcsum(unsigned short *buffer, int length);


void crash_and_burn(char *message);


int socket_sendto_ping_ipv4(int s, struct sockaddr *saddr, socklen_t saddr_len, uint16_t icmp_seq_nr,
                            uint16_t icmp_id_nr) {
    struct icmp *icp;
    int n;

    icp = (struct icmp *) ping_buffer;

    icp->icmp_type = ICMP_ECHO;
    icp->icmp_code = 0;
    icp->icmp_cksum = 0;
    icp->icmp_seq = htons(icmp_seq_nr);
    icp->icmp_id = htons(icmp_id_nr);

//    for (n = ((void *) &icp->icmp_data - (void *) icp); n < ping_pkt_size; ++n) {
//        ping_buffer[n] = random() & 0xFF;
//    }

    icp->icmp_cksum = calcsum((unsigned short *) icp, ping_pkt_size);

    n = sendto(s, icp, ping_pkt_size, 0, saddr, saddr_len);

    return n;
}


int open_ping_socket_ipv4() {
    struct protoent *proto;
    int s;

    /* confirm that ICMP is available on this machine */
    if ((proto = getprotobyname("icmp")) == NULL)
        crash_and_burn("icmp: unknown protocol");

    /* create raw socket for ICMP calls (ping) */
    s = socket(AF_INET, SOCK_RAW, proto->p_proto);
    if (s < 0) {
        /* try non-privileged icmp (works on Mac OSX without privileges, for example) */
        s = socket(AF_INET, SOCK_DGRAM, proto->p_proto);
        if (s < 0) {
            errno_crash_and_burn("can't create socket (must run as root?)");
        }
    }

    return s;
}


void init_ping_buffer_ipv4(size_t ping_data_size) {
    /* allocate ping buffer */
    ping_pkt_size = ping_data_size + ICMP_MINLEN;
    ping_buffer = (char *) calloc(1, ping_pkt_size);
    if (!ping_buffer)
        crash_and_burn("can't malloc ping packet");
}


void errno_crash_and_burn(char *message) {
    fprintf(stderr, "%d: %s : %s\n", getpid(), message, strerror(errno));
    exit(4);
}

void crash_and_burn(char *message) {
    exit(4);
}


unsigned short calcsum(unsigned short *buffer, int length) {
    unsigned long sum;

    // initialize sum to zero and loop until length (in words) is 0
    for (sum = 0; length > 1; length -= 2) // sizeof() returns number of bytes, we're interested in number of words
        sum += *buffer++;    // add 1 word of buffer to sum and proceed to the next

    // we may have an extra byte
    if (length == 1)
        sum += (char) *buffer;

    sum = (sum >> 16) + (sum & 0xFFFF); // add high 16 to low 16
    sum += (sum >> 16);            // add carry
    return ~sum;
}


int main() {


    init_ping_buffer_ipv4(150);

    int fd = common::IcmpSockets::GetSenderIcmpSocket();
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = 3470773871;

    int id = getpid();
    for (int index = 0; index < 100000;) {
        int n = socket_sendto_ping_ipv4(fd, (struct sockaddr *) &target, sizeof(sockaddr_in), index, id);
        if (n >= 0) {
            LOG_INFO << "SendIpOk ip=" << target.sin_addr.s_addr;
        }
        usleep(100 * 1000);
    }
    LOG_INFO << "SenderDone";

}
