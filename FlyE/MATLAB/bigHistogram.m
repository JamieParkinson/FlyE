function nToRead = bigHistogram(h5path, phaseVariable, axis, nbins)
    H5_GROUP_PATH = '/Succeeded/data';
    MAX_READ_N = 5e5;
    
    start = [1 2 phaseVariable axis];
    count = [MAX_READ_N Inf Inf Inf];
    stride = [1 2 2 3];
    
    disp('Reading HDF5 file info...');
    info = h5info(h5path, H5_GROUP_PATH);
    nToRead = info.Dataspace.Size(1);
    
    binCentres = zeros(1, nbins);
    totalCounts = zeros(1, nbins);
    
    while start(1) + count(1) <= nToRead
        disp([int2str(start(1)) ' - ' int2str(start(1) + count(1))]);
        
        thisData = h5read(h5path, H5_GROUP_PATH, start, count, stride);
        
        if start(1) == 1
            binCentres = linspace(min(thisData), max(thisData), nbins);
        end
        
        [thisCounts] = hist(thisData, binCentres);
        
        totalCounts = totalCounts + thisCounts;
        
        start(1) = start(1) + MAX_READ_N;
        if (start(1) + count(1) > nToRead && start(1) < nToRead)
            count(1) = nToRead - start(1);
        end
    end
    
    stairs(binCentres, totalCounts)
end