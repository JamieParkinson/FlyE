function f = getFreq(trajectories)
lengths = sum(~isnan(trajectories),2); %lengths of each trajectory
maxLength = size(trajectories,2);

longTrajectories = trajectories(lengths > 0.66*maxLength,1:floor(0.66*maxLength)); %discard short trajectories
centre = nanmedian(nanmedian(longTrajectories));
longTrajectories = abs(longTrajectories - centre);
meanTraj = double(mean(longTrajectories));
[~, minxs] = findpeaks(meanTraj,'MinPeakHeight',max(meanTraj)/4); %gets the peak locations of the mean trajectory

plot(max(meanTraj) - meanTraj);
minxs

f = 0.5*1e6/mean(diff(minxs));