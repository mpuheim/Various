%  Casova analyza

global sys a b c d;

volba = menu('Analyza v casovej oblasti','Prechodova charakteristika',...
            'Impulzna charakteristika','Odozva na lubovolny signal');
switch volba
    case 1
        % Prechodova char.
        step(sys)
        title('Prechodova casova charakteristika')
    case 2
        % Impulzna char.
        impulse(sys)
        title('Impulzna casova charakteristika')
    case 3
        % Odozva na lubovolny signal
        odozva;
end