import tkinter, time, math
w=500; h=500; G = 20
#object parameters
sun_mass = 50
sun_x = w/2
sun_y = h/2
earth_mass = 10
earth_x = w/2+50
earth_y = h/2
earth_vx = 0
earth_vy = -5
#create window
window=tkinter.Tk()
canvas=tkinter.Canvas(width=w,height=h)
canvas.pack()
#draw sun
x1=sun_x-sun_mass/2
y1=sun_y-sun_mass/2
x2=sun_x+sun_mass/2
y2=sun_y+sun_mass/2
sun=canvas.create_oval(x1,y1,x2,y2,fill="yellow")
#draw earth
x1=earth_x-earth_mass/2
y1=earth_y-earth_mass/2
x2=earth_x+earth_mass/2
y2=earth_y+earth_mass/2
earth=canvas.create_oval(x1,y1,x2,y2,fill="blue")
#simulation loop
dt = 0.1
while True:
    #compute the distance
    dx = sun_x - earth_x
    dy = sun_y - earth_y
    d = math.sqrt(dx**2 + dy**2)
    #compute the force of attraction
    f = G * earth_mass * sun_mass / (d**2)
    #compute the direction of the force
    theta = math.atan2(dy, dx)
    fx = math.cos(theta) * f
    fy = math.sin(theta) * f
    #update speed (velocity)
    earth_vx += fx / earth_mass * dt
    earth_vy += fy / earth_mass * dt
    #compute position delta
    px = earth_vx * dt
    py = earth_vy * dt
    #move objects
    earth_x += px
    earth_y += py
    #draw objects
    x1=earth_x-earth_mass/2
    y1=earth_y-earth_mass/2
    x2=earth_x+earth_mass/2
    y2=earth_y+earth_mass/2
    canvas.coords(earth,x1,y1,x2,y2)
    canvas.update()
    #pause simulation for delta t
    time.sleep(dt)
    #print(earth_x, earth_y)

