#!/usr/bin/env bash

printf "%s\n" "play XO-game"
# create array of XO-game board
# it will create 9 variables from field0 ... to field8 with content '_'
field=(_ _ _
	_ _ _
	_ _ _)

function print_field() {
	for ((i = 0; i < 3; i++)); do
		# to do math expression surround with ((...))
		local col0=${field[(((3 * i) + 0))]}
		local col1=${field[(((3 * i) + 1))]}
		local col2=${field[(((3 * i) + 2))]}
		printf "%s|%s|%s\n" "$col0" "$col1" "$col2"
	done
}

function check_3_indexes() {
	local v0=${field[$1]}
	local v1=${field[$2]}
	local v2=${field[$3]}

	if [ "$v0" == "$v1" ] && [ "$v1" == "$v2" ]; then
		if [ "$v0" == "X" ]; then
			echo "X - won!!!"
			return 0
		elif [ "$v0" == "O" ]; then
			echo "O - won!!!"
			return 0
		fi
	fi
	return 1
}

function check_horizontal() {
	i0=$1 # $1 - start index of horizontal line 0, 3, 6
	i1=$((i0 + 1))
	i2=$((i0 + 2))

	if check_3_indexes "$i0" "$i1" "$i2"; then
		return 0
	fi
	return 1
}

function check_vertical() {
	local i0=$1 # $1 - start index of vertical line 0, 1, 2
	local i1=$((i0 + 3))
	local i2=$((i0 + 6))

	if check_3_indexes "$i0" "$i1" "$i2"; then
		return 0
	fi
	return 1
}

function check_all_horizontal() {
	if check_horizontal 0 ||
		check_horizontal 3 ||
		check_horizontal 6; then
		return 0
	fi
	return 1
}

function check_all_vertical() {
	if check_vertical 0 ||
		check_vertical 1 ||
		check_vertical 2; then
		return 0
	fi
	return 1
}

function check_win_state() {
	if check_all_horizontal ||
		check_all_vertical ||
		check_3_indexes 0 4 8 ||
		check_3_indexes 2 4 6; then
		exit 0
	fi
	return 1
}

print_field

while true; do
	echo "Where to put X? (1-9)"
	# Do not let backslash (\\) act as an escape character (read -r) - raw
	read -r position
	player1_pos=$((position - 1))
	field[player1_pos]="X"
	print_field
	check_win_state
	echo "Where to put O? (1-9)"
	read -r position
	player2_pos=$((position - 1))
	field[player2_pos]="O"
	print_field
	check_win_state
done
