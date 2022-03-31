#!/bin/sh

# ----- Sink to MongoDB

curl -s \
     -X "POST" "http://localhost:8083/connectors/" \
     -H "Content-Type: application/json" \
     -d '{
  "name": "mongo-sink",
  "config": {
     "connector.class": "com.mongodb.kafka.connect.MongoSinkConnector",
        "tasks.max": "1",
        "topics": "sysdigtopic",
        "connection.uri":"mongodb://root:admin@mongo:27017/admin?ssl=true",
        "database": "SysdigCapture",
        "collection": "SysdigCapture",
        "key.converter": "org.apache.kafka.connect.storage.StringConverter",
        "value.converter": "org.apache.kafka.connect.json.JsonConverter",
        "value.converter.schemas.enable": "false"
  }
}'
