import tkinter
import random

#vytvorenie hernej plochy
def draw_area():
    #premalujeme celu plochu na bielo jednym velkym obdlznikom
    #canvas.create_rectangle(0, 0, 300, 200, fill='white', outline='')
    
    # ale lepsie je zmazat nakreslene utvary
    for stvorec in had_r:
        canvas.delete(stvorec)
    # nakoniec vyprazdnime pomocny zoznam (kedze stvorce boli zmazane)
    had_r.clear()

# vytvorenie hada
def draw_snake():
    # velkost stvorca na ploche
    d=10
    # suradnice kresleneho stvorca
    x = d*had_x[0]
    y = d*had_y[0]
    # vykreslenie (hlavy hada)
    id=canvas.create_rectangle(x, y, x+d, y+d, fill='blue', outline='')
    had_r.append(id)
    # vykreslenie clankov hada
    for i in range(1,len(had_x)):
        # suradnice kresleneho stvorca
        x = d*had_x[i]
        y = d*had_y[i]
        # vykreslenie clanku
        id=canvas.create_rectangle(x, y, x+d, y+d, fill='lightblue', outline='')
        had_r.append(id)

# pohyb hada        
def move_snake():
    had_x.pop()
    had_y.pop()
    had_x.insert(0,had_x[0]+smer_x)
    had_y.insert(0,had_y[0]+smer_y)

# aktualizacia hry
def herna_slucka(kolko=100, ms=200):
    for i in range(kolko):
        move_snake()
        draw_area()
        draw_snake()
        canvas.update()
        canvas.after(ms)

def handler_vpravo(event):
    global smer_x # zmenu globalnej premennej musime deklarovat
    global smer_y
    smer_x = 1  #smer v x-ovej osi
    smer_y = 0  #smer v y_ovej osi
    print("vpravo")

def handler_vlavo(event):
    global smer_x # zmenu globalnej premennej musime deklarovat
    global smer_y
    smer_x = -1 #smer v x-ovej osi
    smer_y = 0  #smer v y_ovej osi
    print("vlavo")

#start programu
canvas = tkinter.Canvas(width=300,height=200)
canvas.pack()
#nastavenie globalnych premennych
had_x=[3,3,3,2,1]
had_y=[4,3,2,2,2]
had_r=[] #tu budete ukladat vykreslene stvorce
smer_x = 1 #smer v x-ovej osi
smer_y = 0 #smer v y_ovej osi
#priradenie handlerov pre eventy
canvas.bind_all('d', handler_vpravo)
canvas.bind_all('a', handler_vlavo)
#spustenie hernej slucky
herna_slucka()

#canvas.mainloop()