datDirectory = '/home/jamie/FlyEfiles/';
kVelStore1 = cell(24,1);
kVelStore2 = cell(24,1);
kPosStore1 = cell(24,1);
kPosStore2 = cell(24,1);
ax1 = zeros(24);
ax2 = zeros(24);
figure(1);

pbaspect([1.5 1 1]);

for k=1:24
   disp(k);
   dat1 = h5read(strcat(datDirectory,'kRuns/outFile80.0V',int2str(k),'k.h5'),'/Succeeded');
   dat2 = h5read(strcat(datDirectory,'kRuns/outFile145.0V',int2str(k),'k.h5'),'/Succeeded');
   vels1 = dat1(:,1,2,3);
   vels2 = dat2(:,1,2,3);
   pos1 = dat1(:,1,1,3);
   pos2 = dat2(:,1,1,3);
   kPosStore1{k} = pos1;
   kPosStore2{k} = pos2;
   kVelStore1{k} = vels1;
   kVelStore2{k} = vels2;
   
   figure(1);
   ax1(k) = subplot(6,4,k);
   pbaspect([1.33 1 1])
   scatter(pos2,vels2,1,'.');
   xlabel('z / mm');
   ylabel('v_{z} / mm');
   title(['k = ' int2str(k)]);
   
   %{
   figure(1);
   ax1(k) = subplot(6,4,k);
   hist(vels1,60);
   xlabel('Velocity / ms^{-1}');
   ylabel('Frequency');
   title(strcat(['k = ' int2str(k) ', 80V']));
   
   figure(2);
   ax2(k) = subplot(6,4,k);
   hist(vels2,60);
   xlabel('Velocity / ms^{-1}');
   ylabel('Frequency');
   title(['k = ' int2str(k) ', 145V']);
   %}
end

figure(1);
linkaxes(ax1);
axis([0 1000 0 4500]);

figure(2);
linkaxes(ax2);
axis([0 1000 0 4500]);