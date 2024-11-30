#!/bin/bash
# change name to run_myself.sh to be ignored by git.
./build/server/server \
-server_addr=192.168.3.18:25575 \
-db_ip=127.0.0.1 \
-db_port=3306 \
-db_username=root \
-db_password=password \
-db_name=your_db