#pragma once

#include <string>
#include <mosquitto.h>

class MQTTClient
{
public:

    MQTTClient();

    ~MQTTClient();

    bool initialize(const std::string& iniFile);

    bool connectBroker();

    void loop();

    bool publishISADI(const std::string& payload);

    bool publishISAMI(const std::string& payload);

    bool publishOASData(
        const std::string& oasId,
        double targetDistance,
        double energy,
        double correlation);

    double latitude() const;

    double longitude() const;

    double vehicleDepth() const;

    bool isConnected() const;

    bool reconnect();

    bool m_connected;

private:

    struct mosquitto* m_mosq;

    std::string m_host;

    int m_port;

    std::string m_clientId;

    std::string m_publishTopicISADI;

    std::string m_publishTopicISAMI;

    double m_latitude;

    double m_longitude;

    double m_vehicleDepth;

private:

    bool loadConfig(const std::string& iniFile);

    static void onMessage(
            struct mosquitto* mosq,
            void* obj,
            const struct mosquitto_message* message);
};