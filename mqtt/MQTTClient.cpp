#include "MQTTClient.h"

#include <iostream>
#include <fstream>
#include <sstream>

MQTTClient::MQTTClient()
{
    m_mosq = nullptr;

    m_port = 1883;

    m_latitude = 18.2234;
    m_longitude = 72.5531;
    m_vehicleDepth = 82.5;

    m_connected = false;
}

MQTTClient::~MQTTClient()
{
    if(m_mosq)
    {
        mosquitto_disconnect(m_mosq);

        mosquitto_destroy(m_mosq);
    }

    mosquitto_lib_cleanup();
}

bool MQTTClient::initialize(const std::string& iniFile)
{
    if(!loadConfig(iniFile))
    {
        return false;
    }

    mosquitto_lib_init();

    m_mosq = mosquitto_new(
                m_clientId.c_str(),
                true,
                this);

    if(!m_mosq)
    {
        return false;
    }

    //mosquitto_message_callback_set(m_mosq, MQTTClient::onMessage);

    return true;
}

bool MQTTClient::connectBroker()
{
    int rc = mosquitto_connect(
                m_mosq,
                m_host.c_str(),
                m_port,
                60);

    if(rc != MOSQ_ERR_SUCCESS)
    {
        std::cerr << "MQTT Connection Failed: "
                  << mosquitto_strerror(rc)
                  << std::endl;
        m_connected = false;
        return false;
    }

    std::cout << "Connected to MQTT Broker"
              << std::endl;

    m_connected = true;

    return true;
}

bool MQTTClient::isConnected() const
{
    return m_connected;
}

bool MQTTClient::reconnect()
{
    std::cout << "Attempting MQTT reconnect..."
              << std::endl;

    mosquitto_disconnect(m_mosq);

    int rc =
        mosquitto_reconnect(
            m_mosq);

    if(rc == MOSQ_ERR_SUCCESS)
    {
        m_connected = true;

        std::cout << "MQTT Reconnected"
                  << std::endl;

        return true;
    }

    m_connected = false;

    std::cerr << "MQTT Reconnect Failed: "
              << mosquitto_strerror(rc)
              << std::endl;

    return false;
}

void MQTTClient::loop()
{
    if(m_mosq)
    {
        int rc =
            mosquitto_loop(
                m_mosq,
                0,
                1);

        if(rc != MOSQ_ERR_SUCCESS)
        {
            m_connected = false;
        }
    }
}

bool MQTTClient::publishISADI(
        const std::string& payload)
{
    int rc = mosquitto_publish(
                m_mosq,
                NULL,
                m_publishTopicISADI.c_str(),
                payload.size(),
                payload.c_str(),
                0,
                false);

    return (rc == MOSQ_ERR_SUCCESS);
}

bool MQTTClient::publishISAMI(
        const std::string& payload)
{
    int rc = mosquitto_publish(
                m_mosq,
                NULL,
                m_publishTopicISAMI.c_str(),
                payload.size(),
                payload.c_str(),
                0,
                false);

    return (rc == MOSQ_ERR_SUCCESS);
}

double MQTTClient::latitude() const
{
    return m_latitude;
}

double MQTTClient::longitude() const
{
    return m_longitude;
}

double MQTTClient::vehicleDepth() const
{
    return m_vehicleDepth;
}

bool MQTTClient::loadConfig(
        const std::string& iniFile)
{
    std::ifstream file(iniFile);

    if(!file.is_open())
    {
        std::cerr << "Unable to open config file"
                  << std::endl;

        return false;
    }

    std::string line;

    while(std::getline(file, line))
    {
        if(line.empty())
            continue;

        auto pos = line.find('=');

        if(pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);

        std::string value = line.substr(pos + 1);

        if(key == "mqtt_host")
            m_host = value;

        else if(key == "mqtt_port")
            m_port = std::stoi(value);

        else if(key == "mqtt_client_id")
            m_clientId = value;

        else if(key == "mqtt_publish_topic_isadi")
            m_publishTopicISADI = value;

        else if(key == "mqtt_publish_topic_isami")
            m_publishTopicISAMI = value;
    }

    return true;
}

void MQTTClient::onMessage(
        struct mosquitto*,
        void* obj,
        const struct mosquitto_message* message)
{
    MQTTClient* self = static_cast<MQTTClient*>(obj);

    std::string payload(
                static_cast<char*>(message->payload),
                message->payloadlen);

    std::cout << "Received MQTT Payload: "
              << payload
              << std::endl;

    try
    {
        /*
            Very lightweight JSON parsing
            without external library
        */

        auto findValue = [&](const std::string& key) -> double
        {
            size_t keyPos = payload.find(key);

            if(keyPos == std::string::npos)
            {
                return 0.0;
            }

            size_t colonPos = payload.find(":", keyPos);

            size_t commaPos = payload.find(",", colonPos);

            size_t endBracePos = payload.find("}", colonPos);

            size_t endPos;

            if(commaPos == std::string::npos)
            {
                endPos = endBracePos;
            }
            else
            {
                endPos = std::min(commaPos, endBracePos);
            }

            std::string valueStr =
                    payload.substr(
                        colonPos + 1,
                        endPos - colonPos - 1);

            return std::stod(valueStr);
        };

        /*
            Extract values
        */

        self->m_latitude =
                findValue("latitude");

        self->m_longitude =
                findValue("longitude");

        self->m_vehicleDepth =
                findValue("depth");

        std::cout << "Updated Data:"
                  << std::endl;

        std::cout << "Latitude: "
                  << self->m_latitude
                  << std::endl;

        std::cout << "Longitude: "
                  << self->m_longitude
                  << std::endl;

        std::cout << "Depth: "
                  << self->m_vehicleDepth
                  << " meters"
                  << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "MQTT Payload Parse Error: "
                  << e.what()
                  << std::endl;
    }
}

bool MQTTClient::publishOASData(
        const std::string& oasId,
        double targetDistance,
        double energy,
        double correlation)
{
    std::stringstream ss;

    ss << "{"
       << "\"oas_id\":\""
       << oasId
       << "\","
       << "\"target_distance\":"
       << targetDistance
       << ","
       << "\"energy\":"
       << energy
       << ","
       << "\"correlation_factor\":"
       << correlation
       << "}";

    std::string payload =
        ss.str();

    std::string topic =
        "UUV/sensor/OAS/" + oasId;

    int rc =
        mosquitto_publish(
            m_mosq,
            NULL,
            topic.c_str(),
            payload.size(),
            payload.c_str(),
            0,
            false);

    return (rc == MOSQ_ERR_SUCCESS);
}