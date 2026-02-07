# example how to program in octave

x = zeros (50,1); # matrix 50x1 - column of 50 zeroes
for i = 1:2:100 # iterate from 1 to 100 with step size 2
  x(i) = i^2;
endfor

y = zeros (50,1);
k = 1;
step = 2;
while (k <= 100)
  y(k) = k^2;
  k = k + step;
endwhile

i = 1:2:100;      # create an array with 50-elements
x = i.^2;         # each element is squared
y = x + 9;        # add 9 to each element
z = y./i;         # divide each element in y by the corresponding value in i
w = sin (i / 10); # take the sine of each element divided by 10

# 2d plot line graph
plot (i / 10, w);
title ('w = sin (i / 10)');
xlabel ('i / 10');
ylabel ('w');

# working with strings in gnu octave
first_string = "hello world";
second_string = "!";
[first_string, second_string] # concatenate both strings
printf("printf is working as you expected %s\n", [first_string, second_string]);
printf("strings is simple matrix of characters\n");
s = "string";
s = [s, " ", s, " ", s]
s = "";
for i = 1:10
  s = [s, num2str(i)];
  if(i < 10)
    s = [s, " "];
  endif
endfor
["[\"", s, "\"]"]

# function:
# no return value
function wakeup (message)
  printf ("from wakeup function: %s\n", message);
endfunction
wakeup("hello")
# with return value
function retval = avg (v)
  retval = sum (v) / length (v);
endfunction

printf("function avg returns: %f\n", avg([1, 2, 3, 4, 5]));

function [y, ia, ib] = my_func(a_arg, b_arg, c_arg)
    disp(inputname(1)); # s
    disp(inputname(2)); # <NONE>
    y = 1;
    ia = 2;
    ib = 4;
endfunction

[a, b, c] = my_func(s, 2, 3);
nargin("my_func") # ans -3

# integration example
function y = f (x)
  y = x .* sin (1./x) .* sqrt (abs (1 - x));
endfunction

# intergrate function "f" on interval 0...3
[q, ier, nfun, err] = quad ("f", 0, 3)

# display function "f"
x = 0:0.0001:3;
y = f(x);
plot(x, y)
title ('y = f(x)');
xlabel ('x 0...3');
ylabel ('y');
