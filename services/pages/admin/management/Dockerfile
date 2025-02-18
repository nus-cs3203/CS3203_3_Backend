# Use an official Ubuntu 20.04 as the base
FROM ubuntu:20.04

# Set up environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary dependencies, including those for building mongo-cxx-driver
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    pkg-config \
    libssl-dev \
    libsasl2-dev \
    libboost-all-dev \
    libasio-dev \
    tree \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory inside the container
WORKDIR /app

# Clone the CrowCpp library
RUN mkdir -p libs && cd libs && git clone https://github.com/CrowCpp/Crow.git

# Build and install mongo-cxx-driver
RUN cd /tmp && \
    curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.10.1/mongo-cxx-driver-r3.10.1.tar.gz && \
    tar -xzf mongo-cxx-driver-r3.10.1.tar.gz && \
    cd mongo-cxx-driver-r3.10.1/build && \
    cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DBSONCXX_POLY_USE_BOOST=1 \
      -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF && \
    cmake --build . && \
    cmake --build . --target install

# Copy project files to the container
COPY CMakeLists.txt /app
COPY common /app/common
COPY services /app/services
COPY scripts /app/scripts

# Configure and build the main project
RUN mkdir build && cd build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=../release \
        -DCROW_INCLUDE_DIR="/app/libs/Crow/include/" \
    && make -j3 && make install

# Set the default command to run the application
RUN echo "/usr/local/lib" > /etc/ld.so.conf.d/mongo-cxx-driver.conf && \
    ldconfig
CMD ["./release/bin/pages_admin_management"]