import tkinter
canvas = tkinter.Canvas(width=200,height=200)
canvas.pack()

hlava = canvas.create_oval(90,10,110,30,fill="yellow")
rukaL = canvas.create_line(100,40,50,20,fill="yellow",width=5)
rukaP = canvas.create_line(100,40,150,20,fill="yellow",width=5)
nohaL = canvas.create_line(90,90,90,140,fill="blue",width=5)
nohaP = canvas.create_line(110,90,110,140,fill="blue",width=5)
telo  = canvas.create_oval(80,30,120,100,fill="blue")

while True:
    x1,y1,x2,y2 = canvas.coords(rukaL)
    canvas.coords(rukaL,x1,y1,x2,y2+40)
    x1,y1,x2,y2 = canvas.coords(rukaP)
    canvas.coords(rukaP,x1,y1,x2,y2+40)
    canvas.update()
    canvas.after(500)
    
    x1,y1,x2,y2 = canvas.coords(rukaL)
    canvas.coords(rukaL,x1,y1,x2,y2-40)
    x1,y1,x2,y2 = canvas.coords(rukaP)
    canvas.coords(rukaP,x1,y1,x2,y2-40)
    canvas.update()
    canvas.after(500)
