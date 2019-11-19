from random import randint

pocetnosti = [0,0,0,0,0,0]

for i in range(1000):
    vysledok = randint(0,5)
    pocetnosti[vysledok] = pocetnosti[vysledok]+1



print("[#1#, #2#, #3#, #4#, #5#, #6#]")
print(pocetnosti)

