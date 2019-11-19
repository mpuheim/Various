%  Program pre riesenie LDR II. radu.
%  (d2y/dt^2)-7*(dy/dt)+10*y=20*(t^2)-28*t+14
%  Casovy interval [T1..T2] je urceny uzivatelom.

%  Urcenie casoveho intervalu
T(2)=input('Zadaj konecnu hodnotu casoveho intervalu:');
T(1)=0;
%  Urcenie pociatocnych podmienok
PP(2)=0;
PP(1)=0;
%  Kontrolny vypis
fprintf('Pocitam diferencialnu rovnicu (d2y/dt^2)-7*(dy/dt)+10*y=20*(t^2)-28*t+14,\n')
fprintf('na casovom intervale t=[%d..%d], pre pociatocne podmienky t=0, y(t)=0, dy(t)/dt=0.\n',T(1),T(2))

%  Riesenie pomocou funkcie ode45
[t,y]=ode45('dy',T,PP);
%  Analyticke riesenie
d=(2/3).*exp(5.*t)-(5/3).*exp(2.*t)+2.*(t.^2)+1;

%  Odhad chyby
rozdiel=abs(d-y(:,1));   %vektor rozdielov funkcii v danom case
chyba=max(rozdiel);      %najvacsia odchylka numerickeho a analytickeho riesenia
fprintf('Maximalna odchylka = %f\n',chyba)

%  Vykreslenie vyriesenych priebehov:
subplot(3,1,1)
plot(t,y(:,1))
title('Numericke riesenie y(t)'), xlabel('t'),ylabel('yn(t)')
subplot(3,1,2)
plot(t,d,'g--')
title('Analyticke riesenie y(t)'), xlabel('t'),ylabel('ya(t)')
subplot(3,1,3)
plot(t,y(:,1),t,d,'g--')
title('Obe riesenia y(t)'), xlabel('t'),ylabel('yn(t),ya(t)')
return
