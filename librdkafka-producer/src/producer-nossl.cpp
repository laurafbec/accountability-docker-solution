/*
 * librdkafka - Apache Kafka C library
 *
 * Copyright (c) 2019, Magnus Edenhill
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <thread>

#include <librdkafka/rdkafkacpp.h>
#include <fstream>

#include </opt/producer/pstreams/pstream.h>

static volatile sig_atomic_t run = 1;

static void sigterm (int sig) {
  run = 0;
}


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
  void dr_cb (RdKafka::Message &message) {
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
   /* else
      std::cerr << "% Message delivered to topic " << message.topic_name() <<
        " [" << message.partition() << "] at offset " <<
        message.offset() << std::endl;*/
  }
};

class ExampleEventCb : public RdKafka::EventCb {
 public:
  void event_cb (RdKafka::Event &event) {
    switch (event.type())
    {
      case RdKafka::Event::EVENT_ERROR:
        if (event.fatal()) {
          std::cerr << "FATAL ";
          run = 0;
        }
        std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;

      case RdKafka::Event::EVENT_STATS:
      { std::cerr << "\"STATS\": " << event.str() << std::endl; 
        std::ofstream file("stats.json", std::ios_base::app);
	std::string my_event = event.str();
	file << my_event;
	file << "\n";
        break;}

      case RdKafka::Event::EVENT_LOG:
        fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str());
        break;

      default:
        std::cerr << "EVENT " << event.type() <<
            " (" << RdKafka::err2str(event.err()) << "): " <<
            event.str() << std::endl;
        break;
    }
  }
};

int main (int argc, char **argv) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <brokers> <topic>\n";
    exit(1);
  }

  std::string brokers = argv[1];
  std::string topic = argv[2];

  /* Create configuration object */
  RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

  std::string errstr;


  /* Set bootstrap broker(s) as a comma-separated list of host or host:port (default port 9092). */
  if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

 /* Set the Delay in milliseconds to wait for messages in the producer queue to accumulate before constructing message batches (MessageSets) to transmit to brokers.*/
 if (conf->set("linger.ms", "500", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

  /* Maximum number of messages allowed on the producer queue. This queue is shared by all topics and partitions. */
// if (conf->set("queue.buffering.max.messages", "50", errstr) != RdKafka::Conf::CONF_OK) {
  if (conf->set("batch.num.messages", "1000", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }
/* Delay in milliseconds to wait for messages in the producer queue to accumulate before constructing message batches (MessageSets) to transmit to brokers. 
A higher value allows larger and more effective (less overhead, improved compression) batches of messages to accumulate at the expense of increased message delivery latency. */
  if (conf->set("queue.buffering.max.ms", "1000", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }
/* Compression codec to use for compressing message sets.*/
 if (conf->set("request.required.acks", "1", errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }


/* Set  broker(s) as a comma-separated list of host or host:port (default port 9092). */
  if (conf->set("metadata.broker.list", brokers, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }


  signal(SIGINT, sigterm);
  signal(SIGTERM, sigterm);

  /* Set the delivery report callback.
   * This callback will be called once per message to inform
   * the application if delivery succeeded or failed.
  */
  ExampleDeliveryReportCb ex_dr_cb;

  if (conf->set("dr_cb", &ex_dr_cb, errstr) != RdKafka::Conf::CONF_OK) {
    std::cerr << errstr << std::endl;
    exit(1);
  }

 /* Set the event stats callback */
//  ExampleEventCb ex_event_cb;
//  conf->set("event_cb", &ex_event_cb, errstr);

  /* Create producer instance.*/
  RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
  if (!producer) {
    std::cerr << "Failed to create producer: " << errstr << std::endl;
    exit(1);
  }

  delete conf;

  /* Run a process and create a streambu that reads its stdout and stderr*/
  redi::ipstream proc("sysdig -c ros_log.lua", redi::pstreams::pstdout | redi::pstreams::pstderr);
  std::string line;

  /* Read sysdig output*/
  while (std::getline(proc.out(), line) && run)
        {
          //std::cout << "stdout: " << line << '\n';
          if (line.empty()) {
            producer->poll(0);
            continue;
           }

     
	 /* Send/Produce message.*/
  	retry:
    	RdKafka::ErrorCode err =
      		producer->produce(topic, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY, const_cast<char *>(line.c_str()), line.size(), NULL, 0, 0, NULL, NULL);

    	if (err != RdKafka::ERR_NO_ERROR) {
      		std::cerr << "% Failed to produce to topic " << topic << ": " <<
        	RdKafka::err2str(err) << std::endl;

     		if (err == RdKafka::ERR__QUEUE_FULL) {
        		/* If the internal queue is full, wait for messages to be delivered and then retry.*/
        		producer->poll(1000/*block for max 1000ms*/);
        		goto retry;
      		}

    	}
	else {
      		//std::cerr << "% Enqueued message (" << line.size() << " bytes) " << "for topic " << topic << std::endl;
    	}

    	producer->poll(0);
    }

    if (proc.eof() && proc.fail()){
                        proc.clear();
                }

  /* Wait for final messages to be delivered or fail. */
  std::cerr << "% Flushing final messages..." << std::endl;
  producer->flush(11*1000 /* wait for max 10 seconds */);

  /* Read sysdig stderr */
  while (std::getline(proc.err(), line) && run){
    std::cout << "stderr: " << line << '\n';}

  if (producer->outq_len() > 0)
    std::cerr << "% " << producer->outq_len() <<
              " message(s) were not delivered" << std::endl;

  delete producer;

  return 0;
}
