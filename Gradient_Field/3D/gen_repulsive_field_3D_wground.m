function repulseField = gen_repulsive_field_3D_wground(xSize,ySize, zSize, cylObs, sphereObs,cubeObs)
%
% Inputs: 
%       xSize - size in row space
%       ySize - size in column space
%       zSize - size in depth space
%       sphereObs - (x,y,z,radius) of sphere
%       cubeObs - The obstacle mapped by two corner points of the 3D
%            polygon given by (xpt1,ypt1,zpt1,xpt2,ypt2,zpt2)
%
%
%

obstacle  = false(xSize, ySize, zSize);
[x, y, z] = meshgrid(1:ySize, 1:xSize, 1:zSize);

% cylObs format is [x,y,z, h, r]: start point[x,y,z], height [h], radius [r]
if ~isempty(cylObs)
    numCylObs = size(cylObs,1);
    for iObs = 1:numCylObs
        cntrPt  = cylObs(iObs,1:3);
        radCyl  = cylObs(iObs,5);
        startZ = cylObs(iObs,3);
        endZ   = startZ + cylObs(iObs,4);
        if endZ > zSize
            endZ = zSize;
        end
        
        obsLog = ((x - cntrPt(1)).^2 + (y - cntrPt(2)).^2 ) < radCyl^2;
        
        if ~isequal(startZ,1)
            obsLog(:,:,1:startZ-1) = false;
        end
        
        if ~isequal(endZ,zSize)
            obsLog(:,:,endZ+1:end) = false;
        end
        obstacle(obsLog) = true;
    end
end


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

% Set the ground as repulsive
obstacle(:,:,1) = true;

d = bwdist(obstacle);

% Rescale and transform distances
d2 = (d/100) + 1;

d0 = 2;
nu = 800;

repulseField = nu*((1./d2 - 1/d0).^2);

repulseField(d2 > d0) = 0;

end



