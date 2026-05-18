#include "OASManager.h"

#include "../TCP/TCPClient.h"
#include "../mqtt/MQTTClient.h"

#include <iostream>
#include <fstream>
#include <sstream>

OASManager::OASManager()
{
   m_period = std::chrono::milliseconds(100);

    m_mqttClient = new MQTTClient();

    m_mqttClient->initialize("Config/config.ini");

    m_mqttClient->connectBroker();

    loadConfiguration("Config/config.ini");
}

OASManager::~OASManager()
{
    stop();

    delete m_mqttClient;
}

void OASManager::start()
{
    for(const auto& config : m_oasConfigs)
    {
        auto client =
            std::make_unique<TCPClient>(
                config.id,
                config.ip,
                config.port,
                m_mqttClient);

        client->start();

        m_tcpClients.push_back(std::move(client));

        std::cout << "Started "
                  << config.id
                  << " IP: "
                  << config.ip
                  << " Port: "
                  << config.port
                  << std::endl;
    }
}

void OASManager::stop()
{
    for(auto& client : m_tcpClients)
    {
        client->stop();
    }
}

void OASManager::process()
{
    /*
        Supervisor Thread

        Can later monitor:
        - connection state
        - watchdogs
        - reconnection
        - statistics
    */

        /*
        MQTT health monitoring
    */

    if(!m_mqttClient->isConnected())
    {
        m_mqttClient->reconnect();
    }

    /*
        Process MQTT internal loop
    */

    m_mqttClient->loop();
}

bool OASManager::loadConfiguration(const std::string& fileName)
{
    std::ifstream file(fileName);

    if(!file.is_open())
    {
        std::cerr << "Failed to open config: "
                  << fileName
                  << std::endl;

        return false;
    }

    std::string line;

    OASConfig currentConfig;

    while(std::getline(file, line))
    {
        if(line.empty())
        {
            continue;
        }

        /*
            Section
        */

        if(line[0] == '[')
        {
            if(!currentConfig.id.empty())
            {
                m_oasConfigs.push_back(currentConfig);
            }

            currentConfig = OASConfig();

            size_t endPos = line.find(']');

            currentConfig.id = line.substr(1, endPos - 1);

            continue;
        }

        /*
            Key = value
        */

        size_t equalPos = line.find('=');

        if(equalPos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, equalPos);

        std::string value = line.substr(equalPos + 1);

        if(key == "ip")
        {
            currentConfig.ip = value;
        }
        else if(key == "port")
        {
            currentConfig.port = std::stoi(value);
        }
    }

    /*
        Push last section
    */

    if(!currentConfig.id.empty())
    {
        m_oasConfigs.push_back(currentConfig);
    }

    std::cout << "Loaded OAS Configs: "
              << m_oasConfigs.size()
              << std::endl;

    return true;
}