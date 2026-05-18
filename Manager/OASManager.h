#pragma once

#include "Manager.h"

#include <vector>
#include <memory>
#include <string>

#define OAS_COUNT 6

class TCPClient;
class MQTTClient;

struct OASConfig
{
    std::string id;

    std::string ip;

    int port;
};

class OASManager : public Manager
{
public:

    OASManager();

    virtual ~OASManager();

protected:

    void start() override;

    void stop() override;

    void process() override;

private:

    MQTTClient* m_mqttClient;

    std::vector<std::unique_ptr<TCPClient>> m_tcpClients;

    std::vector<OASConfig> m_oasConfigs;

    bool loadConfiguration(const std::string& fileName);
};