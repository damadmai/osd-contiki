#! /bin/bash

#set -x

BASH_GREEN="\e[1;32m"
BASH_RED="\e[1;31m"
BASH_NORMAL="\e[0m"

function printGreen() {
    echo -e "${BASH_GREEN}$1${BASH_NORMAL}"
}

printGreen "Enter IPv6 Address"
read post
pre="fdaa:e9b8:d03a:0:"
IPv6=$pre$post
printGreen "Adress: $IPv6"

printGreen "Version"
coap get coap://\[$IPv6\]/p/version
sleep 2
printGreen "Battery"
coap get coap://\[$IPv6\]/s/battery
sleep 2
printGreen "Status Led"
coap get coap://\[$IPv6\]/a/led
sleep 2
printGreen "Set LED on"
coap post coap://\[$IPv6\]/a/led -p mode=1
sleep 2
printGreen "Set LED off"
coap post coap://\[$IPv6\]/a/led -p mode=0
sleep 2
printGreen "get SG mode"
coap get coap://\[$IPv6\]/a/sg_mode
sleep 2
printGreen "set SG mode 2"
coap post coap://\[$IPv6\]/a/sg_mode -p mode=2
sleep 2
printGreen "get SG mode"
coap get coap://\[$IPv6\]/a/sg_mode
sleep 2
printGreen "set SG mode 3"
coap post coap://\[$IPv6\]/a/sg_mode -p mode=3
sleep 2
printGreen "get SG mode"
coap get coap://\[$IPv6\]/a/sg_mode
sleep 2
printGreen "set SG mode 4"
coap post coap://\[$IPv6\]/a/sg_mode -p mode=4
sleep 2
printGreen "get SG mode"
coap get coap://\[$IPv6\]/a/sg_mode
sleep 2
printGreen "set SG mode 1"
coap post coap://\[$IPv6\]/a/sg_mode -p mode=1
sleep 2
printGreen "get SG mode"
coap get coap://\[$IPv6\]/a/sg_mode
printGreen "test done"
