import tkinter
root = tkinter.Tk()
w = 400 #w = root.winfo_screenwidth()
h = 400 #h = root.winfo_screenheight()
root.geometry("%dx%d" % (w, h))
canvas = tkinter.Canvas(root,bg="black")
canvas.pack(fill = "both", expand = True)

x1=w//2
y1=h//2
while True:
    x2=root.winfo_pointerx() - root.winfo_rootx()
    y2=root.winfo_pointery() - root.winfo_rooty()
    line=canvas.create_line(x1,y1,x2,y2,fill="white")
    canvas.update()
    canvas.delete(line)
