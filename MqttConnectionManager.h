#include <mqtt/async_client.h>
#include <string>
class MqttConnectionManager{
public:
    MqttConnectionManager(std::string &, std::string &);
    void connect();
    void disconnect();
    void publish(const std::string &, const std::string &);
private:
    mqtt::async_client m_client;
};
