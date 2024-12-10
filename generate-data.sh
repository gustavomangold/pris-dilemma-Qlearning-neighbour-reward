#!/bin/bash
LC_NUMERIC="en_US.UTF-8"

for defects in 0 5000 9000
do
    echo "1.2 $defects 1. 0."
    ./pris_pd_Qlearning_glut 1.2 $defects 1. 0. &
done
