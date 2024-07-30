% How to do symbolic evaluation in OCTAVE
% let ax+b=c
% how to solve it to find x - symbolicaly?
% > pkg install -forge symbolic
% > pkg load symbolic
% > pip install --user sympy (check pip --version same as default python)
pkg load symbolic

syms a b c x y;
f = a*x+b+c;

solve(f==0,x) % expected result: (-b - c)/a

f(x) = 3*x^2+2*x+5;
f(1) % expected result: 10

% lets solve system of linear equations
eq1 = x + y == 6;
eq2 = x - y == 2;
ans1 = solve(eq1, eq2);
fprintf("x = %f y = %f\n", double(ans1.x), double(ans1.y));

syms m1 real;
syms v1 real;
syms m2 real;
syms v2 real;
syms v3 real;
syms v4 real;
eq3 = m1*v1 + m2*v2 == m1*v3 + m2*v4;
eq4 = (m1*v1^2)/2 + (m2*v2^2)/2 == (m1*v3^2)/2 + (m2*v4^2)/2;
ans2 = solve(eq3,eq4, v3, v4);
disp(ans2{1}.v3);
disp(ans2{1}.v4);
disp(ans2{2}.v3);
disp(ans2{2}.v4);

% try collision next iter
% e = {0...1} - 1-полностью упругое столкновение 0-полносью неупругое склеиваются объекты
syms e real;
eq5 = e == -(v3-v4)/(v1-v2);
ans3 = solve(eq3, eq5, v3, v4);
disp(ans3.v3);
disp(ans3.v4);
