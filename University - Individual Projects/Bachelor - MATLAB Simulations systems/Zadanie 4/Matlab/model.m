% Hlavna cast programu Fyzikalno-dynamickeho modelu.

global sys a b c d;

sys=vstup

volba = 7;

while volba ~= 5
    volba = menu ('Hlavne menu','Konverzia modelov','Casove charakteristiky',...
                 'Frekvencne charakteristiky','Urcenie stability','Koniec');
    clc; 
    switch volba
        case 1
            konverzia(a,b,c,d,sys);
            pause
        case 2
            casova;
        case 3
            frekvencna;
        case 4
            stabilita;
            pause
    end
    clc;
end