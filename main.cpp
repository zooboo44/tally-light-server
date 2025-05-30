#include <iostream>
#include "AtemConnectionManager.h"

int main(int argc, const char * argv[]) {
    //const char* ip = "192.168.1.168";
    std::cout << "argv[1]: " << argv[1] << std::endl;
    AtemConnectionManager* connManager = new AtemConnectionManager(argv[1]);
    while(true){
        
    }
>>>>>>> 2d60127 (implement base atem connection manager functions)
    return 0;
}
