# syntax=docker/dockerfile:1
FROM ubuntu:20.04

ENV BUILD2_DIR="~/build2-build"
ENV BUILD2_URL="https://download.build2.org/0.15.0/build2-install-0.15.0.sh"
ENV LIBECS_DIR="~/development/libecs/"

RUN apt-get -y update
RUN apt-get -y install build-essential curl

RUN mkdir -p ${BUILD2_DIR}
RUN cd ${BUILD2_DIR}
RUN curl -sSfO ${BUILD2_URL}
RUN shasum -a 256 -b build2-install-0.15.0.sh
RUN yes | sh "build2-install-0.15.0.sh" --local

RUN mkdir -p ${LIBECS_DIR}

COPY . ${LIBECS_DIR}

WORKDIR ${LIBECS_DIR}

RUN bdep init -C @gcc-debug "./configs/gcc/debug/" cc config.cxx="g++" config.cxx.poptions="-DDEBUG" config.cxx.coptions="-g -Wall -Wextra"
RUN b update: "./configs/gcc/debug/"

RUN "./configs/gcc/debug/basic/basic/basic"
