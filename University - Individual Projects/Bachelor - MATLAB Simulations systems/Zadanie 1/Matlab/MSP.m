function I=MSP(U,R)
% MSP - Metoda sluckovych prudov.
% I=MSP(U,R)
% I - vetvove napatia I(1) az I(5).
% U - napatia U(1) a U(2).
% R - odpory R(1) az R(5).
% Funkcia vypocita hodnoty prudov vo vetvach obvodu podla prilozenej schemy metodou
% sluckovych prudov.

% Zostavenie matic pre rovnicu A*Is=B, kde Is su sluckove prudy:
A=[ R(1),       -R(1)     ,        0           ;
   -R(1),   R(1)+R(2)+R(3),       -R(3)        ;
    0   ,       -R(3)     ,   R(3)+R(4)+R(5)   ] ;

B=[-U(1);
    0   ;
   -U(2)] ;

% Vypocet sluckovych prudov:
Is=A\B;

% Vypocet vetvovych prudov:
I(1)=Is(1);
I(2)=Is(1)-Is(2);
I(3)=Is(2);
I(4)=Is(2)-Is(3);
I(5)=Is(3);

return