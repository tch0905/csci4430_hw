#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctime>
#include <vector>

void run_server(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (sockfd < 0) {
        std::cout << "Error opening socket" << std::endl;
        exit(1);
    }

    int opt = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cout << "Error setting socket options" << std::endl;
        close(sockfd);
        exit(1);
    }


    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Error binding socket" << std::endl;
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 1) < 0) {
        std::cout << "Error listening on socket" << std::endl;
        close(sockfd);
        exit(1);
    }

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock < 0) {
        std::cout << "Error accepting connection" << std::endl;
        close(sockfd);
        exit(1);
    }

    clock_t start_time = clock();

    std::vector<char> buffer;
    const char fin[] = {0x01, 0x02, 0x03, 0x04};
    long total_bytes = 0;
    bool fin_received = false;

    while (!fin_received) {
        char temp_buf[10000];
        int bytes_read = recv(client_sock, temp_buf, sizeof(temp_buf), 0);
        if (bytes_read < 0) {
            std::cout << "Error reading from socket" << std::endl;
            break;
        } else if (bytes_read == 0) {
            break;
        }

        buffer.insert(buffer.end(), temp_buf, temp_buf + bytes_read);

        if (buffer.size() >= 4) {
            for (size_t i = 0; i <= buffer.size() - 4; ++i) {
                if (memcmp(&buffer[i], fin, 4) == 0) {
                    total_bytes += i;
                    const char ack[] = {0x05, 0x05, 0x05, 0x05};
                    send(client_sock, ack, 4, 0);
                    fin_received = true;
                    buffer.clear();
                    break;
                }
            }
            if (fin_received) {
                break;
            }
        }
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    double total_kb = total_bytes / 1000.0;
    double rate = (total_bytes * 8) / time_taken;

    printf("Received=%.0f KB, Rate=%.3f Mbps\n", total_kb, rate);




    close(client_sock);
    close(sockfd);
}

void run_client(const char* hostname, int port, int time_sec, clock_t start_time) {
    struct hostent* server = gethostbyname(hostname);
    if (!server) {
        std::cout << "Error resolving hostname" << std::endl;
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Error opening socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cout << "Error connecting to server" << std::endl;
        close(sockfd);
        exit(1);
    }

    char data[10000];
    memset(data, 0, sizeof(data));

    long bytes_sent = 0;

    bool sending = true;
    while (sending) {
        clock_t current_time = clock();
        double elapsed =  (double)(current_time - start_time) / 1000;
//        std::cout << "Elapsed time: " << elapsed << " seconds" << std::endl;
        if (elapsed >=  (double)time_sec) {
            sending = false;
            break;
        }

        int sent = send(sockfd, data, sizeof(data), 0);
        if (sent < 0) {
            std::cout << "Error sending data" << std::endl;
            break;
        }
        bytes_sent += sent;
    }

    const char fin[] = {0x01, 0x02, 0x03, 0x04};
    send(sockfd, fin, 4, 0);

    char ack[4];
    recv(sockfd, ack, 4, 0);

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / 1000;
    double total_kb = bytes_sent / 1000.0;
    double rate = (bytes_sent * 8) / time_taken;

    printf("Sent=%.0f KB, Rate=%.3f Mbps\n", total_kb, rate);




    close(sockfd);
}

int main(int argc, char* argv[]) {
    // Printing all the arguments passed to the program
//    std::cout << "Total arguments: " << argc << std::endl;
//    for (int i = 0; i < argc; ++i) {
//        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
//    }
//
//
    // handle args
    if (!(argc == 4 || argc == 8)) {
        std::cout << "Error: missing or extra arguments" << std::endl;
        return 1;
    }

    // server mode
    if (strcmp(argv[1], "-s") == 0) {

        if (argc != 4 || strcmp(argv[2], "-p") != 0) {
            std::cout << "Error: missing or extra arguments" << std::endl;
            return 1;
        }

        int port = atoi(argv[3]);

        if (port < 1024 || port > 65535) {
            std::cout << "Error: port number must be in the range of [1024, 65535]" << std::endl;
            return 1;
        }
        run_server(port);
    }
    // client mode
    else if (strcmp(argv[1], "-c") == 0) {
        if (argc != 8 || strcmp(argv[2], "-h") != 0 || strcmp(argv[4], "-p") != 0 || strcmp(argv[6], "-t") != 0) {
            std::cout << "Error: missing or extra arguments" << std::endl;
            return 1;
        }
        const char* hostname = argv[3];
        int port = atoi(argv[5]);
        if (port < 1024 || port > 65535) {
            std::cout << "Error: port number must be in the range of [1024, 65535]" << std::endl;
            return 1;
        }
        int time = atoi(argv[7]);
        if (time <= 0) {
            std::cout << "Error: time argument must be greater than 0" << std::endl;
            return 1;
        }
        clock_t start_time = clock();
        run_client(hostname, port, time, start_time);
    } else {
        std::cout << "Error: invalid mode" << std::endl;
        return 1;
    }

    return 0;
}