% Odozva na lubovolny vstupny signal

global sys;

signal_type=input('Zadajte typ signalu (napr. ''sin''):\n')
ton=input('Zadajte periodu vzorkovania:\n')
time=input('Zadajte celkovu dobu simulacie:\n')
dt=input('Zadajte vzorkovanie casu:\n')

[u,t]=gensig(signal_type,ton,time,dt);
lsim(sys,u,t)
grid;
title('Odozva na lubovolny signal');