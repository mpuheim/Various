from random import randint

nahodne=randint(1,10)
print("Myslim si cislo od 1 do 10")

tip = None
while tip != nahodne:
    tip=int(input("Ake cislo si myslim?: "))
    if tip < nahodne:
        print("Smola, moje cislo je vacsie.")
    elif tip > nahodne:
        print("Smola, moje cislo je mensie.")
        
print("Super, uhadol si! Myslel som si cislo",nahodne,":)")

