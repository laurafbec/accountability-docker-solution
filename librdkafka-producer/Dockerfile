# Pull base image
FROM ubuntu:20.04

#Environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV SYSDIG_HOST_ROOT=/host
ENV HOST_ROOT /host
ENV HOME /root

RUN cp /etc/skel/.bashrc /root && cp /etc/skel/.profile /root

# Install sysdig and librdkafka required packages and dependencies
RUN apt-get update \
 && apt-get upgrade  -y \
 && apt-get install -y --no-install-recommends \
 && apt-get install -y build-essential \
    git \
    pkg-config \
    curl \
    libssl-dev \
   && rm -rf /var/lib/apt/lists/*


# Sysdig installation
# Trust the Draios GPG key, configure the apt repository, and update the package list
RUN curl -s https://s3.amazonaws.com/download.draios.com/DRAIOS-GPG-KEY.public | apt-key add - 
RUN curl -s -o /etc/apt/sources.list.d/draios.list https://s3.amazonaws.com/download.draios.com/stable/deb/draios.list
RUN apt-get update

# Install kernel headers
# The following command might not work with any kernel. Customization of the name of the package could be required
RUN apt-get -y install linux-headers-$(uname -r)

# Install sysdig
RUN apt-get -y install sysdig

# Librdkafka installation
WORKDIR /root
RUN git clone https://github.com/edenhill/librdkafka.git

WORKDIR /root/librdkafka
RUN /root/librdkafka/configure
RUN make
RUN make install

COPY ./conf/ld.so.conf /etc/ld.so.conf
RUN ldconfig

WORKDIR /opt/producer

COPY ./producer_entrypoint.sh /
ENTRYPOINT ["/producer_entrypoint.sh"]

