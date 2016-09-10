function potentialField = gen_potential_function_3D(goalPos, sizeCoords, sphereObs, cubeObs)

xTarget = goalPos(1);
yTarget = goalPos(2);
zTarget = goalPos(3);

xSize = sizeCoords(1);
ySize = sizeCoords(2);
zSize = sizeCoords(3);

repulseField   = gen_repulsive_field_3D(xSize,ySize, zSize, sphereObs,cubeObs);
attractField   = gen_attractive_field_3D(xTarget,yTarget, zTarget, xSize, ySize, zSize);
potentialField = repulseField + attractField;

end