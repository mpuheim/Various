import tkinter, random

def create_star(x,y):
    points=[x,y+8,x+2,y+2,x+8,y,x+2,y-2,x,y-8,x-2,y-2,x-8,y,x-2,y+2]
    star=canvas.create_polygon(points, fill='yellow', width=0)
    return star

def animate_stars(stars):
    for s in stars:
        points = canvas.coords(s)
        if points[0]>0:
            canvas.move(s,-1,0)
        else:
            canvas.move(s,+600,0)

canvas = tkinter.Canvas(width=600,height=600)
canvas.pack()

sky = canvas.create_rectangle(0,0,600,600, fill='black')
stars = []

for i in range(100):
    x = random.randint(0,600)
    y = random.randint(0,600)
    s = create_star(x,y)
    stars.append(s)

while True:
    animate_stars(stars)
    canvas.update()

