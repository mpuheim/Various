%  Frekvencna analyza

global sys a b c d;

volba = menu('Analyza vo frekvencnej oblasti','Nyquistova charakteristika',...
            'Bodeho charakteristika','Nicholsova charakteristika');
switch volba
    case 1
        % Nyquistova frekvencna char.
        nyquist(sys)
        title('Nyquistova frekvencna charakteristika')
    case 2
        % Bodeho frekvencna char.
        bode(sys)
        title('Bodeho frekvencna charakteristika')
    case 3
        % Nicholsova frekvencna char.
        nichols(sys)
        title('Nicholsova frekvencna charakteristika')
end