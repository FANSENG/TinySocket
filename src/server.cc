#include "server.hh"

int main(){
    Server *server = new Server();
    server->run([](char* data){
        cout << endl << "Client:" << data << endl;    
    });
    return 0;
}