## Open VS Code
## install 'Dev Containers' extension
## Press F1
## select option "Dev ContainerS: Open Folder in Container"
## sit back. let VS Code do the work.
## build (ctrl+shift+b)
## run

## Architecture
                +-------------------+
                | Qt OAS Emulator   |
                | OAS1 -> TCP 7000  |
                | OAS2 -> TCP 7001  |
                | OAS3 -> TCP 7002  |
                | OAS4 -> TCP 7003  |
                | OAS5 -> TCP 7004  |
                | OAS6 -> TCP 7005  |
                +---------+---------+
                          |
                          v
                +-------------------+
                | OASManager        |
                | Thread Manager    |
                +---------+---------+
                          |
        +-----------------+------------------+
        |        |       |       |       |   |
        v        v       v       v       v   v
     TCPClient TCPClient TCPClient ... TCPClient
        |        |       |
        +--------+-------+
                 |
                 v
           MQTT Publisher
                 |
                 v
        UUV/sensor/OAS -->

## Payload Structure Example
{
  "oas_id": "OAS1",
  "message_type": "ISADI",
  "target_distance": 37.5,
  "energy": 0.56,
  "correlation_factor": 0.51
}