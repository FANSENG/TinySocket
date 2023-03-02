#include "client.hh"


// 通过命令行传参时,最后会保存一个 0 作为结束符
// 所以如果传入 3 个参则长度为 4
int main(int argc, char* argv[]){
    if(argc != 2){
        cout << "ERROR: client 需要接收一个参数." << endl;
        exit(1);
    }
    Client *client = new Client();
    Socket::Address address;
    address.ip = argv[1];
    address.port = 8765;
    address.family = AF_INET;

    client->run(address);

    return 0;
}