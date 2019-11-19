sucet=0

n=int(input("Zadaj N:"))
for i in range(n):
    cislo = int(input("Zadaj "+str(i+1)+". cislo: "))
    sucet = sucet + cislo

print("Priemer:",sucet/n)
