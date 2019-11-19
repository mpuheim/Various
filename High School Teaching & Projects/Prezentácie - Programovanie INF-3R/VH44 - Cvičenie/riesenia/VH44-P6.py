print("Zadaj heslo, ktoré obsahuje aspoň jedno:")
print("- velke pismeno")
print("- male pismeno")
print("- cislicu")
heslo = input("Heslo: ")

pocet_cislic   = 0
pocet_velkych = 0
pocet_malych  = 0

for pismeno in heslo:
    if pismeno in "123456789":
        pocet_cislic = pocet_cislic+1
    elif pismeno == pismeno.upper():
        pocet_velkych = pocet_velkych+1
    elif pismeno == pismeno.lower():
        pocet_malych = pocet_malych+1 

if pocet_cislic > 0 and pocet_velkych > 0 and pocet_malych > 0:
    print("Heslo je v poriadku")
else:
    print("Chyba! Heslo musi obsahovat velke a male pismena a cisla!")
