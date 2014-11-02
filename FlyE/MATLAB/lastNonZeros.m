function [nonzs] = lastNonZeros(in)
nonzs = zeros(size(in,1),1);

for p=1:size(in,1)
   nonzs(p) = in(p,find(in(p,:),1,'last')); 
end