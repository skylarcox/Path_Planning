%
% TestScript for Assignment 1
%

%% Define a small map
map = false(25);

% Add an obstacle
map (1:5, 6) = true;
map (15:end, 12) = true;
map (5:9, 9) = true;

start_coords = [6, 2];
dest_coords  = [23, 23];

%%
close all;
%[route, numExpanded] = DijkstraGrid (map, start_coords, dest_coords);
% Uncomment following line to run Astar
[route, numExpanded] = AStarGrid (map, start_coords, dest_coords);
