# How to build 

# Build with real sensors (release)
mkdir build && cd build
cmake .. -DUSE_REAL_SENSORS=ON -DBUILD_DEBUG=OFF
make

# Build with real sensors (debug)
mkdir build && cd build
cmake .. -DUSE_REAL_SENSORS=ON -DBUILD_DEBUG=ON
make

# Build mocked
cmake .. -DUSE_REAL_SENSORS=OFF -DBUILD_DEBUG=OFF
make

# External protobuf repository
This project now uses an external protobuf repository for `snapshot.proto`.

- CMake will prefer fetching the remote repo from:
  `https://github.com/oxfrd/silo-sensors-proto-msg.git`
- If you want to use a local clone instead, set:
  `-DPROTO_REPO_PATH=/full/path/to/silo-sensors-proto-msg`
- You can override the fetch target with:
  `-DPROTO_REPO_GIT_URL=https://github.com/oxfrd/silo-sensors-proto-msg.git`
  `-DPROTO_REPO_GIT_TAG=main`

If `PROTO_REPO_PATH` is not provided, CMake will use the remote Git URL. If the remote fetch is disabled by an empty `PROTO_REPO_GIT_URL`, it will fall back to a sibling directory named `silo-sensors-proto-msg`.

# Instalacja
make install