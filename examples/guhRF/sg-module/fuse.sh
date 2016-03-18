#! /bin/bash

sudo avrdude -pm256rfr2 -catmelice -Pusb -U hfuse:w:0x98:m -U lfuse:w:0xce:m -U efuse:w:0xf8:m
