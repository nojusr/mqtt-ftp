#!/bin/bash

#performs a low-grade small test suite on the program.
#to test, start the daemon, run it on localhost with example.conf, and subscribe to 'kelp/#', then run this script.

# the 4 commands below should all return RESP_OK (rc 0)
mosquitto_pub -h localhost -m "{\"request_id\": 1234, \"command\": \"create\", \"file_path\": \"/tmp/test3\"}" -t kelp/input 
mosquitto_pub -h localhost -m "{\"request_id\": 1235, \"command\": \"write\", \"file_path\": \"/tmp/test3\", \"write_input\": \"file contents!\"}" -t kelp/input 
mosquitto_pub -h localhost -m "{\"request_id\": 1236, \"command\": \"read\", \"file_path\": \"/tmp/test3\"}" -t kelp/input 
mosquitto_pub -h localhost -m "{\"request_id\": 1237, \"command\": \"delete\", \"file_path\": \"/tmp/test3\"}" -t kelp/input 

# the commands below should return RESP_NOT_FOUND (rc 2)
mosquitto_pub -h localhost -m "{\"request_id\": 1238, \"command\": \"read\", \"file_path\": \"/tmp/not_a_file\"}" -t kelp/input 
mosquitto_pub -h localhost -m "{\"request_id\": 1236, \"command\": \"delete\", \"file_path\": \"/tmp/not_a_file\"}" -t kelp/input 

