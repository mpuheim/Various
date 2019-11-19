function xder=dy(t,x)
%  Zapis danej DR II. radu na dve DR I. radu.
xder=[x(2);7.*x(2)-10.*x(1)+20.*(t^2)-28.*t+14];
return