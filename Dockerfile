FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    gdbserver \
    pkg-config \
    git \
    nano \
    vim \
    iputils-ping \
    net-tools \
    mosquitto \
    mosquitto-clients \
    libmosquitto-dev

WORKDIR /workspace

CMD ["/bin/bash"]