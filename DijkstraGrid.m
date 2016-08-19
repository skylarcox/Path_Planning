function [route,numExpanded] = DijkstraGrid_r1 (input_map, start_coords, dest_coords)
% Run Dijkstra's algorithm on a grid.
% Inputs : 
%   input_map : a logical array where the freespace cells are false or 0 and
%   the obstacles are true or 1
%   start_coords and dest_coords : Coordinates of the start and end cell
%   respectively, the first entry is the row and the second the column.
% Output :
%    route : An array containing the linear indices of the cells along the
%    shortest route from start to dest or an empty array if there is no
%    route. This is a single dimensional vector
%    numExpanded: Remember to also return the total number of nodes
%    expanded during your search. Do not count the goal node as an expanded node.


% set up color map for display
% 1 - white - clear cell
% 2 - black - obstacle
% 3 - red = visited
% 4 - blue  - on list
% 5 - green - start
% 6 - yellow - destination

cmap = [1 1 1; ...
        0 0 0; ...
        1 0 0; ...
        0 0 1; ...
        0 1 0; ...
        1 1 0; ...
	0.5 0.5 0.5];

colormap(cmap);

% variable to control if the map is being visualized on every
% iteration
drawMapEveryTime = true;

[nrows, ncols] = size(input_map);

totalElms = nrows * ncols;

% map - a table that keeps track of the state of each grid cell
map = zeros(nrows,ncols);

map(~input_map) = 1;   % Mark free cells
map(input_map)  = 2;   % Mark obstacle cells

% Generate linear indices of start and dest nodes
start_node = sub2ind(size(map), start_coords(1), start_coords(2));
dest_node  = sub2ind(size(map), dest_coords(1),  dest_coords(2));

map(start_node) = 5;
map(dest_node)  = 6;

% Initialize distance array
distanceFromStart = Inf(nrows,ncols);

% For each grid cell this array holds the index of its parent
parent = zeros(nrows,ncols);

distanceFromStart(start_node) = 0;
trueDist = distanceFromStart;
% keep track of number of nodes expanded 
numExpanded = 0;

% Main Loop
while true
    
    % Draw current map
    map(start_node) = 5;
    map(dest_node) = 6;
    
    % make drawMapEveryTime = true if you want to see how the 
    % nodes are expanded on the grid. 
    if (drawMapEveryTime)
        image(1.5, 1.5, map);
        grid on;
        axis image;
        drawnow;
    end
    
    % Find the node with the minimum distance
    [min_dist, current] = min(distanceFromStart(:));
    
    if ((current == dest_node) || isinf(min_dist))
        break;
    end;
    
    % Update map
    %map(current) = 3;         % mark current node as visited
    %distanceFromStart(current) = Inf; % remove this node from further consideration
    
    % Compute row, column coordinates of current node
    [i, j] = ind2sub(size(distanceFromStart), current);
    
   % ********************************************************************* 
    % YOUR CODE BETWEEN THESE LINES OF STARS
    
    % Visit each neighbor of the current node and update the map, distances
    % and parent tables appropriately.
    neighborNodes = [current - nrows, current - 1, current + 1, current + nrows];
    
    [x, y] = ind2sub(size(distanceFromStart), neighborNodes);
    
    dRows = abs(x - i);
    dCols = abs(y - j);
    magDist = sqrt(dRows.^2 + dCols.^2);
    nonNeis = find(magDist > 1);
    if ~isempty(nonNeis)
        neighborNodes(nonNeis) = [];
    end
    
    neighborNodes(neighborNodes < 1) = [];
    neighborNodes(neighborNodes > totalElms) = [];
    numNeighbors = length(neighborNodes);
    
    for iNode = 1:numNeighbors
        currNei = neighborNodes(iNode);
        if ~(map(currNei) == 3 || map(currNei) == 2 || map(currNei) == 5)
            if ~(map(currNei) == 3 || map(currNei) == 2 || map(currNei) == 5)
                map(currNei) = 4;
            end
            
            tentDistVal = distanceFromStart(current) + 1;
            
            if tentDistVal < distanceFromStart(currNei) 
                parent(currNei) = current;
            end
            
            distanceFromStart(currNei) = min(distanceFromStart(currNei),tentDistVal);
            trueDist(currNei) = min(distanceFromStart(currNei),tentDistVal);
        end
    end
    
    map(current) = 3;         % mark current node as visited
    distanceFromStart(current) = Inf; % remove this node from further consideration
    numExpanded = numExpanded + 1;
    
    %*********************************************************************

end

%% Construct route from start to dest by following the parent links
if (isinf(distanceFromStart(dest_node)))
    route = [];
else
    route = [dest_node];
    
    while (parent(route(1)) ~= 0)
        route = [parent(route(1)), route];
    end
    
        % Snippet of code used to visualize the map and the path
    for k = 2:length(route) - 1        
        map(route(k)) = 7;
        pause(0.1);
        image(1.5, 1.5, map);
        grid on;
        axis image;
    end
end

end
