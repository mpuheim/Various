import tkinter

def klik(event):
    global x,y
    x=event.x
    y=event.y
    #bodka = canvas.create_oval(x-size/2,y-size/2,x+size/2,y+size/2,fill=color,width=0)

def tahanie(event):
    global x,y
    ciara = canvas.create_line(x,y,event.x,event.y,width=size,fill=color)
    #bodka = canvas.create_oval(x-size/2,y-size/2,x+size/2,y+size/2,fill=color,width=0)
    x=event.x
    y=event.y

def klavesnica(event):
    global size, color
    if event.keysym == "plus":
        size = size * 2
    elif event.keysym == "minus":
        size = size // 2
    elif event.keysym == "w":
        color = "white"
    elif event.keysym == "b":
        color = "black"

canvas = tkinter.Canvas(width=800,height=800,bg="white")
canvas.pack()
x=0
y=0
size=5
color="black"
canvas.bind('<Button-1>', klik)
canvas.bind('<B1-Motion>', tahanie)
canvas.bind_all('<Key>', klavesnica)
