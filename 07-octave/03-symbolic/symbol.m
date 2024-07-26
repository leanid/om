% How to do symbolic evaluation in OCTAVE
% let ax+b=c
% how to solve it to find x - symbolicaly?
% > pkg install -forge symbolic
% > pkg load symbolic
% > pip install --user sympy (check pip --version same as default python)
pkg load symbolic

syms a b c x;
f = a*x+b+c;

solve(f==0,x) % expected result: (-b - c)/a
