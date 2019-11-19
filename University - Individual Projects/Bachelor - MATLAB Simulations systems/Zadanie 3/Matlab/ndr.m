%  Program pre riesenie nelinearnej diferencialnej rovnice
%  (dy/dt)=-ty+4t/y
%  na casovom intervale t=[0..4]
%  s pociatocnou podmienkou y(0)=1

%  Urcenie casoveho intervalu
T=0:0.1:4;
%  Urcenie pociatocnych podmienok
PP=1;
%  Kontrolny vypis
fprintf('Pocitam diferencialnu rovnicu (dy/dt)=-ty+4t/y,\n')
fprintf('na casovom intervale t=[%d..%d], pre pociatocne podmienky t=0, y(t)=1.\n',T(1),T(length(T)))

%  Riesenie pomocou funkcie ode45
[t1,y]=ode45('dy',T,PP);
%  Riesene metodou Runge-Kutta
[t2,d]=rungekutta(T);

%  Vykreslenie vyriesenych priebehov:
subplot(1,3,1)
plot(t1,y(:,1))
title('ode45 y(t)'), xlabel('t'),ylabel('yn(t)')
subplot(1,3,2)
plot(t2,d,'g--')
title('Runge-Kutta riesenie riesenie y(t)'), xlabel('t'),ylabel('ya(t)')
subplot(1,3,3)
plot(t1,y(:,1),t2,d,'g--')
title('Obe riesenia y(t)'), xlabel('t'),ylabel('yn(t),ya(t)')
return
