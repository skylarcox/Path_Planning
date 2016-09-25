
%
close all
clear all
%% Generate some points
start = [50, 350];
goal = [375, 235];

nrows = 500;
ncols = 500;

%obstacle = false(nrows, ncols);

%[x, y] = meshgrid (1:ncols, 1:nrows);

%% Generate some obstacle
rectObs = [10,200,20,250; 250,250,300,300; 450,200,500,220];
circObs = [450,150,25;30,30,5];

potentialField = gen_potential_function(goal, nrows, ncols, circObs, rectObs);

%% Show obstacles by themselves
%obstacle   = gen_repulsive_field(nrows,ncols, circObs,rectObs);
obstacle = false(nrows, ncols);
numCircObs = size(circObs,1);
numRectObs = size(rectObs,1); 
for iObs = 1:numCircObs
    cntrPt  = circObs(iObs,1:2);
    radCirc = circObs(iObs,3);
    obsLog = ((x - cntrPt(1)).^2 + (y - cntrPt(2)).^2) < radCirc^2;
    obstacle(obsLog) = true;
end

for iObs = 1:numRectObs
    obstacle (rectObs(iObs,1):rectObs(iObs,3), rectObs(iObs,2):rectObs(iObs,4)) = true;
end
%potentialField = gen_potential_function(xTarget,yTarget, xSize, ySize, circObs, rectObs)

%% Display 2D configuration space

figure(3);
imshow(~obstacle);

hold on;
plot (goal(1), goal(2), 'r.', 'MarkerSize', 25);
hold off;

axis ([0 ncols 0 nrows]);
axis xy;
axis on;

xlabel ('x');
ylabel ('y');

title ('Configuration Space');

%% Combine terms

%f = attractive + repulsive;
f = potentialField;

figure(4);
m = mesh (f);
m.FaceLighting = 'phong';
axis equal;

title ('Total Potential');

%% Plan route

%start = [300,10];
%route1 = GradientBasedPlanner_r1 (f, start, goal, 1000);
route = GradientBasedPlanner (f, start, goal, 2000);
%% Plot the energy surface

figure(5);
m = mesh (f);
axis equal;

%% Plot ball sliding down hill

[sx, sy, sz] = sphere(20);

scale = 20;
sx = scale*sx;
sy = scale*sy;
sz = scale*(sz+1);

hold on;
p = mesh(sx, sy, sz);
p.FaceColor = 'red';
p.EdgeColor = 'none';
p.FaceLighting = 'phong';
hold off;

for i = 1:size(route,1)
    P = round(route(i,:));
    z = f(P(2), P(1));
    
    p.XData = sx + P(1);
    p.YData = sy + P(2);
    p.ZData = sz + f(P(2), P(1));
    
    drawnow;
    
    drawnow;
    
end

%% quiver plot
[gx, gy] = gradient (-f);
skip = 20;

figure(6);

xidx = 1:skip:ncols;
yidx = 1:skip:nrows;

quiver (x(yidx,xidx), y(yidx,xidx), gx(yidx,xidx), gy(yidx,xidx), 0.4);

axis ([1 ncols 1 nrows]);

hold on;

ps = plot(start(1), start(2), 'r.', 'MarkerSize', 30);
pg = plot(goal(1), goal(2), 'g.', 'MarkerSize', 30);
p3 = plot (route(:,1), route(:,2), 'r', 'LineWidth', 2);
