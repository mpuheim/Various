%  Zadefinovanie systému
n1=[5 6];
d1=[1 2];
n2=[3 1];
d2=[1 1];
n3=[7 1];
d3=[5 1];
n4=[2 1];
d4=[7 1];
n5=[1];
d5=[3 1];
n6=[7];
d6=[1 0];
n7=[8];
d7=[1 0];

nblocks=7;
blkbuild;
q=[2  1 -6;
   3  2  5;
   4  3 -7;
   5  1 -6;
   6  5  0;
   7  4  0]
input=1;
output=4;

[Ad,Bd,Cd,Dd]=connect(a,b,c,d,q,input,output);

%  Výpis v stavovom priestore a vo forme transfer function
printsys(Ad,Bd,Cd,Dd)
[num,den]=ss2tf(Ad,Bd,Cd,Dd,1);
system=tf(num,den)

%  Vykreslenie priebehu v Matlabe
pause
[y,t]=step(system,time);
plot(t,y,'r-')
title('Odozva systému na jednotkový skok'),xlabel('time'),ylabel('output')

%  Vykreslenie priebehu v Matlabe aj Simulinku
pause
plot(t,y,'r-',time,out,'b-.')
title('Porovnanie priebehov y z Matlabu a Simulinku'),xlabel('time'),ylabel('output')

%  Urèenie maximálnej odchýlky
max_chyba=abs(max(out-y))