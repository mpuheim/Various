import tkinter

#priprava grafickej plochy s prazdnym objektom obrazku
canvas = tkinter.Canvas(background='blue')
canvas.pack()
obr = canvas.create_image(200, 150)

#načítanie zoznamu obrázkov zo súborov
obrazky=[]
for i in range(8):
    nazov = 'vtak/vtak'+str(i)+'.png'
    obrazok = tkinter.PhotoImage(file=nazov)
    obrazky.append(obrazok)

#animacia obrazku
i = 0
while True:
    canvas.itemconfig(obr, image=obrazky[i])
    i = (i + 1) % len(obrazky)
    canvas.update()
    canvas.after(100)
