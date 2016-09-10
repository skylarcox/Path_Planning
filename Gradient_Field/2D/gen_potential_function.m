function potentialField = gen_potential_function(goalPos, xSize, ySize, circObs, rectObs)

xTarget = goalPos(1);
yTarget = goalPos(2);

repulseField   = gen_repulsive_field(xSize,ySize, circObs,rectObs);
attractField   = gen_attractive_field(xTarget,yTarget, xSize, ySize);
potentialField = repulseField + attractField;

end