dir = '/home/jamie/FlyEfiles/voltageEfficiency/';
result = zeros(21,2);
axes = zeros(1,10);

for v = 25:5:75
    v
    fileName = [dir int2str(v) '.000000.h5'];
    dat = h5read(fileName, '/Succeeded/maxFields');
    
    axes((v-20)/5) = subplot(3,4,(v-20)/5);
    histDat = hist(dat,50);
    
    maxField = histDat(max(peakfinder(histDat)))
    result((v-20)/5, :) = [maxField 100 * (length(dat)/100000)];
    
    
end
linkaxes(fliplr(axes))