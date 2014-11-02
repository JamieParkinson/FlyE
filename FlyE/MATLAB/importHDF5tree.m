function [s, c, i] = importHDF5tree(filename)
s{1} = h5read(filename, '/Succeeded/data');
s{2} = h5read(filename, '/Succeeded/neutralTimes');

c{1} = h5read(filename, '/Collided/data');
c{2} = h5read(filename, '/Collided/neutralTimes');

i{1} = h5read(filename, '/Ionised/data');
i{2} = h5read(filename, '/Ionised/neutralTimes');