# Base image
FROM ubuntu:16.04

# Install essentials
RUN apt-get update \
&&  apt-get install -y --no-install-recommends \
    tar \
    wget \
    build-essential \
    clang-4.0 \
    llvm-4.0-dev \
&&  ln -s /usr/bin/clang++-4.0 /usr/bin/clang++

WORKDIR /root

# Fetch and install boost and cmake from binaries
RUN wget "https://dl.bintray.com/boostorg/release/1.64.0/source/boost_1_64_0.tar.gz" --no-check-certificate \
&&  tar xf boost_1_64_0.tar.gz \
&&  rm boost_1_64_0.tar.gz \
&&  wget "https://cmake.org/files/v3.10/cmake-3.10.0-Linux-x86_64.tar.gz" --no-check-certificate \
&&  tar xf cmake-3.10.0-Linux-x86_64.tar.gz \
&&  rm cmake-3.10.0-Linux-x86_64.tar.gz \
&&  cp -r cmake-3.10.0-Linux-x86_64/bin/* /usr/local/bin \
&&  cp -r cmake-3.10.0-Linux-x86_64/doc/* /usr/local/doc \
&&  cp -r cmake-3.10.0-Linux-x86_64/share/* /usr/local/share \
&&  cp -r cmake-3.10.0-Linux-x86_64/man/* /usr/local/man

# Fetch and install protobuf from source
RUN wget "https://github.com/google/protobuf/releases/download/v3.5.1/protobuf-cpp-3.5.1.tar.gz" --no-check-certificate \
&& tar xf protobuf-cpp-3.5.1.tar.gz \
&&  rm protobuf-cpp-3.5.1.tar.gz \
&&  cd protobuf-3.5.1 \
&& ./configure --disable-shared --with-pic --prefix=/root/protobuf \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Set env
ENV BOOST_ROOT="/root/boost_1_64_0" \
    LD_LIBRARY_PATH="/root/simulator/lib:${LD_LIBRARY_PATH}" CXX="clang++" \
    CMAKE_PREFIX_PATH="/root/protobuf" CMAKE_BUILD_TYPE="Release"

# Copy simulator source
COPY ./ /root/codecharacter

# Copy runner script
COPY ./docker/compiler/compiler.sh /root

WORKDIR /root/codecharacter/build

# Build simulator
RUN cmake .. -DBUILD_PROJECT=no_tests -DCMAKE_INSTALL_PREFIX="/root/simulator" \
&& make -j $(expr $(grep -c '^processor' /proc/cpuinfo) - 1) install

# Make output dirs
RUN mkdir /root/output_libs

CMD ["/bin/bash", "/root/compiler.sh"]
