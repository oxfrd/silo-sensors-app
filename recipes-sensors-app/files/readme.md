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

# Instalacja
make install