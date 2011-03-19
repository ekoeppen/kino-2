#! /bin/bash

echo -n "$4=" >/tmp/r
sed "s/%/%25/g;s/&/%26/g;s/=/%3d/g" $1 >>/tmp/r

request=`cat /tmp/r`
receiver=$2
method=$3
length=`wc -c /tmp/r | awk '{print $1}'`

echo "POST /$receiver#$method HTTP/1.0
Content-length: $length 

$request" | tcpconnect localhost 9999