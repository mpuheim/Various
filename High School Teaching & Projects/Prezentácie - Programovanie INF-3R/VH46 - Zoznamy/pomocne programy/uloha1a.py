from random import randint

pocet1 = 0
pocet2 = 0
pocet3 = 0
pocet4 = 0
pocet5 = 0
pocet6 = 0

for i in range(1000):
    vysledok = randint(1,6)
    if vysledok == 1: pocet1 = pocet1 + 1
    if vysledok == 2: pocet2 = pocet2 + 1
    if vysledok == 3: pocet3 = pocet3 + 1
    if vysledok == 4: pocet4 = pocet4 + 1
    if vysledok == 5: pocet5 = pocet5 + 1
    if vysledok == 6: pocet6 = pocet6 + 1

print("#1# #2# #3# #4# #5# #6#")
print(pocet1,pocet2,pocet3,pocet4,pocet5,pocet6)
