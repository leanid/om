# gray scale image
I = imread ("stalin.jpg");
printf("image_size: %d %d\n", size(I))
printf("pixel min: %d\n", min(I(:)))
printf("pixel max: %d\n", max(I(:)))
printf("pixel mean: %d\n", mean(I(:)))

hold ("on");
imshow(I)
# generate random numbers 0..1
R = rand (size(I));
[row, col] = find (R > 0.99);
# imshow (R);
# render red Ooous over stalin photo
plot (col, row, "ro");
hold ("off");
# imwrite(I, "out.png")
