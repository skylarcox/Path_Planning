function attractField = gen_attractive_field_3D(xTarget,yTarget, zTarget, xSize, ySize, zSize)
%
% Inputs: 
%       xTarget - target location in X
%       yTarget - target location in Y
%       zTarget - target location in Z
%       xSize - size in row space
%       ySize - size in column space
%       zSize - size in depth space
%
%

[x, y, z] = meshgrid(1:ySize, 1:xSize, 1:zSize);

xi = 1/700;

attractField = xi * ( (x - xTarget).^2 + (y - yTarget).^2 + (z - zTarget).^2);

end