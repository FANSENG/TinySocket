#ifndef SERVER_HH
#define SERVER_HH

#include "../utils/socket.hh"
#include "../utils/utils.hh"
#include <string.h>

#define MAX_LENGTH 1024

class Server{
public:
    Server();
    Server(int listen_num);
    Server(int listen_num, int max_client);
    ~Server();

    // 监听端口port
    void listen(int port);

    // 单线程Server，client发送来的数据使用 dataProcess函数处理
    void run_single(void (*dataProcess)(char*));

    // 多线程Server，client发送来的数据使用 dataProcess函数处理
    void run_multithread(void (*dataProcess)(char*));

    // 具体server过程
    void serve(int client_fd, struct sockaddr_in client_addr, void (*dataProcess)(char*));
private:
    int handShake(int client_fd);   // 模拟三次握手
    Socket::Socket listen_socket;   // 监听套接字描述符
    char buff[MAX_LENGTH];          // 缓冲区
    int listen_num;                 // 同时connect的最多数量
    vector<int> client_list;        // 服务 client 的套接字描述符列表
    int max_client;                 // 最多同时服务 client 的数量
};

Server::Server(){ Server(1); }

Server::Server(int listen_num){
    Server(listen_num, 5);
}

Server::Server(int listen_num, int max_client){
    this->max_client = max_client;
    this->listen_num = listen_num;
    memset(this->buff, 0, MAX_LENGTH);
}

Server::~Server(){
    shutdown(this->listen_socket, SHUT_RDWR);
}

// 学 TCP 三次握手步骤
// TODO: 检查返回参数，记录同步号，检查 recv_message 输出问题。懒得搞了，有时间再弄吧
int Server::handShake(int client_fd){
    // SYN.ACK.seq.ack
    int ACK = 0, ack = 0, SYN = 1, seq = rand() % 100;
    string message;
    char recv_message[MAX_LENGTH];
    
    read(client_fd, recv_message, MAX_LENGTH);
    handShakeSetParm(message, SYN, ACK, seq, ack);
    cout << "【TCP三次握手 Server】: 接受消息1[" << recv_message << "]" << endl << endl;
    
    ACK = 1; SYN = 1; ack = seq + 1; seq = rand() % 100;
    message = to_string(SYN) + "." + to_string(ACK) + "." + to_string(seq) + "." + to_string(ack) + '\0';
    cout << "【TCP三次握手 Server】: 发送消息2[" << message << "]" << endl << endl;
    write(client_fd, message.c_str(), message.size());

    read(client_fd, recv_message, MAX_LENGTH);
    cout << "【TCP三次握手 Server】: 接受消息2[" << recv_message << "]" << endl << endl;

    return 1;
}

void Server::listen(int port){
    // Ipv4 字节流套接字 TCP
    this->listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(this->listen_socket == -1){   
        exit(1);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if((bind(this->listen_socket, (sockaddr*)&address, sizeof(struct sockaddr_in))) == -1){
        exit(1);
    }

    cout << "开始监听任务..." << endl;
    if(::listen(this->listen_socket, 5) != 0){
        perror("listen");
        exit(1);
    }
}

void Server::run_single(void (*dataProcess)(char*)){
    listen(8765);

    struct sockaddr_in client_addr;
    int size = sizeof(sockaddr_in);
    
    // * accept 会阻塞线程等待连接, 当 accept 收到连接请求, 获得到 client_fd时 才与client建立了连接
    int client_fd = accept(this->listen_socket, (struct sockaddr*)&client_addr, (socklen_t*)&size);

    char client_ip[16];
    inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), client_ip, 16);
    // 将网络字节序的端口转到本机字节序
    uint16_t client_port = ntohs(client_addr.sin_port);
    
    usleep(10000);

    // 模拟 TCP 握手协议
    if(handShake(client_fd) < 0){
        exit(1);
    }
    
    cout << endl << "与 client [" << client_fd << "-> "<< client_ip <<":" << client_port << "] 成功建立连接, 开始服务..." << endl;
    serve(client_fd, client_addr, dataProcess);
    cout << endl <<"监听结束..." << endl;
}

void Server::run_multithread(void (*dataProcess)(char*)){
    listen(8765);

    char client_ip[16];
    struct sockaddr_in client_addr;
    int client_fd, size = sizeof(sockaddr_in);
    uint16_t client_port;
    while(true){
        // * accept 会阻塞线程等待连接, 当 accept 收到连接请求, 获得到 client_fd时 才与client建立了连接
        client_fd = accept(this->listen_socket, (struct sockaddr*)&client_addr, (socklen_t*)&size);
        // 读取IP
        inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), client_ip, 16);
        // 读取Port
        client_port = ntohs(client_addr.sin_port);
        
        cout << endl << "与 client [" << client_fd << "-> "<< client_ip <<":" << client_port << "] 成功建立连接, 开始服务..." << endl;
        
        // TODO 这里要创建子线程去处理
        serve(client_fd, client_addr, dataProcess);
        
        // 子线程使 client_fd 引用计数 +1
        // 这里的close并不会使套接字真正关闭，只有引用计数为0时才关闭
        close(client_fd);
    }
    cout << endl <<"监听结束..." << endl;
}

// 开始对 client_fd 的服务
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