function I=MUN(U,R)
% MUN - Metoda uzlovych napati.
% I=MUN(U,R)
% I - vetvove napatia I(1) az I(5).
% U - napatia U(1) a U(2).
% R - odpory R(1) az R(5).
% Funkcia vypocita hodnoty prudov vo vetvach obvodu podla prilozenej schemy metodou
% uzlovych napati.

% Zostavenie matic pre rovnicu A*Uu=B, kde Uu su uzlove napatia:
A=1/R(2)+1/R(3)+1/(R(4)+R(5));

B=U(2)/(R(4)+R(5))-U(1)/R(2);

% Vypocet uzlovych napati:
Uu=A\B;

% Vypocet vetvovych prudov:
I(2)=-U(1)/R(1);
I(4)=Uu/R(3);
I(5)=(Uu-U(2))/(R(4)+R(5));
I(3)=I(4)+I(5);
I(1)=I(2)+I(3);

return