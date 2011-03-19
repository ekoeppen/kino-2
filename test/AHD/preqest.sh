#! /bin/bash

request=`cat $1`
length=`wc -c $1 | awk '{print $1}'`

echo "PUT /request.ahd HTTP/1.0
Content-length: $length
Content-type: text/x-ahd
X-AHD-Origin: $2

$request" | tcpconnect localhost 9999
