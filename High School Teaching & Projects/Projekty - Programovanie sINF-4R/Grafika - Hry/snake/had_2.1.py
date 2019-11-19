import tkinter
import random
import math

# vytvorenie hada
def draw_snake():
    #vykreslime clanky tela ako kruhy
    d=10
    for i in range(1,len(had_x)):
        had_c.append(canvas.create_oval(had_x[i], had_y[i], had_x[i]+d, had_y[i]+d, fill='blue', outline=''))
    #nakoniec vykreslime hlavu (aby bola prekreslena "nad" ostatne clanky)
    had_c.insert(0,canvas.create_oval(had_x[0], had_y[0], had_x[0]+d, had_y[0]+d, fill='lightblue', outline=''))

# pohyb hada        
def move_snake():
    global zmena_smeru
    global zmena_rychlosti
    #vypocitame smerovanie hlavy (uhol v ktorom sa pohybuje)
    novy_uhol = had_u[0]+zmena_smeru
    novy_uhol = (novy_uhol+360)%360 # tu osetrime, aby novy uhol bol z intervalu <0,360>
    #vypocitame rychlost hlavy
    nova_rych = had_v[0]+zmena_rychlosti
    if nova_rych < 1.0: nova_rych = 1.0 # tu osetrime maximalnu a minimalnu rychlost
    if nova_rych > 3.0: nova_rych = 3.0
    #vypocitame nove suradnice hlavy na zaklade rychlosti a smeru pohybu (pouzijeme goniometriu trojuholnika https://goo.gl/YOPkBL )
    uhol_v_radianoch = math.radians(novy_uhol)
    nove_x = had_x[0]+nova_rych*math.cos(uhol_v_radianoch)
    nove_y = had_y[0]+nova_rych*math.sin(uhol_v_radianoch)
    #posunieme hlavu hada na nove suradnice
    had_x.pop()
    had_y.pop()
    had_x.insert(0,nove_x)
    had_y.insert(0,nove_y)
    #nastavime hlave prislusnu rychlost a smer pohybu
    had_u.pop()
    had_v.pop()
    had_u.insert(0,novy_uhol)
    had_v.insert(0,nova_rych)
    #nakreslime novu poziciu hlavy
    d=10
    canvas.delete(had_c[-1]) #vymazeme posledny clanok z grafickej plochy
    had_c.pop() # odstranime posledny clanok zo zoznamu clankov
    canvas.itemconfig(had_c[0],fill='blue') #zmenime farbu prveho clanku
    had_c.insert(0,canvas.create_oval(had_x[0], had_y[0], had_x[0]+d, had_y[0]+d, fill='lightblue', outline='')) #nakreslime novu hlavu
    #resetujeme globalne premenne
    zmena_smeru = 0
    zmena_rychlosti = 0

# generovanie jedla
def generate_food():
    d=10
    for i in range(100):
        x=random.randrange(600)
        food_x.append(x)
        y=random.randrange(600)
        food_y.append(y)
        f='#{:06x}'.format(random.randrange(256**3))
        food_f.append(f)
        c=canvas.create_oval(x,y,x+d,y+d, fill=f, outline='')
        food_c.append(c)

# kolizia s jedlom
def food_collision():
    d=10
    for i in range(len(food_x)):
        if abs(food_x[i]-had_x[0])<9 and abs(food_y[i]-had_y[0])<9:
            #vypocitame smerovanie hlavy (uhol v ktorom sa pohybuje)
            uhol = had_u[0]
            #vypocitame rychlost hlavy
            rych = had_v[0]
            #vypocitame nove suradnice hlavy na zaklade rychlosti a smeru pohybu (pouzijeme goniometriu trojuholnika https://goo.gl/YOPkBL )
            uhol_v_radianoch = math.radians(uhol)
            nove_x = had_x[0]+rych*math.cos(uhol_v_radianoch)
            nove_y = had_y[0]+rych*math.sin(uhol_v_radianoch)
            #posunieme hlavu hada na nove suradnice (smerom k jedlu)
            had_x.insert(0,nove_x)
            had_y.insert(0,nove_y)
            #nastavime hlave prislusnu rychlost a smer pohybu
            had_u.insert(0,uhol)
            had_v.insert(0,rych)
            #nakreslime novu poziciu hlavy
            canvas.itemconfig(had_c[0],fill=food_f[i]) #zmenime farbu prveho clanku
            had_c.insert(0,canvas.create_oval(had_x[0], had_y[0], had_x[0]+d, had_y[0]+d, fill='lightblue', outline='')) #nakreslime novu hlavu
            #zmazeme zjedene jedlo
            canvas.delete(food_c[i])
            food_c.pop(i)
            food_x.pop(i)
            food_y.pop(i)
            food_f.pop(i)
            return

# aktualizacia hry
def herna_slucka(ms=10):
    while run and len(food_x)>0:
        #print("x:",had_x[0]," y:",had_y[0]," v:",had_v[0]," u:",had_u[0])
        move_snake()
        food_collision()
        canvas.update()
        canvas.after(ms)

def handler_vpravo(event):
    global zmena_smeru # otocenie doprava
    zmena_smeru = 5
    #print("vpravo")

def handler_vlavo(event):
    global zmena_smeru # otocenie dolava
    zmena_smeru = -5
    #print("vlavo")
    
def handler_zrychlenie(event):
    global zmena_rychlosti # zrychlenie
    zmena_rychlosti = 0.1
    #print("zrychlenie")

def handler_spomalenie(event):
    global zmena_rychlosti # spomalenie
    zmena_rychlosti = -0.1
    #print("spomalenie")

def quit_game():
    global run
    run = False

#start programu
window = tkinter.Tk()
canvas = tkinter.Canvas(width=600,height=600)
canvas.pack()
#nastavenie globalnych premennych - stav clankov hada
zmena_smeru = 0
zmena_rychlosti = 0
had_x=[100]  #x-ova suradnica clanku
had_y=[100]  #y-ova suradnica clanku
had_v=[2.0]  #rychlost, ktorou sa pohybuje clanok
had_u=[0.0]  #smer v ktorom sa pohybuje clanok na ploche (0-360°)
for i in range(1,50):
    had_x.append(had_x[i-1]-2)
    had_y.append(had_y[i-1])
    had_v.append(had_v[i-1])
    had_u.append(had_u[i-1])
#jednorazove vykreslenie celeho hada
had_c=[] 
draw_snake()
#generovanie jedla na ploche
food_x=[]  #x-ova suradnica jedla
food_y=[]  #y-ova suradnica jedla
food_f=[]  #farba jedla
food_c=[]  #kruh reprezentujuci jedlo
generate_food()
#priradenie handlerov pre eventy
canvas.bind_all('d', handler_vpravo)
canvas.bind_all('a', handler_vlavo)
canvas.bind_all('w', handler_zrychlenie)
canvas.bind_all('s', handler_spomalenie)
window.protocol('WM_DELETE_WINDOW', quit_game) #ukoncenie tlacidlom X
#spustenie hernej slucky
run=True
herna_slucka()

#canvas.mainloop()