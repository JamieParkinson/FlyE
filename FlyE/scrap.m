k6 = h5read([datDirectory 'k6FullDatColl.h5'],'/Collided');
k6 = k6(:,:,1,1);
lengths = sum(k6~=0,2);
k6long = k6(lengths >= 400,1:400);

centre = mean(mean(k6long,1));
rms = sqrt(mean(k6long.^2));
plot(rms)