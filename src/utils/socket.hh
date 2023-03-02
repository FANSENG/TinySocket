#ifndef SOCKET_HH
#define SOCKET_HH

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LENGTH 1024

using namespace std;

namespace Socket{
    
    typedef int Socket;
    typedef string Ip;
    typedef int Port;
    typedef string Data;
    typedef int Family;

    struct _Address
    {
        Ip ip;
        Port port;
        Family family;
    }Address_default={"", 8765, AF_INET};

    typedef struct _Address Address;

    typedef struct
    {
        Address address;
        Data data;
    }Datagram;
    
    struct sockaddr_in* switch_to_sockaddr(const Address& address){
        struct sockaddr_in* ret;
        ret = (struct sockaddr_in*)(malloc(sizeof(struct sockaddr_in)));
        ret->sin_family = address.family;
        inet_aton(address.ip.c_str(), &(ret->sin_addr));
        ret->sin_port = htons(address.port);

        return ret;
    }

    Address* switch_to_address(const struct sockaddr_in& address){
        Address* ret = (Address*)malloc(sizeof(Address));
        // n : 网络字节序格式的信息, inet_ntoa 将网络字节序的ip号转为十进制的字符串格式
        ret->ip = inet_ntoa(address.sin_addr);
        // 网络字节序 转为 unsigned int
        ret->port = ntohs(address.sin_port);
        ret->family = address.sin_family;

        return ret;
    }

    class SocketException{
    private:
        string _message;
    public:
        SocketException(string error) : _message(error) {}
        virtual const char* get(){ return this->_message.c_str(); }
    };

    class UDP{
    private:
        Socket _socket_id; // socket 的描述符
        bool _binded;
    public:
        UDP();
        UDP(int domain);
        ~UDP();
        bool closeUDP();
        void bind(Port port, int family = AF_INET);
        void send(Ip ip, Port port, Data data, int family = AF_INET);
        Datagram receive();
    };

    UDP::UDP(){ UDP(AF_INET); }

    UDP::UDP(int domain){
        this->_socket_id = socket(domain, SOCK_DGRAM, 0);
        if(this->_socket_id == -1){
            throw SocketException("来自 UDP->[构造函数]: 不能创建socket");
        }
        this->_binded = false;
    }

    UDP::~UDP(){
        shutdown(this->_socket_id, SHUT_RDWR);
    }

    bool UDP::closeUDP(){
        if(close(this->_socket_id) < 0){
            throw SocketException("来自 UDP->[closeUDP]: 未能正确关闭socket");
            return false;
        }
        return true;
    }

    void UDP::bind(Port port, int family){
        struct sockaddr_in address;
        address.sin_family = htons(family);
        address.sin_port = htons(port);
        address.sin_addr.s_addr = htonl(INADDR_ANY);

        // 若已经绑定端口，则释放之前的套接字
        // 然后根据输入信息新建套接字
        if(this->_binded){
            this->closeUDP();
            this->_socket_id = socket(family, SOCK_DGRAM, 0);
        }

        // 绑定端口和套接字
        if(::bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr)) < 0){
            stringstream error;
            error << "[监听端口]->[port : " << address.sin_port << "] 无法绑定到socket" << endl;
            throw SocketException(error.str());
        }

        this->_binded = true;
    }

    void UDP::send(Ip ip, Port port, Data data, int family){
        struct sockaddr_in address;
        address.sin_family = htons(family);
        address.sin_port = htons(port);
        inet_aton(ip.c_str(), &address.sin_addr);

        if( sendto(this->_socket_id, data.c_str(), data.size() + 1, 0, (struct sockaddr*)&address, sizeof(sockaddr_in)) == -1){
            stringstream error;
            error << "[发送失败]->[ip : " << ip << "] [port : " << address.sin_port << "] [data : " << data << "]" << endl;
            throw SocketException(error.str());
        }
    }

    Datagram UDP::receive(){
        char* buff = (char*)malloc(sizeof(char) * MAX_LENGTH);
        struct sockaddr_in address;
        Datagram ret;

        if(recvfrom(this->_socket_id, buff, MAX_LENGTH, 0, (struct sockaddr*)&address, (socklen_t*)sizeof(struct sockaddr_in)) == -1){
            stringstream error;
            error << "[接受失败]->[socket_id : " << _socket_id << "]" << endl;
            throw SocketException(error.str());
        }

        ret.data = buff;
        ret.address.family = ntohs(address.sin_family);
        ret.address.port = ntohs(address.sin_port);
        ret.address.ip = inet_ntoa(address.sin_addr);

        free(buff);

        return ret;

    }

    class TCP{
    private:
        Socket _socket_id;
        bool _binded;
        bool _connected;

    public:
        TCP();
        TCP(int domain);
        ~TCP();
        bool closeTCP();
        bool bind(Port port, int family = AF_INET);
        // bool send(Ip ip, Port port, Data data, unsigned family = AF_INET);
        // Data receive();
    };

    TCP::TCP(){ TCP(AF_INET); }

    TCP::TCP(int domain){
        this->_socket_id = socket(domain, SOCK_DGRAM, 0);
        if(this->_socket_id == -1){
            throw SocketException("来自 TCP->[构造函数]: 不能创建socket");
        }
        this->_binded = false;
        this->_connected = false;
    }

    TCP::~TCP(){
        shutdown(this->_socket_id, SHUT_RDWR);
    }

    bool TCP::closeTCP(){
        if(close(this->_socket_id) == -1){
            throw SocketException("来自 TCP->[closeTCP]: 未能正确关闭socket");
            return false;
        }
        return true;
    }
    
    bool TCP::bind(Port port, int family){
        struct sockaddr_in address;
        address.sin_family = htons(family);
        address.sin_port = htons(port);
        address.sin_addr.s_addr = htonl(INADDR_ANY);

        // 若已经绑定端口，则释放之前的套接字
        // 然后根据输入信息新建套接字
        if(this->_binded){
            this->closeTCP();
            this->_socket_id = socket(family, SOCK_DGRAM, 0);
        }

        // 绑定端口和套接字
        if(::bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr)) < 0){
            stringstream error;
            error << "[监听端口]->[port : " << address.sin_port << "] 无法绑定到socket" << endl;
            throw SocketException(error.str());
        }

        this->_binded = true;
        return true;
    };

    // bool TCP::send(Ip ip, Port port, Data data, unsigned family = AF_INET){
        
    // }

    // Data receive(){

    // } 

}

#endif //SOCKET_HH