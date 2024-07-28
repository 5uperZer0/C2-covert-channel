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
#include "helper.h"

#define LENGTH_SET 6

char CSRF[] = "CSRF";

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



int csrf_check(char *payload, int payload_length, int num){
    char csrf_check[5];
    for (int j=0; j<4; j++) csrf_check[j] = payload[num+j];
    if (strcmp(csrf_check, CSRF) == 0) return 0;
    return 1;
}

void packetHandler(unsigned char *packetData, int packetSize)
{
    char csrf[41];

    // Decode the IP header
    struct ip *ip_header = (struct ip *)packetData;
    unsigned int ip_header_length = ip_header->ip_hl * 4; // Length of IP header in bytes

    // Decode the TCP header
    struct tcphdr *tcp_header = (struct tcphdr *)(packetData + ip_header_length);
    unsigned int tcp_header_length = tcp_header->th_off * 4; // Length of TCP header in bytes

        // You can access the raw packet data using the 'packetData' pointer here
        // You can print or process the packet data as needed

        // Decode the payload (data after the TCP header)
        unsigned char *payload = packetData + ip_header_length + tcp_header_length;
        int payload_length = packetSize - ip_header_length - tcp_header_length;

        
        if (!(strcmp(inet_ntoa(ip_header->ip_src), "127.0.0.1") == 0 && ntohs(tcp_header->th_dport) == 54321)) return;
        // Print the sequence number
        int prime_offset = tcp_header->th_seq%16;
        for (int l = 0; l < payload_length; l++){
            if (!(l>0)) continue;
            if (!(payload[l-1] == '\n')) continue;
            if (!(csrf_check(payload, payload_length, l) == 0)) continue;
            
            for (int k=0; k<40; k++) {
                csrf[k] = payload[l+k+5];
            }
            char bits_sol_str[3] = {csrf[4], csrf[5], '\0'};
            long bits_sol = strtol(bits_sol_str, NULL, 16);
            int bytes_sol = bits_sol%4 ? bits_sol/4+1 : bits_sol/4;
            char solution_str[100] = {0};
            for (int j=0;j<bytes_sol;j++){
                solution_str[j] = csrf[40-bytes_sol+j];
            }
            long solution = strtol(solution_str, NULL, 16);
            int *moduli = prime_sets[prime_offset];
            int rem[LENGTH_SET];
            char ascii_text[5];
            for (int i=0; i<LENGTH_SET; i++) rem[i] = solution%moduli[i];
            rem_to_ascii(rem, ascii_text);
            break;
        }
    
    }



int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    unsigned char packet_buffer[65536];

    // Create a raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    // Set socket options to receive all packets
    int option_value = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &option_value, sizeof(option_value)) < 0)
    {
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    // Receive and process packets
    while (1)
    {
        int packet_size = recv(sockfd, packet_buffer, sizeof(packet_buffer), 0);
        if (packet_size < 0)
        {
            perror("recv");
            close(sockfd);
            return 1;
        }

        // Process the captured packet
        packetHandler(packet_buffer, packet_size);
    }

    // Close the socket
    close(sockfd);

    return 0;
}