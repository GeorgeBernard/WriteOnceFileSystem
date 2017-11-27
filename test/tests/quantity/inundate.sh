#!/bin/bash

rm -f ./files/*

for i in {1..1020}
do
    echo wow > ./files/$i.txt
done