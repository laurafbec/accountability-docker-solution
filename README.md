# accountability-solution
This repository includes a Docker-based accountability solution based on Sysdig, Librdkafka producer, Kafka and MongoDB. This approach aims to identify the causes that have triggered a set of specific events, thanks to the use of the syscalls run by the monitored system. Features such as being completely decoupled from the monitored system, real-time analysis and optimized querying make this solution an optimal choice when it comes to understanding the root causes of a system's behaviour. Different assessment scenarios have been developed to define the best strategy to reduce the impact of the audit process and logging tasks.

# Software artifacts
Sysdig (version 0.28.0)

Librdkafka (version 1.7.0)

Zookeeper (version 7.0.1)

Kafka (version 7.0.1)

Kafka-connect (version 7.0.1)

MongoDB (version 5.0.5)

MongoDB Atlas (version 5.0.6 Enterprise)

Docker-compose (version 1.26.0)

# Installation
Dependencies can be installed with [setup.sh](https://github.com/inflfb00/accountability-docker-solution/blob/main/setup.sh).
The kernel headers must be installed in the host operating system, before running sysdig.

# Configuration and usage

## Scenario i. ROS logging engine


## Scenario ii. Zookeeper, Kafka broker, Kafka connect, Librdkafka producer with Sysdig and MongoDB (local)

