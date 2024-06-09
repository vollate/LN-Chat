# Local Network ChatRoom

## Server Deploy

```sh
docker run -d -p ${YOUR_EXPOSE_PORT}:11451 lambillda/ln-chat-server
```

## Client Build

1. Use [vcpkg](https://github.com/microsoft/vcpkg) install `grpc`
1. Install `qt5` develop envrionment with your package manager
1. export `VCPKG_ROOT` environment variable as your vcpkg root path
1. enter `client` directory
1. run `./dev.sh [debug|release]` to build, the binary is at `build-[debug|release]/src/chat_client`

