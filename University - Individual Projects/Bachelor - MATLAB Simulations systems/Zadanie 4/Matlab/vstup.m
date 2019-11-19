function [sys]=vstup()

%  Funkcia pre ur�enie vstupov do syst�mu.

%  Zadefinovanie prenosovej funkcie opisuj�cej LDS v s-oblasti
%  (v polynomil�lnom tvare, v tvare p�ly/nuly) alebo v stavovom
%  priestore pomocou mat�c A, B, C, D.

global sys a b c d;

B = 2  ;%koeficient tlmenia
K = 6   ;%tuhost pruziny
m = 700 ;%hmotnost auta

opakuj=1;
while opakuj==1
    volba = menu ('Vstupy','V polynomialnom tvare', 'Pomocou polov a nul', 'V stavovom priestore');
    switch volba
        case 1
            B %koeficient tlmenia
            K %tuhost pruziny
            m %hmotnost auta
            num=[B K];
            den=[m B K];
            sys=tf(num,den);
            opakuj=0;
        case 2
            nuly = [-K/B]
            poly = [(-B/m+sqrt((B/m)^2-4*K/m))/2 (-B/m-sqrt((B/m)^2-4*K/m))/2]
            zosilnenie = B/m
            sys=zpk(nuly,poly,zosilnenie);
            opakuj=0;
        case 3
            a=[-0.0029, -0.0086; 1, 0];
            b=[1; 0];
            c=[0.0029, 0.0086];
            d=[0];
            sys=ss(a,b,c,d);
            opakuj=0;
    end
end

return