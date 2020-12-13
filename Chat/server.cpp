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
        if(argc == 1) {
            port = reinterpret_cast<int* >(argv[argc]);
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
    int server;
    struct sockaddr_in server_address{};

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
       std::cout << ERROR << "Не удалось создать сокет.\n";
       exit(0);
   }

    server_address.sin_port = htons(*port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    int ret = bind(client, reinterpret_cast<struct sockaddr*>(&server_address),
           sizeof(server_address));

    if(ret == -1) {
       std::cout
       << ERROR
       << "Сокет который вы пытетесь открыть уже занят.\n";
       return -1;
    }

    std::cout << NOTICE <<"Сокет сервера успешно создан...\n";

    socklen_t size = sizeof(server_address);
    std::cout << NOTICE << "Ожидаем подключения клиента...\n";
    listen(client, 1);

    server = accept(client, reinterpret_cast<struct sockaddr*>(&server_address), &size);
    if (server < 0) {
       std::cout << ERROR << "Не удалось установить связь с клиентом.\n";
        Cut_off(port, server);
    }

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(server, &set);
    timeout.tv_sec = static_cast<__time_t>(0.01);
    timeout.tv_usec = 0;
    char buffer[BUFFER_SIZE];
    std::cout << NOTICE<< "Ожидаем ответа от клиента...\n";
    strcpy(buffer, "СЕРВЕР ПОДКЛЮЧЁН...\n");
    send(server, buffer, BUFFER_SIZE, 0);
    recv(server, buffer, BUFFER_SIZE, 0);
    std::cout << buffer
    << "Введите " << CLOSE_SYMBOL <<" для завершения сеанса соединения\n" << std::endl;

       while(!Close_Connection(buffer)) {
           std::cout << "Сервер: ";
           std::cin.getline(buffer, BUFFER_SIZE);
           send(server, buffer, BUFFER_SIZE, 0);
           RS = select(server+1, &set, NULL, NULL, &timeout);
           FD_SET(server, &set);
           if(RS <= 0){
               continue;
           }
           else {
               while(RS>0){
                   recv(server, buffer, BUFFER_SIZE, 0);
                   std::cout << "Клиент: ";
                   std::cout << buffer << std::endl;
                   RS = select(server+1, &set, NULL, NULL, &timeout);
                   if(Close_Connection(buffer))
                       break;
               }
           }
       }
        Cut_off(port, server);
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
