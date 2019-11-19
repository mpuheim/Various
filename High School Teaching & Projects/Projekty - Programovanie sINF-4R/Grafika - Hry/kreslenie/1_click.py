import tkinter
canvas = tkinter.Canvas(width=800,height=800)
canvas.pack()

canvas.bind('<Button-1>', print)
