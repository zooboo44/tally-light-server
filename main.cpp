//
//  main.cpp
//  tally-light-server
//
#include <iostream>
#include "AtemConnectionManager.h"
#include <mqtt/async_client.h>

int main(int argc, const char * argv[]) {
    std::cout << argv[1] << "\n" << argv[2] << std::endl;
    std::string mqttBrokerIp(argv[2]);
    std::string clientId("tallyServer");
    MqttConnectionManager* mqttClient = new MqttConnectionManager(mqttBrokerIp, clientId);
    AtemConnectionManager connManager(argv[1], mqttClient);
    while(true){
        
    }    
    return 0;
}
