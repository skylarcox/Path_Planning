function repulseField = gen_repulsive_field(xSize,ySize, circObs,rectObs)

obs = false(xSize, ySize);

[x, y] = meshgrid(1:ySize, 1:xSize);

numCircObs = size(circObs,1);
numRectObs = size(rectObs,1);

for iObs = 1:numCircObs
    cntrPt  = circObs(iObs,1:2);
    radCirc = circObs(iObs,3);
    obsLog = ((x - cntrPt(1)).^2 + (y - cntrPt(2)).^2) < radCirc^2;
    obs(obsLog) = true;
end

for iObs = 1:numRectObs
    obs (rectObs(iObs,1):rectObs(iObs,3), rectObs(iObs,2):rectObs(iObs,4)) = true;
end

d = bwdist(obs);

% Rescale and transform distances
d2 = (d/100) + 1;

d0 = 2;
nu = 800;

repulseField = nu*((1./d2 - 1/d0).^2);

repulseField(d2 > d0) = 0;

end



