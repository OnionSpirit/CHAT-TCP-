#include <iostream>
#include "cstdlib"
#include "unistd.h"
#include "sys/socket.h"
#include <cstring>
#include "netinet/in.h"
#include <sstream>
#include <thread>
#include "pthread.h"

#define ERROR "ОШИБКА СЕРВЕРА: "
#define NOTICE "СЕРВЕР: "
#define BUFFER_SIZE 100
#define CLOSE_SYMBOL '#'
#define EXTRA_PORT 8000


bool Close_Connection(const char* s);
void Cut_off(int p, int serv);
void must_send();

char buff[BUFFER_SIZE];
bool check = false;

int main(int argc, char* argv[]) {
    memset(buff, 0, BUFFER_SIZE);
    int port = 0;
    if(argc > 1){
        std::stringstream convert(argv[1]);
        if (!(convert >> port)) {
            port = EXTRA_PORT;
            std::cout << ERROR << "Пользовательские данные о порте не обнаружены.\n"
                      << "\tУстановленно значение по умолчанию: " << port << std::endl;
        }
        else {
            std::cout << NOTICE << "Пользовательское значение порта установленно. Значение порта: " << port <<  std::endl;
        }
    }
    else{
        port = EXTRA_PORT;
        std::cout << ERROR << "Пользовательские данные о порте не обнаружены.\n"
                  << "\tУстановленно значение по умолчанию: " << port << std::endl;
    }

    int client;
    int server;
    struct sockaddr_in server_address{};

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
       std::cout << ERROR << "Не удалось создать сокет.\n";
       exit(0);
   }

    server_address.sin_port = htons(static_cast<int>(port));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(client, reinterpret_cast<struct sockaddr*>(&server_address),
            sizeof(server_address)) == -1) {
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

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    std::cout << NOTICE<< "Ожидаем ответа от клиента...\n";
    strcpy(buffer, "СЕРВЕР ПОДКЛЮЧЁН...\n");
    send(server, buffer, BUFFER_SIZE, 0);
    recv(server, buffer, BUFFER_SIZE, 0);
    std::cout << buffer
    << "Введите " << CLOSE_SYMBOL <<" для завершения сеанса соединения\n" << std::endl;
    memset(buffer, 0, BUFFER_SIZE);

    int RS;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(server, &set);
    int process = 0;
    std::thread s_thread(must_send);
    sleep(1);
    while(!Close_Connection(buff)){
        if(check){
            if(send(server, buff, sizeof(buff), 0) == -1){
                std::cout << ERROR << "Сообщение не удаллось отправить" << std::endl;
                Cut_off(port, server);
            }
            memset(buff, 0, BUFFER_SIZE);
            check = false;
        }
        RS = select(server+1, &set, NULL, NULL, &timeout);
        FD_SET(server, &set);
        if(RS <= 0){
            continue;
        }
            process = recv(server, buffer, BUFFER_SIZE, 0);
            if(process == -1){
               std::cout << ERROR << "Сообщение не может быть получено" << std::endl;
               s_thread.detach();
               break;
            }
            if(process == 0){
                std::cout << std::endl;
                std::cout << NOTICE << "Соединение принудительно разорвано клиентом" << std::endl;
                s_thread.detach();
                process = -1;
                break;
            }
            std::cout << "Клиент: ";
            std::cout << buffer << std::endl;
    }
    if(process != -1){
        s_thread.detach();
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
void Cut_off(int p, int serv){
    shutdown(serv, 2);
    std::cout << std::endl << NOTICE <<"Сеанс соединения окончен\n";
    close(serv);
    std::cout << "Порт " << p << " снова доступен\n";
    exit(2);
}
void must_send(){
    while(!Close_Connection(buff)){
        std::cin.getline(buff, sizeof(buff), '\n');
        check = true;
    }
}