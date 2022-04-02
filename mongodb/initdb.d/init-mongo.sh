#!/usr/bin/env bash
echo "Creating mongo users..."
mongo -- "admin" <<EOF
    var admin = db.getSiblingDB('admin');
    admin.auth('root', 'admin');
    db.createUser({user: 'admin', pwd: 'admin', roles: [{ role: "readWrite", db: "SysdigCapture" }]});
EOF
echo "Mongo users created."
