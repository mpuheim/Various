import tkinter

def draw_circle_callback(event):
    x=event.x
    y=event.y
    canvas.create_oval(x-2,y-2,x+2,y+2,fill="red")

canvas = tkinter.Canvas(width=800,height=800)
canvas.pack()
canvas.bind('<Button-1>', draw_circle_callback)
