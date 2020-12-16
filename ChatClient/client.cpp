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

int main(int argc, char* argv[]){
    const char* IP = LOCAL_HOST;
    int port_1 = EXTRA_PORT;
    int* port = &port_1;
    try{
        if(argc > 2) {
            port = reinterpret_cast<int* >(argv[2]);
            IP = reinterpret_cast<const char* >(argv[3]);
        }
        else {
            throw 1;
        }
    }
    catch (int){
        std::cout << ERROR << "Пользовательские данные о порте не обнаружены.\n" << "Установленно значение по умолчанию: " << EXTRA_PORT
                  << std::endl;
    }
    int client;

    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }

    server_address.sin_port = htons(*port);
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, IP, &server_address.sin_addr);

    std::cout << NOTICE << "Сокет клиента успешно создан...\n";

    int ret = connect(client, reinterpret_cast<const struct sockaddr*>(&server_address),
                      sizeof (server_address));
    if(ret == 0){
        std::cout << NOTICE << "Подключаемся к серверу "
                  << inet_ntoa(server_address.sin_addr)
                  << " Порт: " << *port << "\n";
    }
    if(ret == -1){
        std::cout << ERROR << "Не удалось установить соединение с сервером.\n";
        Cut_off(port, client);
    }

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(client, &set);
    timeout.tv_sec = static_cast<__time_t>(0.01);
    timeout.tv_usec = 0;
    char buffer[BUFFER_SIZE];
    std::cout << NOTICE << "Ожидаем ответа от сервера\n";
    recv(client, buffer, BUFFER_SIZE, 0);
    std::cout << buffer
    << "Введите " << CLOSE_SYMBOL << " для завершения сеанса соединения \n" << std::endl;
    strcpy(buffer, "КЛИЕНТ ПОДКЛЮЧЁН...\n");
    send(client, buffer, BUFFER_SIZE,0);

    while(!Close_Connection(buffer)){
        std::cout << "Клиент: ";
        std::cin.getline(buffer, BUFFER_SIZE);
        send(client, buffer, BUFFER_SIZE, 0);
        RS = select(client+1, &set, NULL, NULL, &timeout);
        FD_SET(client, &set);
        if(RS <= 0){
            continue;
        }
        else {
            while(RS>0){
                recv(client, buffer, BUFFER_SIZE, 0);
                std::cout << "Сервер: ";
                std::cout << buffer << std::endl;
                RS = select(client+1, &set, NULL, NULL, &timeout);
                if(Close_Connection(buffer))
                    break;
            }
        }
    }
    Cut_off(port, client);
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
