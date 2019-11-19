#Priprava textu na zapis
text="""The Tatra Mountains, Tatras or Tatra, is a mountain range
that forms a natural border between Slovakia and Poland. This is
the highest mountain range in the Carpathian Mountains. The Tatras
should not be confused with the Low Tatras which are located south
of the Tatra Mountains in Slovakia."""

#Otvorenie suboru v rezime zapisu
subor=open("tatry.txt","w")

#Zapis do suboru
subor.write(text)

#Zatvorenie suboru
subor.close()
