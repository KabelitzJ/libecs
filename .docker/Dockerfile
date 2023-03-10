# syntax=docker/dockerfile:1
FROM ubuntu:20.04

RUN apt-get -y update
RUN apt-get -y install build-essential curl

RUN mkdir -p ~/build2-build
RUN cd ~/build2-build
RUN curl -sSfO https://download.build2.org/0.15.0/build2-install-0.15.0.sh
RUN shasum -a 256 -b build2-install-0.15.0.sh
RUN yes | sh build2-install-0.15.0.sh --local
