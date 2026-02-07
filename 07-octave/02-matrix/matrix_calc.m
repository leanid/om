% see: https://wiki.octave.org/Using_Octave
% variable scale = 2
# Octave is 1-indexed. Matrix elements are accessed as matrix(rowNum, columnNum)
scale = 2; % print nothing cause ends with ;
disp("scale is:"), disp(scale);
scale*scale+scale % print 6

disp(100500) % print 100500

x = 3/4 * pi; # after ';' no text output!
x
y = sin(x)


scale_mat = [scale 0;
             0 scale]
# disp(scale_mat);
disp("matrix element access: scale_mat(1,1)"), disp(scale_mat(1,1));

p = [1;
     1]
scale_mat * p


step_x = 1.75;
step_y = 0.74;

move_mat = [1 0 step_x;
            0 1 step_y;
            0 0 1]
# disp(move_mat);

p3 = [p(1); p(2); 1]; % indexes in matrix start from 1
move_mat * p3

function rot_mat = make_rot2d(angle)
  rot_mat = [cos(angle) -sin(angle) 0;
             sin(angle) cos(angle) 0;
             0          0           1];
endfunction

output_precision(2);
format('short');

angle = pi/2
disp("in degrees: "), disp(rad2deg(angle));
disp("rotation matrix 2d:"), disp(make_rot2d(angle));
point = [1;
         0;
         0]
make_rot2d(angle) * point % expected output [0; 1; 0]
