% Vyhodnotenie stability

global sys

[num,den]=tfdata(sys,'v');
r=roots(den);
max=size(r);

test = 1;

for i= 1:max(:,1)
    if r(i) >0;
        test=0;
    end
end

if test == 0
    disp('System je nestabilny');
else
    disp('System je stabilny, alebo na hranici stability')
end