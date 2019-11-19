function []=konverzia(a,b,c,d,sys)

% Konverzia modelu zo stavoveho priestoru do prechodovej funkcie a naopak

volba = menu('Konverzia','Zo SS do Prechodovej funkcie','Z prechodovej funkcie do SS');

switch volba
    case 1
        disp('Konverzia modelu zo stavoveho priestoru do prechodovej funkcie pomocou "ss2tf"')
        [num,den]=ss2tf(a,b,c,d,1);
        tf(num,den)
    case 2
        disp('Konverzia modelu z prechodovej funkcie do stavoveho priestoru pomocou "tf2ss"')
        [num,den]=tfdata(sys,'v');
        [a,b,c,d]=tf2ss(num,den)
end