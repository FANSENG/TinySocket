#include "server.hh"

int main(){
    Server *server = new Server();
    server->run_single([](char* data){
        cout << endl << "Client:" << data << endl;    
    });
    return 0;
}