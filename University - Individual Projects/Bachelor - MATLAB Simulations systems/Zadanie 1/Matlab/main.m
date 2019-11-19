% Program na urcenie prudov vo vetvach obvodu podla prilozenej schemy
% metodou sluckovych prudov (MSP) a metodou uzlovych napati (MUN).
% 
% Vstupy  - hodnoty odporov rezistorov a napati na zdrojoch.
% Vystupy - hodnoty realnych prudov vo vetvach obvodu.

% Vstupy napati a odporov
clc %vymaz obrazovku
U=input('Zadaj hodnoty napati na zdrojoch vo Voltoch v tvare "[U1 U2]":\n');
R=input('Zadaj hodnoty odporov v Ohmoch v tvare "[R1 R2 R3 R4 R5]":\n');
fprintf('\n----------------------------------------------------------------\n')

% Metoda sluckovych prudov (funkcia MSP.m)
I=MSP(U,R);
fprintf('\nPrudy vo vetvach vypocitane podla MSP su:')
I=I

% Skuska spravnosti pomocou 1. Kirchhofoveho zakona:
if (-I(1)+I(2)+I(3)<1e-6)&&(-I(3)+I(4)+I(5)<1e-6)&&(I(1)-I(2)-I(4)-I(5)<1e-6)
    fprintf('Vypocitane prudy vyhovuju 1.Kirchhofovmu zakonu.\n')
else
    fprintf('CHYBA!!! - Vypocitane prudy NEVYHOVUJU 1.Kirchhofovmu zakonu!\n')
end

fprintf('\n----------------------------------------------------------------\n')

% Metoda uzlovych napati (funkcia MUN.m)
I=MUN(U,R);
fprintf('\nPrudy vo vetvach vypocitane podla MUN su:')
I=I

% Skuska spravnosti pomocou 1. Kirchhofoveho zakona:
if (-I(1)+I(2)+I(3)<1e-6)&&(-I(3)+I(4)+I(5)<1e-6)&&(I(1)-I(2)-I(4)-I(5)<1e-6)
    fprintf('Vypocitane prudy vyhovuju 1.Kirchhofovmu zakonu.\n')
else
    fprintf('CHYBA!!! - Vypocitane prudy NEVYHOVUJU 1.Kirchhofovmu zakonu!\n')
end

fprintf('\n----------------------------------------------------------------\n')

return