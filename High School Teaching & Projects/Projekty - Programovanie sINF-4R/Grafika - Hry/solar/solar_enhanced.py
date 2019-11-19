import tkinter, time, math

def attraction(self, other):
    # Compute the distance of the other body.
    dx = other.px-self.px
    dy = other.py-self.py
    d = math.sqrt(dx**2 + dy**2)
    # Return zero force if distance is zero.
    if d == 0: return 0, 0
    # Compute the force of attraction
    f = G * self.mass * other.mass / (d**2)
    # Compute the direction of the force.
    theta = math.atan2(dy, dx)
    fx = math.cos(theta) * f
    fy = math.sin(theta) * f
    return fx, fy

def update(timestep,bodies):
    for body in bodies:
        # Add up all of the forces exerted on 'body'.
        body.fx = 0.0
        body.fy = 0.0
        for other in bodies:
            # Don't calculate the body's attraction to itself
            if body is other:
                continue
            fx, fy = attraction(body,other)
            body.fx += fx
            body.fy += fy
    # Update velocities based upon on the force.
    for body in bodies:
        body.vx += body.fx / body.mass * timestep
        body.vy += body.fy / body.mass * timestep
        # Update positions
        body.px += body.vx * timestep
        body.py += body.vy * timestep
    # Draw on canvas
    cx, cy = get_center(bodies)
    for body in bodies:
        x1= body.px*SCALE - body.r - cx + w/2
        y1= body.py*SCALE - body.r - cy + h/2
        x2= body.px*SCALE + body.r - cx + w/2
        y2= body.py*SCALE + body.r - cy + h/2
        canvas.coords(body.id,x1,y1,x2,y2)
    # Update canvas
    canvas.update()

def get_center(bodies):
    for body in bodies:
        if body.name == CENTER:
            cx = body.px*SCALE
            cy = body.py*SCALE
            return cx, cy
    return w/2, h/2
    
def set_center(e):
    global CENTER
    distances=[]
    cx, cy = get_center(bodies)
    for body in bodies:
        dx = (e.x-w/2) - (body.px*SCALE-cx)
        dy = (e.y-h/2) - (body.py*SCALE-cy)
        d = math.sqrt(dx**2 + dy**2)
        distances.append([body.name,d])
    CENTER = min(distances,key=lambda x:x[1])[0]
    
def zoom(e):
    global SCALE
    if e.delta > 0:
        SCALE=SCALE*2
    else:
        SCALE=SCALE/2

def speedup(e):
    global timestep
    if e.keysym == "plus":
        timestep = timestep * 2
    if e.keysym == "minus":
        timestep = timestep / 2
        

# Simulation parameters
w=h=600                 # Canvas size
timestep = 5*60         # Simulation time step (5 minutes)
G = 6.67428e-11         # The gravitational constant G
AU = (149.6e6 * 1000)   # Astronomical unit = 149.6 million km
SCALE = 50 / AU         # Assumed scale: 50 pixels = 1AU.
CENTER = 'Sun'          # Central object of simulation

# Simulation window
window = tkinter.Tk()
canvas = tkinter.Canvas(width=w,height=h)
canvas.bind("<MouseWheel>",zoom)
canvas.bind("<Button-1>",set_center)
canvas.bind_all("<Key>",speedup)
canvas.pack()

# Simulation objects
class struct: pass
bodies=[]

sun = struct()
sun.name = 'Sun'
sun.mass = 1.98892 * 10**30
sun.px = 0
sun.py = 0
sun.vx = 0
sun.vy = 0
sun.r = 11
sun.color = "yellow"
bodies.append(sun)

mercury = struct()
mercury.name = 'Mercury'
mercury.mass = 3.3011 * 10**23
mercury.px = 0.387 * AU
mercury.py = 0
mercury.vx = 0
mercury.vy = -47.362 * 1000
mercury.r = 3
mercury.color = "orange"
bodies.append(mercury)

venus = struct()
venus.name = 'Venus'
venus.mass = 4.8685 * 10**24
venus.px = 0.723 * AU
venus.py = 0
venus.vx = 0
venus.vy = -35.02 * 1000
venus.r = 4
venus.color = "red"
bodies.append(venus)

earth = struct()
earth.name = 'Earth'
earth.mass = 5.9742 * 10**24
earth.px = 1*AU
earth.py = 0
earth.vx = 0
earth.vy = -29.783 * 1000 # 29.783 km/sec
earth.r = 5
earth.color = "blue"
bodies.append(earth)

moon = struct()
moon.name = 'Moon'
moon.mass = 7.342 * 10**22
moon.px = 1.00257*AU
moon.py = 0
moon.vx = 0
moon.vy = (-29.783-1.022) * 1000
moon.r = 2
moon.color = "black"
bodies.append(moon)

mars = struct()
mars.name = 'Mars'
mars.mass = 6.4171 * 10**23
mars.px = 1.523*AU
mars.py = 0
mars.vx = 0
mars.vy = -24.007 * 1000
mars.r = 3
mars.color = "red"
bodies.append(mars)

jupiter = struct()
jupiter.name = 'Jupiter'
jupiter.mass = 1.89 * 10**27
jupiter.px = 5.2*AU
jupiter.py = 0
jupiter.vx = 0
jupiter.vy = -13.07 * 1000
jupiter.r = 7
jupiter.color = "blue"
bodies.append(jupiter)

saturn = struct()
saturn.name = 'Saturn'
saturn.mass = 5.6834 * 10**26
saturn.px = 9.5826*AU
saturn.py = 0
saturn.vx = 0
saturn.vy = -9.68 * 1000
saturn.r = 6
saturn.color = "gray"
bodies.append(saturn)

titan = struct()
titan.name = 'Titan'
titan.mass = 1.3452 * 10**23
titan.px = 9.5826*AU + 1221870*1000
titan.py = 0
titan.vx = 0
titan.vy = (-9.68-5.57) * 1000
titan.r = 2
titan.color = "black"
bodies.append(titan)

halley = struct()
halley.name = 'Halley'
halley.mass = 2.2 * 10**14
halley.px = 35*AU
halley.py = 0
halley.vx = 0
halley.vy = 1 * 1000
halley.r = 1
halley.color = "black"
bodies.append(halley)

# Draw objects
cx, cy = get_center(bodies)
for body in reversed(bodies):
    x1= body.px*SCALE - body.r - cx + w/2
    y1= body.py*SCALE - body.r - cy + h/2
    x2= body.px*SCALE + body.r - cx + w/2
    y2= body.py*SCALE + body.r - cy + h/2
    body.id = canvas.create_oval(x1,y1,x2,y2,fill=body.color)

# Print controls info
txt = "Scroll to zoom | Click planet to change focus | Press + or - to change speed"
canvas.create_text(w//2,h-15,fill="black",font="Times 10",text=txt)

# Start simulation
while True:
    update(timestep,bodies)
