
%
close all
clear all
%% Generate some points
startPos = [10, 157,12];
goalPos = [375, 218,125];
maxIter = 3000;

nrows = 400;
ncols = 400;
ndep = 200;

%obstacle = false(nrows, ncols, ndep);

%[x, y] = meshgrid (1:ncols, 1:nrows, 1:ndep);

%% Generate some obstacle
sphereRad = 40;
sphereObs = [200,200,50,sphereRad];
cubeObs = '';
% cubeObs = [10,200,20,20, 250,50;...
%     300,300,5,320,320,40];
% sphereObs = [1,1,1,10;...
%     150,150,15,8];

potentialField = gen_potential_function_3D(goalPos, [nrows, ncols, ndep], sphereObs, cubeObs);
route = return_route_3D(potentialField, startPos, goalPos, maxIter);

%[x,y,z] = sphere(sphereRad);
[sx, sy, sz] = sphere(sphereRad);
figure;
axis equal
scale = sphereRad;
sx = scale*sx;
sy = scale*sy;
sz = scale*(sz);
hold on;
p = surf(sx+sphereObs(1), sy+sphereObs(2), sz+sphereObs(3));
p.FaceColor = 'red';
p.EdgeColor = 'none';
p.FaceLighting = 'phong';
% p.XData = sphereObs(1);
% p.YData = sphereObs(2);
% p.ZData = sphereObs(3);
    
hold on
plot3(route(:,1),route(:,2),route(:,3));
grid on
%potentialField = gen_potential_function(xTarget,yTarget, xSize, ySize, circObs, rectObs)

%% Display 2D configuration space

% figure(3);
% imshow(~obstacle);
% 
% hold on;
% plot (goal(1), goal(2), 'r.', 'MarkerSize', 25);
% hold off;
% 
% axis ([0 ncols 0 nrows]);
% axis xy;
% axis on;
% 
% xlabel ('x');
% ylabel ('y');
% 
% title ('Configuration Space');

% %% Combine terms
% 
% %f = attractive + repulsive;
% f = potentialField;
% 
% figure(4);
% m = mesh (f);
% m.FaceLighting = 'phong';
% axis equal;
% 
% title ('Total Potential');
% 
% %% Plan route
% 
% %start = [300,10];
% %route1 = GradientBasedPlanner_r1 (f, start, goal, 1000);
% route = GradientBasedPlanner (f, start, goalPos, 2000);
% %% Plot the energy surface
% 
% figure(5);
% m = mesh (f);
% axis equal;
% 
% %% Plot ball sliding down hill
% 
% [sx, sy, sz] = sphere(20);
% 
% scale = 20;
% sx = scale*sx;
% sy = scale*sy;
% sz = scale*(sz+1);
% 
% hold on;
% p = mesh(sx, sy, sz);
% p.FaceColor = 'red';
% p.EdgeColor = 'none';
% p.FaceLighting = 'phong';
% hold off;
% 
% for i = 1:size(route,1)
%     P = round(route(i,:));
%     z = f(P(2), P(1));
%     
%     p.XData = sx + P(1);
%     p.YData = sy + P(2);
%     p.ZData = sz + f(P(2), P(1));
%     
%     drawnow;
%     
%     drawnow;
%     
% end
% 
% %% quiver plot
% [gx, gy] = gradient (-f);
% skip = 20;
% 
% figure(6);
% 
% xidx = 1:skip:ncols;
% yidx = 1:skip:nrows;
% 
% quiver (x(yidx,xidx), y(yidx,xidx), gx(yidx,xidx), gy(yidx,xidx), 0.4);
% 
% axis ([1 ncols 1 nrows]);
% 
% hold on;
% 
% ps = plot(start(1), start(2), 'r.', 'MarkerSize', 30);
% pg = plot(goalPos(1), goalPos(2), 'g.', 'MarkerSize', 30);
% p3 = plot (route(:,1), route(:,2), 'r', 'LineWidth', 2);
