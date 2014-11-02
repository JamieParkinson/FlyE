histDat = zeros(sum(cellfun('prodofsize',kVelStore2)),2);
numCounter = 1;
for k=1:24
    numInV = size(kVelStore2{k},1);
    histDat(numCounter:numInV+numCounter-1,1) = kVelStore2{k};
    histDat(numCounter:numInV+numCounter-1,2) = k;
    
    numCounter = numCounter + numInV;
end
