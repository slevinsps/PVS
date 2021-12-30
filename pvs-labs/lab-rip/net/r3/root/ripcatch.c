/* Перехватчик RIP-трафика */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* размер буфера для входящих датаграмм */
#define BUFSIZE 512

struct rip_route {
    uint8_t code;
    uint8_t addr_family;
    uint16_t tag;
    struct in_addr addr;
    struct in_addr mask;
    struct in_addr nexthop;
    uint32_t metric;
};

/* вычисление размера CIDR-маски */
int mask_len(struct in_addr mask) {
    int len = 0;
    int bit_pos = 0;
    while ((bit_pos < 32) && (mask.s_addr & (1 << bit_pos))) {
        len++; bit_pos++;
    }
    return len;
}

/* вывод в консоль содержания RIP-сообщения
 * (сообщение занимает первые `len` байт в буфере)
 */
void dump_rip(char *buffer, int len) {
    struct rip_route *rt = (struct rip_route *)(buffer + 4);
    char *cur;
    while ((char*) rt < buffer + len) {
        printf("\tAFI %d addr %s/%d, tag 0x%04x, metric %u",
            rt->addr_family, inet_ntoa(rt->addr), mask_len(rt->mask), 
            rt->tag, ntohl(rt->metric));
        printf(",  nexthop: %s\n", inet_ntoa(rt->nexthop));
        rt++;
    }
}

int main(void) {
    struct sockaddr_in addr;    /* для своего адреса */
    struct sockaddr_in sender;  /* для адреса отправителя */ 
    socklen_t addrlen;          /* размер структуры с адресом */
    int sk;                     /* файловый дескриптор сокета */
    char buffer[BUFSIZE+1];     /* буфер для приёма датаграмм */
    ssize_t size;               /* размер полученных данных */
    int optvalue;               /* значение параметра сокета */
    struct ip_mreqn membership; /* информация о multicast-группе */
    
    memset(&membership, 0, sizeof(membership));
    membership.imr_multiaddr.s_addr = inet_addr("224.0.0.9"); /* адрес группы */
    membership.imr_address.s_addr = INADDR_ANY;
    membership.imr_ifindex = 3; /* индекс сетевого интерфейса eth0 (см. `ip a`) */
    if (setsockopt(sk, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &membership,
        sizeof(membership)) < 0) perror("membership"); exit(1);

    /* разрешаем повторное использование адресов */
    int optvalue = 1;
    if (setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, (void *) &optvalue,
        sizeof(optvalue)) < 0) perror("reuse addr"); exit(1);

    /* TODO:
        создать сокет,
        подключиться к multicast-группе,
        связать сокет с портом,
        начать приём сообщений в бесконечном цикле,
        принятые сообщения передавать в функцию dump_rip(buffer, len)
    */
    
    
    return 0;   
}
        
        
