# gray scale image
I = imread ("stalin.jpg");
printf("image_size: %d %d\n", size(I))
printf("pixel min: %d\n", min(I(:)))
printf("pixel max: %d\n", max(I(:)))
printf("pixel mean: %d\n", mean(I(:)))

imshow(I)

hold ("on");
% 2. Определите координаты линии (x1, y1) -> (x2, y2)
x = [10, 200]; % Координаты по горизонтали
y = [50, 300]; % Координаты по вертикали

% 3. Нарисуйте зеленую линию
plot(x, y, 'g', 'LineWidth', 2);
# generate random numbers 0..1
R = rand (size(I));
[row, col] = find (R > 0.999);
# render red Ooous over stalin photo
plot (col, row, "ro");
hold ("off");
# imwrite(I, "out.png")
