#ifndef CLIENT_HH
#define CLIENT_HH

#include "socket.hh"
#include <string.h>

#define MAX_LENGTH 1024

class Client{
public:
    Client();
    ~Client();
    void setBuffer(string str);
    void run(Socket::Address address);
    void connect(Socket::Address address);
    void send();
    void receive();
private:
    Socket::Socket conn_socket;
    char* buff;
};

Client::Client(){
    this->buff = (char*)malloc(sizeof(char) * MAX_LENGTH);
    this->conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(this->conn_socket == -1){
        exit(1);
    }
}

Client::~Client(){
    free(this->buff);
    shutdown(this->conn_socket, SHUT_RDWR);
}

void Client::setBuffer(string str){
    strcpy(this->buff, str.c_str());
}

void Client::run(Socket::Address address){
    connect(address);
    while(true){
        cout << "请输入Message: ";
        cin >> this->buff;

        send();

        // 睡眠 10ms
        usleep(10000);

        receive();
        if(strcmp(this->buff, "exit") == 0){
            cout << endl << "断开连接..." << endl;
            break;
        }
    }
    ::close(this->conn_socket);
}

void Client::connect(Socket::Address address){
    sockaddr_in* sock_addr = switch_to_sockaddr(address);
    if(::connect(this->conn_socket, (sockaddr*)sock_addr, sizeof(sockaddr_in)) == -1){
        perror("cannot connect");
        exit(1);
    }
    cout << "已建立连接..." << endl << endl;
}

void Client::send(){
    write(this->conn_socket, this->buff, sizeof(this->buff));
}

void Client::receive(){
    read(this->conn_socket, this->buff, MAX_LENGTH);
    cout << endl <<"Server: " << this->buff << "" << endl;
}

#endif // CLIENT_HH