#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#ifndef __linux
#define __FAVOR_BSD
#endif /* __linux */
#include <netinet/ip.h>

#define STR_BUF 16
#define INVALID_NETMASK (0x12345678)

static void usage(const char *cmd);
static void print(int slash, in_addr_t network, in_addr_t netmask, in_addr_t broadcast);
static const char *int2bin(in_addr_t i);
static const char *ip_ntoa(in_addr_t i);
static in_addr_t ip_aton(const char *ip_addr_str);
static void print_bin(const char *bin);
static void dump_ip_slash(const char *ip_slash);
static in_addr_t generate_netmask(int slash);
static u_int32_t available_ip_num(in_addr_t network, in_addr_t broadcast);
static void dump_ip_netmask(const char *ip, const char *netmask_str);
static int netmask_to_slash(const char *netmask); /* -1 is invalid */

int main(int argc, char *argv[]) {

    if(argc == 2) {
        dump_ip_slash(argv[1]);
    }//end if
    else if(argc == 3) {
        dump_ip_netmask(argv[1], argv[2]);
    }//end if
    else {
        usage(argv[0]);
    }//end else

    return 0;
}//end main

static void usage(const char *cmd) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s ip/slash\n", cmd);
    fprintf(stderr, "\t%s ip netmask\n", cmd);
    exit(1);
}//end usage

static void print(int slash, in_addr_t network, in_addr_t netmask, in_addr_t broadcast) {
    printf("Network:           %s\n", ip_ntoa(network));
    printf("Netmask:           %s\n", ip_ntoa(netmask));
    printf("Bitmask :          %d\n", slash);
    printf("Broadcast:         %s\n", ip_ntoa(broadcast));
    printf("Range:             %s - %s\n", ip_ntoa(network), ip_ntoa(broadcast));

    printf("Network(Binary):   ");
    print_bin(int2bin(ntohl(network)));

    printf("Netmask(Binary):   ");
    print_bin(int2bin(ntohl(netmask)));

    printf("Broadcast(Binary): ");
    print_bin(int2bin(ntohl(broadcast)));

    printf("Available IP address: %u\n", available_ip_num(network, broadcast));

}//end print

static const char *ip_ntoa(in_addr_t i) {
    static char ip[STR_BUF][INET_ADDRSTRLEN];
    static int which = -1;

    which = (which + 1 == STR_BUF ? 0 : which + 1);
    
    memset(ip[which], 0, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &i, ip[which], sizeof(ip[which]));
    
    return ip[which];
}//end ip_ntoa

static in_addr_t ip_aton(const char *ip_addr_str) {
    u_int32_t addr;
    if(1 != inet_pton(AF_INET, ip_addr_str, &addr)) {
        fprintf(stderr, "Invalid IP address: %s\n", ip_addr_str);
        exit(1);
    }//end if
    return addr;
}//end ip_aton

static const char *int2bin(in_addr_t i) {
    static char str[STR_BUF][256];
    static int which = -1;

    which = (which + 1 == STR_BUF ? 0 : which + 1);
    
    memset(str[which], 0, sizeof(str[which]));

    // type punning because signed shift is implementation-defined
    unsigned u = *(unsigned *)&i;
    for(int bits = 32; bits--; u >>= 1) {
        str[which][bits] = u & 1 ? '1' : '0';
    }//end for

    return str[which];
}//end int2bin

static void print_bin(const char *bin) {
    int len = strlen(bin);
    int count = 0;
    for(int i = 0 ; i < len ; i++) {
        printf("%c", bin[i]);
        if((i+1) % 8 == 0 && i+1 != len)
            printf(".");
    }//end for
    printf("\n");
}//end bin

static void dump_ip_slash(const char *ip_slash) {
    char *ip = strdup(ip_slash);
    if(!ip) {
        perror("strdup()");
        exit(1);
    }//end if

    char *slash = strchr(ip, '/');
    if(!slash) {
        fprintf(stderr, "Invalid IP address with slash: %s\n", ip_slash);
        goto BYE;
    }//end if

    *(slash++) = '\0';
    if(atoi(slash) < 0 || atoi(slash) > 32) {
        fprintf(stderr, "Invalid IP address with slash: %s\n", ip_slash);
        goto BYE;
    }//end if

    in_addr_t ip_addr = ip_aton(ip);
    in_addr_t netmask = generate_netmask(atoi(slash));
    in_addr_t network = ip_addr & netmask;
    in_addr_t broadcast = network | ~netmask;

    printf("Input:             %s\n", ip_slash);
    print(atoi(slash), network, netmask, broadcast);

BYE:
    if(ip) {
        free(ip);
    }//end if
}//end dump_ip_slash

static in_addr_t generate_netmask(int slash) {
    in_addr_t netmask = 0;
    in_addr_t mask = 1 << 31;
    for(int i = 0 ; i < slash ; i++) {
        netmask |= mask;
        mask >>= 1;
    }
    return htonl(netmask);
}//end generate_netmask

static u_int32_t available_ip_num(in_addr_t network, in_addr_t broadcast) {
    u_int32_t start = ntohl(network);
    u_int32_t end = ntohl(broadcast);
    return end - start + 1U;
}//end available_ip_num

static void dump_ip_netmask(const char *ip, const char *netmask_str) {
    int slash = netmask_to_slash(netmask_str);

    if(slash == INVALID_NETMASK) {
        fprintf(stderr, "Invalid netmask: %s\n", netmask_str);
        return;
    }//end if

    in_addr_t ip_addr = ip_aton(ip);
    in_addr_t netmask = generate_netmask(slash);
    in_addr_t network = ip_addr & netmask;
    in_addr_t broadcast = network | ~netmask;
    printf("Input:             %s %s\n", ip, netmask_str);
    print(slash, network, netmask, broadcast);

}//end dump_ip_netmask

static int netmask_to_slash(const char *netmask) {
    in_addr_t addr = ntohl(ip_aton(netmask));
    int slash = 0;

    in_addr_t valid_addr[] = {
        0x00000000U, 0x80000000U, 0xc0000000U, 0xe0000000U, 0xf0000000U,
        0xf8000000U, 0xfc000000U, 0xfe000000U, 0xff000000U, 0xff800000U,
        0xffc00000U, 0xffe00000U, 0xfff00000U, 0xfff80000U, 0xfffc0000U,
        0xfffe0000U, 0xffff0000U, 0xffff8000U, 0xffffc000U, 0xffffe000U,
        0xfffff000U, 0xfffff800U, 0xfffffc00U, 0xfffffe00U, 0xffffff00U,
        0xffffff80U, 0xffffffc0U, 0xffffffe0U, 0xfffffff0U, 0xfffffff8U,
        0xfffffffcU, 0xfffffffeU, 0xffffffffU
    };

    for(slash = 0 ; slash < sizeof(valid_addr)/sizeof(valid_addr[0]) ; slash++) {
        if(valid_addr[slash] == addr) {
            break;
        }//end if
    }//end for

    return slash >= sizeof(valid_addr)/sizeof(valid_addr[0])
    ? INVALID_NETMASK : slash;
}//end netmask_to_slash