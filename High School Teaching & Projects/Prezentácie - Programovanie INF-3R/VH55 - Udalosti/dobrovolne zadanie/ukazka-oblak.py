import tkinter

def animacia(event):
    #pohyb mraku
    for x in range(200):
        canvas.move(oblak,1,0)
        canvas.update()
        canvas.after(10)
    #zmena farby oblohy a oblaku
    canvas.itemconfig(obloha,fill="black",outline="black")
    canvas.itemconfig(oblak,fill="grey",outline="grey")

#hlavny program
canvas=tkinter.Canvas(width=400,height=200)
canvas.pack()
obloha=canvas.create_rectangle(0,0,400,200,fill="blue",outline="blue")
slnko=canvas.create_oval(275,50,325,100,fill="yellow",outline="yellow")
oblak=canvas.create_oval(50,50,150,100,fill="white",outline="white")
canvas.bind_all("<space>",animacia)
