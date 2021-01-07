#include <iostream>
#include "cstdlib"
#include "unistd.h"
#include "sys/socket.h"
#include <cstring>
#include "netinet/in.h"
#include <sstream>
#include "thread"

#define BUFFER_SIZE 100
#define CLOSE_SYMBOL '#'
#define NOTICE "СЕРВЕР: "
#define ERROR "ОШИБКА СЕРВЕРА: "
#define EXTRA_PORT 8000


bool Close_Connection(const char* s);
void Cut_off(int p, int serv, sockaddr_in client_address);
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

    int sock;
    struct sockaddr_in server_address, client_address;
    socklen_t size = sizeof(server_address);
    int size_client = sizeof(client_address);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }

    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htons(INADDR_ANY);

    if(bind(sock, reinterpret_cast<struct sockaddr*>(&server_address),
            size) < 0) {
        std::cout << ERROR
                << "Сокет который вы пытетесь открыть уже занят.\n";
        return -1;
    }

    std::cout << NOTICE <<"Сокет сервера успешно создан...\n";

    int RS;
    fd_set set;
    struct timeval timeout;
    FD_ZERO(&set);
    FD_SET(sock, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int process = 0;
    std::thread s_thread(must_send);
    sleep(1);
    if(recvfrom(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&client_address),
                reinterpret_cast<socklen_t *>(&size_client)) == -1){
        std::cout << ERROR << "Клиенту не удалось присоединиться" << std::endl;
        Cut_off(port, sock, client_address);
    }
    memset(buffer, 0, BUFFER_SIZE);
    std::cout << NOTICE << "Клиент подключён" << std::endl;
    std::cout << "Введите " << CLOSE_SYMBOL <<" для завершения сеанса соединения\n" << std::endl;


    while(!Close_Connection(buffer) && !Close_Connection(buff)){
        if(check){
            sendto(sock, buff, sizeof(buff), 0, reinterpret_cast<struct sockaddr*>(&client_address),size_client);
            memset(buff, 0, BUFFER_SIZE);
            check = false;
        }
        RS = select(sock+1, &set, NULL, NULL, &timeout);
        FD_SET(sock, &set);
        if(RS <= 0){
            continue;
        }
        process = recvfrom(sock, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&client_address),
                           reinterpret_cast<socklen_t *>(&size_client));
        if(process == -1){
            std::cout << ERROR << "Сообщение не может быть прочитано" << std::endl;
            s_thread.detach();
            break;
        }
        std::cout << "Клиент: ";
        std::cout << buffer<< std::endl;
    }
    s_thread.detach();
    Cut_off(port, sock, client_address);
}

bool Close_Connection(const char* s) {
    for(int i = 0; i < strlen(s); i++) {
        if(s[i] == CLOSE_SYMBOL) {
            return true;
        }
    }
    return false;
}
void Cut_off(int p, int serv, sockaddr_in client_address){
    int size_client = sizeof(client_address);
    sendto(serv, "#", sizeof("#"), 0, reinterpret_cast<struct sockaddr*>(&client_address),size_client);
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
