#!/usr/bin/env sh

echo "play XO-game"

field=(_ _ _ \
       _ _ _ \
       _ _ _ )

function print_field()
{
    i=0
    while [ "$i" -lt 3 ] # while i "less then" 3
    do
        i0=`expr 3 \* $i` # we have to backslash * symbol
        i1=`expr 3 \* $i + 1`
        i2=`expr 3 \* $i + 2`
        echo ${field[$i0]} "|" ${field[$i1]} "|" ${field[$i2]}
        i=`expr $i + 1`
    done
}

function check_3_indexes()
{
    i0=$1
    i1=$2
    i2=$3

    v0=${field[((i0))]}
    v1=${field[((i1))]}
    v2=${field[((i2))]}

    if [ $v0 == $v1 ] && [ $v1 == $v2 ]
    then
        if [ $v0 == "X" ]
        then
            echo "X - won!!!"
            return 0;
        elif [ $v0 == "O" ]
        then
            echo "O - won!!!"
            return 0;
        fi
    fi
    return 1;

}

function check_horizontal()
{
    i0=$1 # $1 - start index of horizontal line 0, 3, 6
    i1=$((i0+1))
    i2=$((i0+2))

    if check_3_indexes $i0 $i1 $i2
    then
        return 0
    fi
    return 1
}

function check_vertical()
{
    i0=$1 # $1 - start index of vertical line 0, 1, 2
    i1=$((i0+3))
    i2=$((i0+6))

    if check_3_indexes $i0 $i1 $i2
    then
        return 0
    fi
    return 1
}

function check_all_horizontal()
{
    if check_horizontal 0 || check_horizontal 3 || check_horizontal 6
    then
        return 0;
    fi
    return 1;
}

function check_all_vertical()
{
    if check_vertical 0 || check_vertical 1 || check_vertical 2
    then
        return 0;
    fi
    return 1;
}

function check_win_state()
{
    if check_all_horizontal || check_all_vertical || check_3_indexes 0 4 8 || check_3_indexes 2 4 6
    then
        exit 0;
    fi
    return 1;
}

print_field

while [ true ]
do
    echo "Where to put X? (1-9)"
    read position
    field[((position-1))]="X"
    print_field
    check_win_state
    echo "Where to put O? (1-9)"
    read position
    field[((position-1))]="O"
    print_field
    check_win_state
done
