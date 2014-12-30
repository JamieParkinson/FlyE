dataDirectory = '/home/jamie/FlyEfiles/'
finalSpeedsData = zeros(10,2);

for v = 50:1:64
    filename = [dataDirectory 'out' int2str(v) '.h5'];
    [succeeded, ~, ~] = importHDF5tree(filename);
    succeeded = succeeded{1};
    
    meanFinalSpeed = mean(lastNonZeros(succeeded(:,:,2,3)))
    finalSpeedsData(v-49, :) = [v meanFinalSpeed];
end
