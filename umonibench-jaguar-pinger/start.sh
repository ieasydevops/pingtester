#!/bin/sh
cur=`pwd`
appName=jaguar_pinger
nohup $cur/build/release/bin/$appName >./var/jaguar_pinger.log 2>&1 &
