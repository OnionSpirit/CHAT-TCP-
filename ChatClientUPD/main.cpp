#include <iostream>
#include "cstdlib"
#include "unistd.h"
#include "arpa/inet.h"
#include "sys/socket.h"
#include <cstring>
#include "netinet/in.h"
#include <sstream>
#include "thread"

#define LOCAL_HOST "127.0.0.1"
#define BUFFER_SIZE 100
#define CLOSE_SYMBOL '#'
#define NOTICE "КЛИЕНТ: "
#define ERROR "ОШИБКА КЛИЕНТА: "
#define EXTRA_PORT 8000

bool Close_Connection(const char* s);
void Cut_off(int cli, sockaddr_in server_address);
void must_send();

char buff[BUFFER_SIZE];
bool check = false;

int main(int argc, char* argv[]) {
    memset(buff, 0, BUFFER_SIZE);
    const char* IP;
    int port;
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
    int sock;

    struct sockaddr_in server_address, client_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }
    int size = sizeof(server_address);
    int size_client = sizeof(client_address);
    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    if(!(IP = argv[2])){
        IP = LOCAL_HOST;
        std::cout << ERROR << "Пользовательские данные об IP не обнаружены.\n"
                  << "\tУстановленно значение по умолчанию: " << IP << std::endl;
        inet_pton(AF_INET, IP, &server_address.sin_addr);
    }
    else {
        if (!inet_pton(AF_INET, IP, &server_address.sin_addr)) {
            IP = LOCAL_HOST;
            std::cout << ERROR << "Данные об IP введены некорректно.\n"
                                  "\tУстановленно значение по умолчанию: " << IP << std::endl;
            inet_pton(AF_INET, IP, &server_address.sin_addr);
        } else {
            std::cout << NOTICE << "Пользовательское значение IP установленно. Значение IP: " << IP << std::endl;
        }
    }
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(0);
    client_address.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(sock, reinterpret_cast<struct sockaddr*>(&client_address), size_client) < 0){
        std::cout << ERROR
                  << "Сокет который вы пытетесь открыть уже занят.\n";
        return -1;
    }

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(sock, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int process = 0;
    std::thread s_thread(must_send);
    sleep(1);
    if(sendto(sock, "1", sizeof("1"), 0, reinterpret_cast<struct sockaddr*>(&server_address), size) == -1){
        std::cout << ERROR << "Не удалось отправить запрос серверу" << std::endl;
        Cut_off(sock, server_address);
    }
    std::cout << NOTICE << "Подключён к серверу" << std::endl;
    std::cout << "Введите " << CLOSE_SYMBOL << " для завершения сеанса соединения\n" << std::endl;



    while(!Close_Connection(buffer) && !Close_Connection(buff)){
        if(check){
            sendto(sock, buff, sizeof(buff), 0, reinterpret_cast<struct sockaddr*>(&server_address), size);
            memset(buff, 0, BUFFER_SIZE);
            check = false;
        }
        RS = select(sock+1, &set, NULL, NULL, &timeout);
        FD_SET(sock, &set);
        if(RS <= 0){
            continue;
        }
        process = recvfrom(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&server_address),
                           reinterpret_cast<socklen_t *>(&size));
        if(process == -1){
            std::cout << ERROR << "Сообщение не может быть прочитано" << std::endl;
            s_thread.detach();
            break;
        }
        std::cout << "Сервер: ";
        std::cout << buffer<< std::endl;
    }
    s_thread.detach();
    Cut_off(sock, server_address);
}


bool Close_Connection(const char* s) {
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == CLOSE_SYMBOL) {
            return true;
        }
    }
    return false;
}
void Cut_off(int cli, sockaddr_in server_address){
    int size = sizeof(server_address);
    sendto(cli, "#", sizeof("#"), 0, reinterpret_cast<struct sockaddr*>(&server_address), size);
    std::cout << std::endl<< NOTICE << "Завершение сеанса соединения. Закрываю сокет\n";
    shutdown(cli, 2);
    close(cli);
    std::cout << NOTICE << "Сокет закрыт.\n";
    exit(2);
}
void must_send(){
    while(!Close_Connection(buff)){
        std::cin.getline(buff, sizeof(buff), '\n');
        check = true;
    }
}
//бинды проверь
