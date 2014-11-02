function [s, c, i] = getSCI(filename)
s = h5read(filename, '/Succeeded');
c = h5read(filename, '/Collided');
i = h5read(filename, '/Ionised');