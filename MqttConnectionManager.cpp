#include "MqttConnectionManager.h"
#include <string>
#include <iostream>

MqttConnectionManager::MqttConnectionManager(std::string &ip, std::string &clientId) : m_client(ip, clientId){
    
}

void MqttConnectionManager::connect(){
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);
    connOpts.set_keep_alive_interval(1000);
    std::cout << "connecting to mqtt" << std::endl;
    m_client.connect(connOpts)->wait();
    std::cout << "connected to mqtt" << std::endl;
}

void MqttConnectionManager::publish(const std::string &topic, const std::string &payload){
    mqtt::message_ptr message = mqtt::make_message(topic, payload);
    m_client.publish(topic, payload)->wait();
}

void MqttConnectionManager::disconnect(){
    m_client.disconnect()->wait();
}
