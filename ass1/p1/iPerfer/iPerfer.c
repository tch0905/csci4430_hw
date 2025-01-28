#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#define CHUNK_SIZE 1000
#define FIN_SIGNAL 0xFFFFFFFF
#define ACK_MSG "ACK"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void server_mode(int port) {
//    if (port < 1024 || port > 65535) {
//        fprintf(stderr, "Error: port number must be in the range [1024, 65535]\n");
//        exit(1);
//    }
//
//    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0) error("ERROR opening socket");
//
//    int opt = 1;
//    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//
//    struct sockaddr_in serv_addr;
//    memset(&serv_addr, 0, sizeof(serv_addr));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
//    serv_addr.sin_port = htons(port);
//
//    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//        error("ERROR on binding");
//
//    listen(sockfd, 5);
//    struct sockaddr_in cli_addr;
//    socklen_t clilen = sizeof(cli_addr);
//    int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
//    if (newsockfd < 0) error("ERROR on accept");
//
//    char buffer[CHUNK_SIZE];
//    size_t total_bytes = 0;
//    struct timeval start_time, end_time;
//    int fin_received = 0;
//    int first_byte = 1;
//
//    while (1) {
//        ssize_t n = recv(newsockfd, buffer, sizeof(buffer), 0);
//        if (n <= 0) break;
//
//        if (first_byte) {
//            gettimeofday(&start_time, NULL);
//            first_byte = 0;
//        }
//
//        // Check for FIN signal (4-byte 0xFFFFFFFF)
//        if (n == sizeof(uint32_t)) {
//            uint32_t signal;
//            memcpy(&signal, buffer, sizeof(uint32_t));
//            if (signal == FIN_SIGNAL) {
//                fin_received = 1;
//                send(newsockfd, ACK_MSG, strlen(ACK_MSG), 0);
//                gettimeofday(&end_time, NULL);
//                break;
//            }
//        }
//
//        total_bytes += n;
//    }
//
//    if (!fin_received) gettimeofday(&end_time, NULL);
//
//    double elapsed = (end_time.tv_sec - start_time.tv_sec) +
//                     (end_time.tv_usec - start_time.tv_usec) / 1e6;
//    double rate = (total_bytes * 8) / (elapsed * 1e6); // Mbps
//    printf("Received=%zu KB, Rate=%.3f Mbps\n", total_bytes / 1024, rate);
//
//    close(newsockfd);
//    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 4 || strcmp(argv[1], "-s") != 0 || strcmp(argv[2], "-p") != 0) {
        fprintf(stderr, "Error: missing or extra arguments\n");
        exit(1);
    }

    int port = atoi(argv[3]);
    server_mode(port);
    return 0;
}