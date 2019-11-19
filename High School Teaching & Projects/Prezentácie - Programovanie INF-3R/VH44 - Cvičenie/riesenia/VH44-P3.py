sucet_znamok=0
sucet_vah=0

n=int(input("Zadaj N:"))
for i in range(n):
    znamka = int(input("Zadaj "+str(i+1)+". znamku: "))
    vaha = int(input("Zadaj "+str(i+1)+". vahu: "))
    sucet_znamok = sucet_znamok + znamka*vaha
    sucet_vah = sucet_vah + vaha

print("Priemer:",sucet_znamok/sucet_vah)
