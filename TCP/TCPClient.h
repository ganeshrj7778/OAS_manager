#pragma once

#include <string>
#include <thread>

class MQTTClient;

class TCPClient
{
public:

    TCPClient(const std::string& oasId,
              const std::string& ip,
              int port,
              MQTTClient* mqttClient);

    ~TCPClient();

    void start();

    void stop();

private:

    void loop();

    void processMessage(
            const std::string& msg);

private:

    std::string m_oasId;

    std::string m_ip;

    int m_port;

    int m_socketFd;

    bool m_running;

    std::thread m_thread;

    MQTTClient* m_mqttClient;
};