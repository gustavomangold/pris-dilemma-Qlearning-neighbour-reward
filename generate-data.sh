#!/bin/bash
LC_NUMERIC="en_US.UTF-8"

for ALPHA_SHARE in 0. 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.
do
    echo "2.0 5000 1. $ALPHA_SHARE"
    ./pris_pd_Qlearning_glut 2.0 5000 0. $ALPHA_SHARE
done
