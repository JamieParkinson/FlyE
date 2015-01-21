function [nonzs, indices] = lastNonZeros(in)
nonzs = zeros(size(in,1),1);
indices = zeros(size(in,1),1);

for p=1:size(in,1)
   idx = find(in(p,:),1,'last');
   indices(p) = idx;
   nonzs(p) = in(p,idx); 
end