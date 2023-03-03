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
    string buff;

    address.ip = argv[1];
    address.port = 8765;
    address.family = AF_INET;
    client->connect(address);

    while(true){
        cout << "请输入Message: ";
        cin >> buff;
        if(buff == "exit"){
            cout << endl << "断开连接..." << endl;
            break;
        }

        client->setBuffer(buff);
        client->send();

        // 睡眠 10ms
        usleep(10000);

        client->receive();
        buff = client->getBuffer();
        cout << endl <<"Server: " << buff << "" << endl;
    }

    client->closeConnect();
    delete client;

    return 0;
}