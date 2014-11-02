% For importing data from FlyE

dataDirectory = '/home/jamie/FlyEfiles/';

successCounts = zeros(30,1);
heatingFactors = zeros(30,1);
velStore = cell(30,1);
axh = zeros(30);
axp = zeros(30);
axpx = zeros(30);

figure(1);
figure(2);

mh = 1.6737e-27;
kb = 1.3806488e-23;

for v=1:30
    disp(v)
    thisFile = h5read(strcat(dataDirectory,'outFile',int2str(5*(v+1)),'.h5'), '/Succeeded');
    vels = thisFile(:,2,2,3);
    velStore{v} = vels;
    
    %{
    figure(1);
    axh(v) = subplot(5,6,v);
    hist(vels,60)
    xlabel('Velocity/ms^{-1}');
    ylabel('Frequency');
    title(strcat(int2str(5*(v+1)),'V')); 
    %}
    
    figure(1);
    axp(v) = subplot(5,6,v);
    scatter(thisFile(:,1,1,3),thisFile(:,1,2,3),1,'.');
    xlabel('z / mm');
    ylabel('v_{z} / ms^{-1}');
    title(strcat(int2str(5*(v+1)),'V'));
    
    figure(2);
    axpx(v) = subplot(5,6,v);
    scatter(thisFile(:,1,1,1),thisFile(:,1,2,1),1,'.');
    xlabel('x / mm');
    ylabel('v_{x} / ms^{-1}');
    title(strcat(int2str(5*(v+1)),'V'));
    
    %{
    successCounts(v) = size(thisFile,1);
    
    Zscores = zscore(vels);
    vels(find(abs(Zscores)>2.5)) = []; % Discard outliers
    heatingFactors(v) = std(vels,1)^2 * mh * sqrt(2 * log(2)) / kb;
    %}
end

%{
figure(1);
linkaxes(axh);
axis([0 1000 0 4200]);
%}

figure(1);
linkaxes(axp);
axis([22 88 -167.4052 167.4052]);

figure(2);
linkaxes(axpx);
axis([5 47 -167.4052 167.4052]);