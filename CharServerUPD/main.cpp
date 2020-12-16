#include <iostream>
#include "cstdlib"
#include "unistd.h"
#include "sys/socket.h"
#include <cstring>
#include "netinet/in.h"

#define ERROR "ОШИБКА СЕРВЕРА: "
#define NOTICE "СЕРВЕР: "
#define BUFFER_SIZE 100
#define CLOSE_SYMBOL '#'
#define EXTRA_PORT 8000


bool Close_Connection(const char* s);
void Cut_off(int* p, int serv);

int main(int argc, char* argv[]) {
    int port_val = EXTRA_PORT;
    int* port = &port_val;
    try{
        if(argc == 2) {
            port = reinterpret_cast<int* >(argv[1]);
        }
        else {
            throw 1;
        }
    }
    catch (int){
        std::cout << ERROR << "Пользовательские данные о порте не обнаружены.\n" << "Установленно значение по умолчанию: " << EXTRA_PORT
                  << std::endl;
    }

    int sock;
    struct sockaddr_in server_address, client_address;
    socklen_t size = sizeof(server_address);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }

    server_address.sin_port = htons(*port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(sock, reinterpret_cast<struct sockaddr*>(&server_address),
            size) < 0) {
        std::cout << ERROR
                << "Сокет который вы пытетесь открыть уже занят.\n";
        return -1;
    }

    int size_client = sizeof(client_address);

    std::cout << NOTICE <<"Сокет сервера успешно создан...\n";

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(sock, &set);
    timeout.tv_sec = static_cast<__time_t>(0.01);
    timeout.tv_usec = 0;
    char buffer[BUFFER_SIZE];
    std::cout << "Введите " << CLOSE_SYMBOL <<" для завершения сеанса соединения\n" << std::endl;

    while(!Close_Connection(buffer)) {

        std::cout << "Сервер: ";
        std::cin.getline(buffer, BUFFER_SIZE);
        sendto(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&client_address),size_client);
        RS = select(sock+1, &set, NULL, NULL, &timeout);
        FD_SET(sock, &set);
        if(RS <= 0){
            continue;
        }
        else {
            while(RS>0){
                recvfrom(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&client_address),
                         reinterpret_cast<socklen_t *>(&size_client));
                std::cout << "Клиент: ";
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
    for(int i = 0; i < strlen(s); i++) {
        if(s[i] == CLOSE_SYMBOL) {
            return true;
        }
    }
    return false;
}
void Cut_off(int* p, int serv){
    shutdown(serv, 2);
    std::cout << std::endl << NOTICE <<"Соединение разорвано\n";
    close(serv);
    std::cout << "Порт " << *p << " снова доступен\n";
    exit(2);
}