#!/bin/sh
cur=`pwd`
app=$cur/build/release/bin/jaguar_pinger
for data in `ps -ef | grep $app | grep -v grep |awk '{print $2}' `
do 
  kill -9 $data
done

for data in `ps -ef | grep f1 | grep log | grep -v grep |awk '{print $2}' `
do 
  kill -9 $data
done

exit
