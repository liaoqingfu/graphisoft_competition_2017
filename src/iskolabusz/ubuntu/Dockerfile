FROM ubuntu:latest

RUN echo "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu xenial main" > /etc/apt/sources.list.d/toolchain-test.list
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && \
    apt-get update && \
    apt-get -y --allow-unauthenticated install gcc-6 g++-6
