function xder=dy(t,x)
%  Zapis danej DR pomocu substitucneho kanonickeho tvaru
xder=-t.*x(1)+4*t./x(1);
return