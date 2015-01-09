dataDirectory = '/home/jamie/FlyEfiles/ExponentialVoltageRuns/'
fileSpeedsData = zeros(56,2);

files = dir([dataDirectory '*.h5']);
i = 1;

for file = files'
    file.name
    v = str2num(cell2mat(regexp(file.name,'[0-9]*(?=\.h5)','match')))
    [succeeded, ~, ~] = importHDF5tree([dataDirectory file.name]);
    succeeded = succeeded{1};
    
    meanFinalSpeed = mean(lastNonZeros(succeeded(:,:,2,3)))
    finalSpeedsData(i, :) = [v meanFinalSpeed];
    i = i + 1;
end
