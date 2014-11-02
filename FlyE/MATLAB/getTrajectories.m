function [xs,ys,zs] = getTrajectories(in)
xs = zeros2nans(in(:,:,1,1));
ys = zeros2nans(in(:,:,1,2));
zs = zeros2nans(in(:,:,1,3));