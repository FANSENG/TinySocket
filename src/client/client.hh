#ifndef CLIENT_HH
#define CLIENT_HH

#include "../utils/socket.hh"
#include "../utils/utils.hh"
#include <string.h>
#include <cstdlib>

#define MAX_LENGTH 1024

class Client{
public:
    Client();
    ~Client();
    bool closeConnect();
    void setBuffer(string str);
    string getBuffer();
    void connect(Socket::Address address);
    void send();
    void receive();
private:
    int handShake();
    Socket::Socket conn_socket;
    char* buff;
};

Client::Client(){
    this->buff = (char*)malloc(sizeof(char) * MAX_LENGTH);
    this->conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->conn_socket == -1){
        exit(1);
    }
}

Client::~Client(){
    free(this->buff);
    shutdown(this->conn_socket, SHUT_RDWR);
}

bool Client::closeConnect(){
    ::close(this->conn_socket);
}

void Client::setBuffer(string str){
    if(str.size() > MAX_LENGTH){
        cout << "数据超过最大长度(" << MAX_LENGTH << ")! 设置失败" << endl;
        return;
    }
    strcpy(this->buff, str.c_str());
}

string Client::getBuffer(){
    return this->buff;
}

int Client::handShake(){
    // SYN.ACK.seq.ack
    int ACK = 0, ack = 0, SYN = 1, seq = rand() % 100;
    string message = to_string(SYN) + "." + to_string(ACK) + "." + to_string(seq) + "." + to_string(ack) + '\0';
    char recv_message[MAX_LENGTH];
    cout << "【TCP三次握手 Client】: 发送消息1[" << message << "]" << endl << endl;
    write(this->conn_socket, message.c_str(), message.size());

    read(this->conn_socket, recv_message, MAX_LENGTH);
    handShakeSetParm(message, SYN, ACK, seq, ack);
    cout << "【TCP三次握手 Client】: 接受消息2[" << recv_message << "]" << endl << endl;

    int tmp = ack; ACK = 1; SYN = 0; ack = seq + 1; seq = tmp;
    message = to_string(SYN) + "." + to_string(ACK) + "." + to_string(seq) + "." + to_string(ack) + '\0';
    cout << "【TCP三次握手 Client】: 发送消息3[" << message << "]" << endl << endl;
    write(this->conn_socket, message.c_str(), message.size());
    return 1;
}

void Client::connect(Socket::Address address){
    sockaddr_in* sock_addr = switch_to_sockaddr(address);
    // 这里完成了三次握手
    if(::connect(this->conn_socket, (sockaddr*)sock_addr, sizeof(sockaddr_in)) == -1){
        perror("cannot connect");
        exit(1);
    }
    if(handShake() < 0){
        cout << "握手失败" << endl << endl;
        exit(1);
    }
    cout << "已建立连接..." << endl << endl;
}

void Client::send(){
    write(this->conn_socket, this->buff, sizeof(this->buff));
}

void Client::receive(){
    read(this->conn_socket, this->buff, MAX_LENGTH);
}

#endif // CLIENT_HH