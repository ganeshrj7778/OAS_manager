#include "TCPClient.h"
#include "../mqtt/MQTTClient.h"

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

TCPClient::TCPClient(
        const std::string& oasId,
        const std::string& ip,
        int port,
        MQTTClient* mqttClient)
{
    m_oasId = oasId;

    m_ip = ip;

    m_port = port;

    m_mqttClient = mqttClient;

    m_running = false;

    m_socketFd = -1;
}

TCPClient::~TCPClient()
{
    stop();
}

void TCPClient::start()
{
    m_running = true;

    m_thread = std::thread(&TCPClient::loop, this);
}

void TCPClient::stop()
{
    m_running = false;

    if(m_thread.joinable())
    {
        m_thread.join();
    }

    if(m_socketFd > 0)
    {
        close(m_socketFd);
    }
}

void TCPClient::loop()
{
    while(m_running)
    {
        m_socketFd = socket(AF_INET, SOCK_STREAM, 0);

        if(m_socketFd < 0)
        {
            std::cerr << "Socket creation failed"
                      << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2));

            continue;
        }

        sockaddr_in serverAddr;

        serverAddr.sin_family = AF_INET;

        serverAddr.sin_port = htons(m_port);

        inet_pton(AF_INET, m_ip.c_str(), &serverAddr.sin_addr);

        std::cout << "Connecting "
                  << m_oasId
                  << " -> "
                  << m_ip
                  << ":"
                  << m_port
                  << std::endl;

        if(connect(m_socketFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        {
            std::cerr << "TCP Connection Failed: "
                      << m_oasId
                      << std::endl;

            close(m_socketFd);

            std::this_thread::sleep_for(std::chrono::seconds(2));

            continue;
        }

        std::cout << "Connected: "
                  << m_oasId
                  << std::endl;

        char buffer[2048];

        while(m_running)
        {
            int bytes = recv(m_socketFd, buffer, sizeof(buffer)-1, 0);

            if(bytes <= 0)
            {
                std::cerr << "Disconnected: "
                          << m_oasId
                          << std::endl;

                close(m_socketFd);

                break;
            }

            buffer[bytes] = '\0';

            std::string msg(buffer);

            processMessage(msg);
        }
    }
}

void TCPClient::processMessage(const std::string& msg)
{
    std::cout << m_oasId
              << " RX: "
              << msg
              << std::endl;

    double targetDistance = 0.0;

    double energy = 0.0;

    double correlation = 0.0;

    if(msg.find("$ISADI") != std::string::npos)
    {
        int parsed =
            sscanf(msg.c_str(),
                   "$ISADI,%lf,M,%lf,%lf",
                   &targetDistance,
                   &energy,
                   &correlation);

        if(parsed == 3)
        {
            m_mqttClient->publishOASData(
                        m_oasId,
                        targetDistance,
                        energy,
                        correlation);
        }
        else
        {
            std::cerr << m_oasId
                      << " ISADI Parse Failed"
                      << std::endl;
        }
    }
}