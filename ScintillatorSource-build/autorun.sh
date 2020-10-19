#!/bin/bash

while true;
do
    python ./macinc.py
    
    ./main muplus.mac

    echo "Script still going."
done
