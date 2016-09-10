function repulseField = gen_repulsive_field_3D(xSize,ySize, zSize, sphereObs,cubeObs)
%
% Inputs: 
%       xSize - size in row space
%       ySize - size in column space
%       zSize - size in depth space
%       sphereObs - (x,y,z,radius) of sphere
%       cubeObs - The obstacle mapped by two corner pounts of the 3D
%            polygon given by (xpt1,ypt1,zpt1,xpt2,ypt2,zpt2)
%
%
%

obstacle  = false(xSize, ySize, zSize);
[x, y, z] = meshgrid(1:ySize, 1:xSize, 1:zSize);

if ~isempty(sphereObs)
    numSphereObs = size(sphereObs,1);
    for iObs = 1:numSphereObs
        cntrPt  = sphereObs(iObs,1:3);
        radSphere = sphereObs(iObs,4);
        obsLog = ((x - cntrPt(1)).^2 + (y - cntrPt(2)).^2 + (z - cntrPt(3)).^2) < radSphere^2;
        obstacle(obsLog) = true;
    end
end

if ~isempty(cubeObs)
    numCubeObs = size(cubeObs,1);
    for iObs = 1:numCubeObs
        obstacle(cubeObs(iObs,1):cubeObs(iObs,4), cubeObs(iObs,2):cubeObs(iObs,5), cubeObs(iObs,3):cubeObs(iObs,6)) = true;
    end
end

d = bwdist(obstacle);

% Rescale and transform distances
d2 = (d/100) + 1;

d0 = 2;
nu = 800;

repulseField = nu*((1./d2 - 1/d0).^2);

repulseField(d2 > d0) = 0;

end



