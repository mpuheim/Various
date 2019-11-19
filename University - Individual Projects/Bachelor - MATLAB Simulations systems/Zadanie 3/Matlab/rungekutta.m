function [vt,vy]=rungekutta(T)
%  Vypocet funkcie metodou Runge-Kutta 4. radu
vt=zeros(length(T)-1,1);
vy=zeros(length(T)-1,1);
poc=1; % pocitadlo
h=T(2)-T(1); % krok
t=0;
y=1;
while t<=max(T)
    vt(poc)=t;
    vy(poc)=y;
    K1=h*(-t*y+4*t/y);
    K2=h*(-(t+h/2)*(y+K1/2)+4*(t+h/2)/(y+K1/2));
    K3=h*(-(t+h/2)*(y+K2/2)+4*(t+h/2)/(y+K2/2));
    K4=h*(-(t+h)*(y+K3)+4*(t+h)/(y+K3));
    y=y+(K1+2*K2+2*K3+K4)/6;
    t=t+h;
    poc=poc+1;
end
return