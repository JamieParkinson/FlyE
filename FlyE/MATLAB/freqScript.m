datDirectory = '/home/jamie/FlyEfiles/';
nextDir = 'kRuns10k/';
fs = zeros(1,24);

for k=1:24
    k
    [s, c, i] = importHDF5tree([datDirectory nextDir int2str(k) 'k.h5']);
    [s,~,~] = getTrajectories(s);
    [c,~,~] = getTrajectories(c);
    [i,~,~] = getTrajectories(i);
    
    s = [s; c]; % Concat successes and collisions
    fs(k) = getFreq(s);
end
