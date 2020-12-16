#include <iostream>
#include "cstdlib"
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include <cstring>
#include "netinet/in.h"

#define LOCAL_HOST "127.0.0.1"
#define BUFFER_SIZE 100
#define CLOSE_SYMBOL '#'
#define NOTICE "КЛИЕНТ: "
#define ERROR "ОШИБКА КЛИЕНТА: "
#define EXTRA_PORT 8000

bool Close_Connection(const char* s);
void Cut_off(int* p, int cli);

int main(int argc, char* argv[]) {
    const char *IP = LOCAL_HOST;
    int port_1 = EXTRA_PORT;
    int *port = &port_1;
    try {
        if (argc > 2) {
           *port = (atoi(argv[argc]));
            IP = reinterpret_cast<const char * >(argv[argc]);
        } else {
            throw 1;
        }
    }
    catch (int) {
        std::cout << ERROR << "Пользовательские данные о порте не обнаружены.\n"
                  << "Установленно значение по умолчанию: " << EXTRA_PORT
                  << std::endl;
    }
    int sock;

    struct sockaddr_in server_address, client_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }
    int size = sizeof(server_address);
    int size_client = sizeof(client_address);
    server_address.sin_port = htons(*port);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, IP, &server_address.sin_addr);
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(0);
    client_address.sin_addr.s_addr = htons(INADDR_ANY);

    bind(sock, reinterpret_cast<struct sockaddr*>(&client_address), size_client);

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(sock, &set);
    timeout.tv_sec = static_cast<__time_t>(0.01);
    timeout.tv_usec = 0;
        char buffer[BUFFER_SIZE];
        std::cout << "Введите " << CLOSE_SYMBOL << " для завершения сеанса соединения\n" << std::endl;

        while (!Close_Connection(buffer)) {
            std::cout << "Клиент: ";
            std::cin.getline(buffer, BUFFER_SIZE);
            sendto(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&server_address), size);
        RS = select(sock+1, &set, NULL, NULL, &timeout);
        FD_SET(sock, &set);
        if(RS <= 0){
            continue;
        }
        else {
            while(RS>0){
            recvfrom(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&server_address),
                     reinterpret_cast<socklen_t *>(&size));
            std::cout << "Сервер: ";
            std::cout << buffer << std::endl;
                RS = select(sock+1, &set, NULL, NULL, &timeout);
                if(Close_Connection(buffer))
                    break;
            }
        }
        }
        Cut_off(port, sock);
}


bool Close_Connection(const char* s) {
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == CLOSE_SYMBOL) {
            return true;
        }
    }
    return false;
}
void Cut_off(int* p, int cli){
    std::cout << std::endl<< NOTICE << "Завершение сеанса соединения. Закрываю сокет\n";
    shutdown(cli, 2);
    close(cli);
    std::cout << NOTICE << "Сокет закрыт.\n";
    exit(2);
}
