#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "helper.h"

#define SRC_IP "127.0.0.1"  // Source IP address
#define DEST_IP "127.0.0.1" // Destination IP address
#define SRC_PORT 12345      // Source port
#define DEST_PORT 54321     // Destination port

int LENGTH_SET = 6;
// Predefined pairwise coprime sets
int prime1[] = {101, 103, 107, 109, 113, 127};
int prime2[] = {131, 137, 139, 149, 151, 157};
int prime3[] = {163, 167, 173, 179, 181, 191};
int prime4[] = {193, 197, 199, 211, 223, 227};
int prime5[] = {229, 233, 239, 241, 251, 257};
int prime6[] = {263, 269, 271, 277, 281, 283};
int prime7[] = {293, 307, 311, 313, 317, 331};
int prime8[] = {337, 347, 349, 353, 359, 367};
int prime9[] = {373, 379, 383, 389, 397, 401};
int prime10[] = {409, 419, 421, 431, 433, 439};
int prime11[] = {439, 443, 449, 457, 461, 463};
int prime12[] = {467, 479, 487, 491, 499, 503};
int prime13[] = {509, 521, 523, 541, 547, 557};
int prime14[] = {563, 569, 571, 577, 587, 593};
int prime15[] = {599, 601, 607, 613, 617, 619};
int prime16[] = {631, 641, 643, 647, 653, 659};
int *prime_sets[] = {prime1, prime2, prime3, prime4, prime5, prime6, prime7, prime8, prime9, prime10, prime11, prime12, prime13, prime14, prime15, prime16};

// Function to send data over TCP
void sendData(const char *buffer, size_t size, int prime_set_num)
{

    // Embed prime_set_num in sequence number
    long seq = 1;
    for (int i=0; i<9; i++){
        seq *= random()%9+1;
        seq += random()%9;
    }

    seq = seq - seq%16 + prime_set_num-1;


    int sockfd;
    struct sockaddr_in dest_addr;
    char packet[sizeof(struct ip) + sizeof(struct tcphdr) + size];

    // Create a raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0)
    {
        perror("socket");
        return;
    }

    // Fill in the IP header
    struct ip *ip_header = (struct ip *)packet;
    ip_header->ip_hl = 5;
    ip_header->ip_v = 4;
    ip_header->ip_tos = 0;
    ip_header->ip_len = htons(sizeof(struct ip) + sizeof(struct tcphdr) + size);
    ip_header->ip_id = htons(12345); // ID of this packet
    ip_header->ip_off = 0;
    ip_header->ip_ttl = 64;
    ip_header->ip_p = IPPROTO_TCP;
    ip_header->ip_sum = 0; // Let the kernel compute the checksum
    ip_header->ip_src.s_addr = inet_addr(SRC_IP);
    ip_header->ip_dst.s_addr = inet_addr(DEST_IP);

    // Fill in the TCP header
    struct tcphdr *tcp_header = (struct tcphdr *)(packet + sizeof(struct ip));
    tcp_header->th_sport = htons(SRC_PORT);
    tcp_header->th_dport = htons(DEST_PORT);
    tcp_header->th_seq = seq;
    tcp_header->th_ack = 0;
    tcp_header->th_off = sizeof(struct tcphdr) / 4;
    tcp_header->th_flags = TH_SYN;
    tcp_header->th_win = htons(65535);
    tcp_header->th_sum = 0;
    tcp_header->th_urp = 0;

    // Set the destination address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    if (inet_pton(AF_INET, DEST_IP, &(dest_addr.sin_addr)) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        return;
    }

    // Copy data to the packet buffer
    memcpy(packet + sizeof(struct ip) + sizeof(struct tcphdr), buffer, size);

    // Send the packet
    if (sendto(sockfd, packet, sizeof(struct ip) + sizeof(struct tcphdr) + size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0)
    {
        perror("sendto");
        close(sockfd);
        return;
    }

    close(sockfd);
}


int main(int argc, char* argv[])
{
    if (argc != 3){
        printf("Usage: %s <text> <prime_set_number>\n", argv[0]);
        return 1;
    }

    // Take input string and the set of primes used for CRT
    char *input_txt = argv[1];
    int prime_set_num = atoi(argv[2]);


    if (prime_set_num < 1 || prime_set_num > 16) {
        printf("Error: Invalid set number. Please choose a set number between 1 and 16.\n");
        return 1;
    }

    printf("%s\n", input_txt);
    // Handling if length input_txt is not divisible by 4
    // Adds '\' characters to pad
    int len = strlen(input_txt);
    int extra = len%4;
    char cat[5] = {0};
    if (extra){
        for (int j=0; j<4-extra; j++){
            cat[j] = '\\';
        }
        strcat(input_txt, cat);
    }


    // Sends the string
    printf("%s\n", input_txt);
    for (int i=0; i<strlen(input_txt)/4; i++){
        char temp[5] = {input_txt[4*i], input_txt[4*i+1], input_txt[4*i+2], input_txt[4*i+3], '\0'};

        int *moduli = prime_sets[(prime_set_num-1+i)%16];
        int rem[LENGTH_SET];
        txt_to_ascii_rem(temp, rem, moduli);
        long long solution = CRT(rem, moduli);
        int bits_sol = (int)log2(solution) + 1;
        char bits_sol_str[2] = {0};
        sprintf(bits_sol_str, "%x", bits_sol);
        int extra_bytes = bits_sol%4 ? bits_sol/4+2 : bits_sol/4+1;

        char rand[] = "0123456789abcdef";
        srand(time(NULL));
        char buf[100] = {0};
        for (int l=0;l<40-extra_bytes+1;l++){
            if (l == 4){
                buf[l] = bits_sol_str[0];
            } else if ( l == 5){
                buf[l] = bits_sol_str[1];
            }else {
                buf[l]= rand[random() % strlen(rand)];
            }
        }
        

        char sendDataBuffer[1000] = {0};
        sprintf(sendDataBuffer, "POST /post/upvote HTTP/1.1\nHost: useful-it-forum.net\nContent-Type: application/x-www-form-urlencoded\nCSRF=%s%llx",buf,solution);
        // Send data
        sendData(sendDataBuffer, strlen(sendDataBuffer), (prime_set_num+i)%16);
    }
    return 0;
}