function out = zeros2nans(in)
out = in;
out(~out) = nan;
