from tkinter import *
import math, cmath

# inspired by -> http://effbot.org/zone/tkinter-complex-canvas.htm

def rotate(x,y,angle):
    offset = complex(center[0], center[1])
    v = angle * (complex(x, y) - offset) + offset
    return v.real, v.imag

def rotatepolygon(xy,angle):
    newxy = []
    for x, y in xy:
        rx,ry = rotate(x,y,angle)
        newxy.append((rx,ry))
    return newxy

def xy2list(xy):
    newxy = []
    for x, y in xy:
        newxy.append(x)
        newxy.append(y)
    return newxy

def getangle(x,y):
    dx = c.canvasx(x) - center[0]
    dy = c.canvasy(y) - center[1]
    try:
        return complex(dx, dy) / abs(complex(dx, dy))
    except ZeroDivisionError:
        return 0.0 # cannot determine angle
        
def shoot_callback(event):
    global shot
    x1=root.winfo_pointerx() - root.winfo_rootx()
    y1=root.winfo_pointery() - root.winfo_rooty()
    x2=center[0]
    y2=center[1]
    dx=(x1-x2)/100
    dy=(y1-y2)/100
    s=c.create_oval(x2-5,y2-5,x2+5,y2+5,fill="black")
    shot=[s,dx,dy]

# Create canvas and bind events
root=Tk()
c = Canvas(root,width=1000, height=1000)
c.pack()
c.bind("<Button-1>", shoot_callback)
# Draw initial arrow
center = [580, 640]
xy = [(580, 520), (640, 630), (600, 600), (600, 680), (560,680), (560,600), (520,630)]
polygon_item = c.create_polygon(xy)
# Rotate arrow by 90 degrees
cangle = math.radians(90)
cangle = cmath.exp(cangle*1j)
xy = rotatepolygon(xy,cangle)
# Track mouse pointer
shot=None
while True:
    px=root.winfo_pointerx() - root.winfo_rootx()
    py=root.winfo_pointery() - root.winfo_rooty()
    angle = getangle(px,py)         # get mouse pointer angle
    newxy = rotatepolygon(xy,angle) # rotate polygon towards pointer
    newxy = xy2list(newxy)          # convert coords [(x1,y1),(x2,y2)] to list [x1,y1,x2,y2]
    c.coords(polygon_item, *newxy)  # change polygon coordinates using unpacked list
    if shot:
        c.move(shot[0],shot[1],shot[2])
    c.after(10)
    c.update()

