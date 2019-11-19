import tkinter, time, random

'''DEFINICIE FUNKCII'''
def kresli_budovy():
    buildings_ID=[]
    buildings_x1=[]
    buildings_y1=[]
    buildings_x2=[]
    buildings_y2=[]
    for i in range(5,35):
        x1=d*(i-1)
        y1=400-random.randint(50,250)
        x2=d*i
        y2=400
        ID=canvas.create_rectangle(x1,y1,x2,y2,fill="blue",outline="black")
        buildings_ID.append(ID)
        buildings_x1.append(x1)
        buildings_y1.append(y1)
        buildings_x2.append(x2)
        buildings_y2.append(y2)
    return buildings_ID, buildings_x1, buildings_y1, buildings_x2, buildings_y2

def pohni_bomberom(ID,x,y):
    canvas.move(ID,d,0)
    x=x+d
    if x>=400:
        canvas.coords(ID,0,y+d,d,y+d+d)
        x=0; y=y+d;
    return x,y

def pohni_bombou(ID,x,y):
    canvas.move(ID,0,d)
    y=y+d
    return x,y

def vyhod_bombu(event):
    global bomb_ID, bomb_x, bomb_y
    if event.keysym == "Down" and bomb_ID == None:
        bomb_x = x
        bomb_y = y+d
        bomb_ID=canvas.create_rectangle(bomb_x,bomb_y,bomb_x+d,bomb_y+d,fill="black")

def kolizia_obdlznikov(Ax1,Ay1,Ax2,Ay2,Bx1,By1,Bx2,By2):
    # ak (Ax2 je vnutri Bx1<->Bx2) alebo (Ax1 je vnutri Bx1<->Bx2):
    if (Ax2 >= Bx1 and Ax2 <= Bx2) or (Ax1 >= Bx1 and Ax1 <= Bx2):
        # ak (Ay2 je vnutri By1<->By2) alebo (Ay1 je vnutri By1<->By2):
        if (Ay2 >= By1 and Ay2 <= By2) or (Ay1 >= By1 and Ay1 <= By2):
            return True
    return False

def kontrola_kolizie():
    global bomb_ID, bomb_x, bomb_y
    for i in range(len(b_ID)):
        Ax1=bomb_x
        Ay1=bomb_y
        Ax2=bomb_x+d
        Ay2=bomb_y+d
        Bx1=b_x1[i]
        By1=b_y1[i]
        Bx2=b_x2[i]
        By2=b_y2[i]
        if kolizia_obdlznikov(Ax1,Ay1,Ax2,Ay2,Bx1,By1,Bx2,By2) == True:
            #zmensenie budovy o 5 poschodi
            b_y1[i] = b_y1[i] + 5*d
            canvas.coords(b_ID[i],b_x1[i],b_y1[i],b_x2[i],b_y2[i])
            #znicenie bomby
            canvas.delete(bomb_ID)
            bomb_ID = None; bomb_x = None; bomb_y = None
            return
    if bomb_y > 400:
        canvas.delete(bomb_ID)
        bomb_ID = None; bomb_x = None; bomb_y = None

'''HLAVNY PROGRAM'''
#vytvorenie okna
window=tkinter.Tk()
window.title("Bomber")
#pridanie kresliacej plochy
canvas=tkinter.Canvas(window,width=400,height=400)
canvas.pack()
#nastavenie suradnic a vykreslednie bombera
x=0; y=0; d=10;
bomber=canvas.create_rectangle(x,y,x+d,y+d,fill="brown")
canvas.update()
#priprava bomby
bomb_ID = None; bomb_x = None; bomb_y = None
canvas.bind_all("<Key>",vyhod_bombu)
#vykreslenie budov
b_ID, b_x1, b_y1, b_x2, b_y2 = kresli_budovy()
#herna slucka
while y<400:
    time.sleep(0.1)
    x, y = pohni_bomberom(bomber,x,y)
    if bomb_ID != None:
        bomb_x, bomb_y = pohni_bombou(bomb_ID,bomb_x,bomb_y)
        kontrola_kolizie()
    canvas.update()
#zatvorenie okna a koniec programu
window.destroy()
