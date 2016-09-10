function attractField = gen_attractive_field(xTarget,yTarget, xSize, ySize)

[x, y] = meshgrid (1:ySize, 1:xSize);

xi = 1/700;

attractField = xi * ( (x - xTarget).^2 + (y - yTarget).^2 );

end