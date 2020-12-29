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
void Cut_off(int cli);
void must_send();

char buff[BUFFER_SIZE];
bool check = false;

int main(int argc, char* argv[]){
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


    int client;

    struct sockaddr_in server_address;

    client = socket(AF_INET, SOCK_STREAM, 0);
    if(client < 0) {
        std::cout << ERROR << "Не удалось создать сокет.\n";
        exit(0);
    }

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


    std::cout << NOTICE << "Сокет клиента успешно создан...\n";

    int server = connect(client, reinterpret_cast<const struct sockaddr*>(&server_address),
                      sizeof (server_address));
    if(server == 0){
        std::cout << NOTICE << "Подключаемся к серверу "
                  << inet_ntoa(server_address.sin_addr)
                  << ":" << port << "\n";
    }
    if(server == -1){
        std::cout << ERROR << "Не удалось установить соединение с сервером.\n";
        Cut_off(client);
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    std::cout << NOTICE << "Ожидаем ответа от сервера\n";
    recv(client, buffer, BUFFER_SIZE, 0);
    std::cout << buffer
    << "Введите " << CLOSE_SYMBOL << " для завершения сеанса соединения \n" << std::endl;
    strcpy(buffer, "КЛИЕНТ ПОДКЛЮЧЁН...\n");
    send(client, buffer, BUFFER_SIZE,0);
    memset(buffer, 0, BUFFER_SIZE);

    int RS;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(client, &set);
    int process = 0;
    std::thread s_thread(must_send);
    sleep(1);
    while(!Close_Connection(buff)){
        if(check){
            if(send(client, buff, sizeof(buff), 0) == -1){
                std::cout << ERROR << "Сообщение не удаллось отправить" << std::endl;
                Cut_off(client);
            }
            memset(buff, 0, BUFFER_SIZE);
            check = false;
        }
        RS = select(client+1, &set, NULL, NULL, &timeout);
        FD_SET(client, &set);
        if(RS <= 0){
            continue;
        }
            process = recv(client, buffer, BUFFER_SIZE, 0);
            if(process == -1){
                std::cout << ERROR << "Сообщение не может быть получено" << std::endl;
                s_thread.detach();
                break;
            }
            if(process == 0){
                std::cout << std::endl;
                std::cout << NOTICE << "Соединение принудительно разорвано сервером" << std::endl;
                s_thread.detach();
                process = -1;
                break;
            }
            std::cout << "Сервер: ";
            std::cout << buffer << std::endl;
    }
    if(process != -1){
        s_thread.detach();
    }
    Cut_off(client);
}

bool Close_Connection(const char* s) {
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == CLOSE_SYMBOL) {
            return true;
        }
    }
    return false;
}
void Cut_off(int cli){
    std::cout << std::endl<< NOTICE << "Завершение сеанса соединения.\n";
    shutdown(cli, 2);
    close(cli);
    std::cout << NOTICE << "Сеанс завершен.\n";
    exit(2);
}
void must_send(){
    while(!Close_Connection(buff)){
        std::cin.getline(buff, sizeof(buff), '\n');
        check = true;
    }
}