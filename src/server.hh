#ifndef SERVER_HH
#define SERVER_HH

#include "socket.hh"
#include <string.h>

#define MAX_LENGTH 1024

class Server{
public:
    Server();
    Server(int listen_num);
    ~Server();
    void run(void (*dataProcess)(char*));
    void serve(int client_fd, struct sockaddr_in client_addr, void (*dataProcess)(char*));
private:
    Socket::Socket listen_socket;
    char buff[MAX_LENGTH];
    int listen_num;
};

Server::Server(){ Server(1); }

Server::Server(int listen_num){
    this->listen_num = listen_num;
    memset(this->buff, 0, MAX_LENGTH);
}

Server::~Server(){
    shutdown(this->listen_socket, SHUT_RDWR);
}

void Server::run(void (*dataProcess)(char*)){
    this->listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(this->listen_socket == -1){   
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8765);
    if((bind(this->listen_socket, (sockaddr*)&address, sizeof(struct sockaddr_in))) == -1){
        exit(1);
    }

    cout << "开始监听任务..." << endl;
    if(listen(this->listen_socket, 5) != 0){
        perror("listen");
        exit(1);
    }

    struct sockaddr_in client_addr;
    int size = sizeof(sockaddr_in);
    
    // * accept 会阻塞线程等待连接, 当 accept 收到连接请求, 获得到 client_fd时 才与client建立了连接
    int client_fd = accept(this->listen_socket, (struct sockaddr*)&client_addr, (socklen_t*)&size);

    char client_ip[16];
    inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), client_ip, 16);
    //将网络字节序的端口转到本机字节序
    uint16_t client_port = ntohs(client_addr.sin_port);
    
    // *尝试加一个握手协议吧,你可以的!
    // string sync = "SYN = 101";
    // write(client_fd, sync.c_str(), sync.size());
    

    cout << endl << "与 client [" << client_fd << "-> "<< client_ip <<":" << client_port << "] 成功建立连接, 开始服务..." << endl;
    serve(client_fd, client_addr, dataProcess);
    cout << endl <<"监听结束..." << endl;
}

void Server::serve(int client_fd, struct sockaddr_in client_addr, void (*dataProcess)(char*)){
    int size = sizeof(struct sockaddr_in);
    while(true){
        int num = read(client_fd, this->buff, sizeof(this->buff));
        if(num < 0){
            ::close(client_fd);
            perror("read error");
            exit(1);
        }
        if(num == 0 || strcmp(this->buff, "exit") == 0){
            std::cout << endl << "Client 申请断开连接" << std::endl;
            string tmp = "exit";
            write(client_fd, tmp.c_str(), sizeof(tmp));
            sleep(1);
            break;
        }
        string client_data = this->buff;
        dataProcess(this->buff);
        sprintf(this->buff, "You send [ %s ]\n", client_data.c_str());
        write(client_fd, this->buff, sizeof(this->buff));
    }
    ::close(client_fd);
}

#endif // SERVER_HH