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
