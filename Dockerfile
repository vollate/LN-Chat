FROM ubuntu:22.04

RUN apt update && apt install -y git zip unzip curl wget tar build-essential pkg-config python3 cmake ninja-build

WORKDIR /workspace

RUN git clone https://github.com/microsoft/vcpkg.git &&\
    cd vcpkg &&\
    ./bootstrap-vcpkg.sh &&\
    ./vcpkg install grpc

# Build the target

WORKDIR /workspace

RUN git clone https://github.com/vollate/LN-Chat.git&&\
    cd LN-Chat &&\
    cd server &&\
    export VCPKG_ROOT=/workspace/vcpkg &&\
    ./dev.sh release

EXPOSE 11451

ENTRYPOINT ["/bin/sh", "-c", "cd /workspace/LN-Chat/server/build-release/src && ./ln_server"]

