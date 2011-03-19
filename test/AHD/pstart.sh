#! /bin/bash

echo "POST /printserver.ahd#activate HTTP/1.0
Content-length: 0

" | tcpconnect localhost 9999