
  
# vim:set ft=dockerfile:
FROM debian:buster

RUN apt-get update
RUN apt-get install -y mc
RUN apt-get install -y openssh-server
RUN apt-get install -y apt-utils
RUN mkdir /usr/share/wctlab/
COPY ./peer_server/ /usr/share/wctlab/
SHELL ["/bin/bash"]
