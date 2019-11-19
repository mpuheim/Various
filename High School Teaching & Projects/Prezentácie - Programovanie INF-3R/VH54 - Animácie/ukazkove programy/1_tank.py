import tkinter
canvas = tkinter.Canvas(width=500,height=200)
canvas.pack()

podvozok  = canvas.create_rectangle(10,190,110,170,fill="brown")
karoseria = canvas.create_rectangle(10,170,110,140,fill="green")
veza      = canvas.create_rectangle(50,140,90,120,fill="green")
hlaven    = canvas.create_rectangle(90,125,150,130,fill="green")
strela    = canvas.create_rectangle(150,125,160,130,fill="black")

for i in range(400):        # 400 krat po sebe
    canvas.move(strela,1,0) # pohni strelou v smere osi x o 1 pixel
    canvas.update()         # aktualizuj kresliacu plochu
    canvas.after(10)        # cakaj 10 milisekund

canvas.delete(strela)
    
while True:
    canvas.move(podvozok,1,0)
    canvas.move(karoseria,1,0)
    canvas.move(veza,1,0)
    canvas.move(hlaven,1,0)
    canvas.update()
    canvas.after(10)
