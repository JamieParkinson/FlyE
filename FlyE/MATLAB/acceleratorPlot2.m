function acceleratorPlot2(filename, n2plot)
[s, ~, ~] = importHDF5tree(filename);

[sx,sy,sz] = getTrajectories(s{1});
ntimes = s{2};
ntimes = ntimes + 1;
events = nan(length(ntimes),3);

nInFile = length(sz(:,2));
if (n2plot == -1 || n2plot > nInFile)
    n2plot = nInFile;
end

afterx = nan(size(sx));
aftery = nan(size(sy));
afterz = nan(size(sz));

for p=(find(ntimes))';
    if (p > n2plot) break; end
    
    afterx(p,ntimes(p):end) = sx(p,ntimes(p):end);
    aftery(p,ntimes(p):end) = sy(p,ntimes(p):end);
    afterz(p,ntimes(p):end) = sz(p,ntimes(p):end);
    
    sx(p,(ntimes(p)+1):end) = nan;
    sy(p,(ntimes(p)+1):end) = nan;
    sz(p,(ntimes(p)+1):end) = nan;
    
    events(p,1:3) = [sx(p,ntimes(p)) sy(p,ntimes(p)) sz(p,ntimes(p))];
end

[cyx,cyy,cyz] = cylinder(26);
cyz(2,:) = 198;
cyx = cyx + 26;
cyy = cyy + 26;

greys = cat(1, [0 0 0], repmat(rand(100,1),1,3));
set(0,'DefaultAxesColorOrder',greys);

hold on
plot3(sz(1:n2plot,:)',sx(1:n2plot,:)',sy(1:n2plot,:)','LineWidth',0.7);
plot3(afterz(1:n2plot,:)',afterx(1:n2plot,:)',aftery(1:n2plot,:)','LineWidth',0.7,'Color','red');
scatter3(events(:,3), events(:,1), events(:,2), 80, 'blue', '*');
cyl = surf(cyz,cyx,cyy,'EdgeColor','none','FaceColor','black');
alpha(cyl,0.05)
hold off

pbaspect([4 1 1])
campos([1.0215e3 -0.4624e3 -0.3633e3]);