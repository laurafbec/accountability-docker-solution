#!/bin/bash

set -e

#Compile produce or Kafka's client
g++ -std=c++17 $(pkg-config --cflags rdkafka++) /opt/producer/producer.cpp -o /opt/producer/producer $(pkg-config --libs rdkafka++)
exec "$@"
