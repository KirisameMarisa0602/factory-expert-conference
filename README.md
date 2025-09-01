# Factory Expert Conference

A Qt 5 (qmake) based client/server demo for screen sharing, audio chat, and collaboration.

## Requirements
- Qt 5.12.8 (qmake)
- A C++ compiler toolchain (e.g., gcc/g++)
- Linux recommended for testing

## Build

### Server
```bash
cd server
qmake
make -j"$(nproc)"
./server
```

### Client
```bash
cd client
qmake client.pro
make -j"$(nproc)"
./client
```

## Notes
- The server reuses the shared protocol implementation from `server/common` via `common.pri`.
- The client uses QtMultimedia for audio and UDP for screen transport. Ensure your system has the necessary audio backends.
- Default resources are in `client/Resources/resources.qrc`.