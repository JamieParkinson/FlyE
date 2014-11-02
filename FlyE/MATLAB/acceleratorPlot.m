function acceleratorPlot(sIn, iIn,n2plot)
[sx,sy,sz] = getTrajectories(sIn);
%[cx,cy,cz] = getTrajectories(cIn);
[ix,iy,iz] = getTrajectories(iIn);

if (n2plot == -1)
    n2plot = length(sz(:,2));
end

[cyx,cyy,cyz] = cylinder(26);
cyz(2,:) = 198;
cyx = cyx + 26;
cyy = cyy + 26;

greys = cat(1, [0 0 0], repmat(rand(100,1),1,3));
set(0,'DefaultAxesColorOrder',greys);

hold on
plot3(sz(1:n2plot,:)',sx(1:n2plot,:)',sy(1:n2plot,:)','LineWidth',0.7);
plot3(iz',ix',iy','LineWidth',2,'Color','red');
cyl = surf(cyz,cyx,cyy,'EdgeColor','none','FaceColor','black');
alpha(cyl,0.05)
hold off

pbaspect([4 1 1])