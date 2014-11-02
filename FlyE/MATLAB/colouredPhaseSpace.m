function colouredPhaseSpace(in)
colormap jet
scatter(in(:,1,1,3), in(:,1,2,3), 4, in(:,2,2,3), 'filled');
