#!/usr/bin/env bash

echo play XY-game

field=(_ _ _ \
       _ _ _ \
       _ _ _ )

function print_field()
{
    for ((i=0; i<3; i++))
    do
        echo ${field[((3*i))]} "|" ${field[(((3*i)+1))]} "|" ${field[(((3*i)+2))]}
    done
}

print_field

echo "Where to put X? (1-9)"

read position

field[((position-1))]="X"

print_field
