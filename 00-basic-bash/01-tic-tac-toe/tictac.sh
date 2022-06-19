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

function check_horizontal()
{
    i0=$1 # $1 - start index of horizontal line 0, 3, 6
    i1=$((i0+1))
    i2=$((i0+2))

    v0=${field[((i0))]}
    v1=${field[((i1))]}
    v2=${field[((i2))]}

    if [ v0 == v1 ] && [ v1 == v2 ]
    then
        if [ v0 == "X" ]
        then
            echo "X - won!!!"
            return 0;
        elif [ v0 == "O" ]
        then
            echo "O - won!!!"
            return 0;
        fi
    fi
    return 1;
}

print_field

echo "Where to put X? (1-9)"

read position

field[((position-1))]="X"

print_field
