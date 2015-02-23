function [s, c, i, r] = importHDF5tree(filename)
s{1} = h5read(filename, '/Succeeded/data');
s{2} = h5read(filename, '/Succeeded/neutralTimes');
s{3} = h5read(filename, '/Succeeded/ks');

c{1} = h5read(filename, '/Collided/data');
c{2} = h5read(filename, '/Collided/neutralTimes');
c{3} = h5read(filename, '/Collided/ks');

i{1} = h5read(filename, '/Ionised/data');
i{2} = h5read(filename, '/Ionised/neutralTimes');
i{3} = h5read(filename, '/Ionised/ks');

r{1} = h5read(filename, '/Remaining/data');
r{2} = h5read(filename, '/Remaining/neutralTimes');
r{3} = h5read(filename, '/Remaining/ks');