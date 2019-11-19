import tkinter

def klik(event):
    global x,y
    x=event.x
    y=event.y

def tahanie(event):
    global x,y
    ciara = canvas.create_line(x,y,event.x,event.y)
    x=event.x
    y=event.y

canvas = tkinter.Canvas(width=800,height=800)
canvas.pack()
x=0
y=0
canvas.bind('<Button-1>', klik)
canvas.bind('<B1-Motion>', tahanie)
