function route = return_route_3D(f, startPos, goalPos, maxIter)
% GradientBasedPlanner : This function plans a path through a 3D
% environment from a start to a destination based on the gradient of the
% function f which is passed in as a 3D array. The two arguments
% start_coords and end_coords denote the coordinates of the start and end
% positions respectively in the array while max_its indicates an upper
% bound on the number of iterations that the system can use before giving
% up.
% The output, route, is an array with 3 columns and n rows where the rows
% correspond to the coordinates of the robot as it moves along the route.
% The first column corresponds to the x coordinate, the second to the y
% coordinate, and the third to teh z coordinate.
toleranceVal = 5;

[gx, gy, gz] = gradient (-f);

pos = startPos;
route = pos;

goodToGo = true;
counter = 0;

while goodToGo
    distToGoal = norm(pos - goalPos);
    if (counter == maxIter || distToGoal < toleranceVal)
        goodToGo = false;
    end
%     dSpace = [gx(round(pos(1)), round(pos(2)), round(pos(3)) ),...
%               gy(round(pos(1)), round(pos(2)), round(pos(3)) ),...
%               gz(round(pos(1)), round(pos(2)), round(pos(3)) )];
    dSpace = [gx(round(pos(2)), round(pos(1)), round(pos(3)) ),...
              gy(round(pos(2)), round(pos(1)), round(pos(3)) ),...
              gz(round(pos(2)), round(pos(1)), round(pos(3)) )];
    
    pos = pos + dSpace/norm(dSpace);
    
    route = [route;pos];
    counter = counter + 1;
end

end
