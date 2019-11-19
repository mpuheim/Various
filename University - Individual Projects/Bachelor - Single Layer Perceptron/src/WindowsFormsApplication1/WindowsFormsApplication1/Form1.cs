using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        int pocitadlo = 0;
        int celkove_iteracie = 0; //celkovy pocet iteracii
        double[,] body = new double[10000,3];
        double vahaX, vahaY, vahaP; // Vahy
        bool nastavene_vahy = false;

        public Form1()
        {
            InitializeComponent();
        }        

        private void button1_Click(object sender, EventArgs e) //Ucenie
        {
            if (nastavene_vahy==false)
            {
                MessageBox.Show("Váhy neboli inicializované!");
                return;
            }
            if (pocitadlo > 0)
            {
                // Prefarbi celu plochu na pociatocnu farbu
                System.Drawing.Graphics graphicsObj;
                graphicsObj = panel1.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.White, 600);
                Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
                graphicsObj.DrawRectangle(myPen, myRectangle);
                // Opatovne vykresli zadane body
                for (int p = 0; p < pocitadlo; p++)
                {
                    graphicsObj = panel1.CreateGraphics();
                    if (body[p, 2] == 0) //cerveny
                        myPen = new Pen(System.Drawing.Color.Red, 2);
                    else //modry
                        myPen = new Pen(System.Drawing.Color.Blue, 2);
                    myRectangle = new Rectangle((int)(body[p, 0]), (int)(body[p, 1]), 2, 2);
                    graphicsObj.DrawRectangle(myPen, myRectangle);
                }
                // Nastav parameter ucenia.
                double ucenie = 0.05;
                int iteracia = 0; //Iteracie v ramci tohto cyklu ucenia
                double globalna_chyba;               
                // Jadro perceptronu.
                int vystup;
                do
                {
                    globalna_chyba = 0;                    
                    for (int p = 0; p < pocitadlo; p++)
                    {
                        // Vypocitaj vystup.
                        double funkcia = (body[p, 0] / 500) * vahaX + (body[p, 1] / 500) * vahaY - vahaP;
                        if (funkcia <= (1 / 2)) //500 je rozmer vykreslovacej plochy, delenie suradnice bodu je kvoli normalizacii pre perceptron
                            vystup = 0;
                        else
                            vystup = 1;
                        // Vypocitaj lokalnu chybu (tj. chybu pre dany vstup/bod).
                        double lokalna_chyba = body[p, 2] - vystup; // chyba = ocakavany vystup - vystup                        
                        if (lokalna_chyba != 0) 
                        {
                            // Ak je lokalna chyba vacsia ako nula zmen vahy.
                            vahaX += ucenie * lokalna_chyba * (body[p, 0] / 500);
                            vahaY += ucenie * lokalna_chyba * (body[p, 1] / 500);
                            vahaP += ucenie * lokalna_chyba * (-1);
                        }
                        // Absolutna hodnota chyby.
                        globalna_chyba += Math.Abs(lokalna_chyba);
                    }
                    iteracia++; //Aktualizuj iteracie v ramci tohto cyklu ucenia
                } while ((globalna_chyba != 0)&&(iteracia<1000));
                celkove_iteracie += iteracia; //Aktualizuj celkovy pocet iteracii
                if (iteracia < 1000)
                {
                    MessageBox.Show(string.Format("Body boli úspešne odseparované po {0} iteráciách.", celkove_iteracie));
                    //DEBUG MessageBox.Show(string.Format("vahaX = {0}\nvahaY = {1}\nvahaP = {2}", vahaX, vahaY, vahaP));
                    //DEBUG MessageBox.Show(string.Format("a = {0} \nb = {1}", -vahaY/vahaX, (0.5 + vahaP) / vahaX));
                }
                else
                    MessageBox.Show(string.Format("Body neboli odseparovane ani po {0} iteráciách. Zrejme nie sú lineárne separovateľné.", celkove_iteracie));
                //  Vykresli priamku
                graphicsObj = panel1.CreateGraphics();
                myPen = new Pen(System.Drawing.Color.Black, 1);
                if (vahaY/vahaX<0.001)
                    graphicsObj.DrawLine(myPen, -5000, (float)((5000 * vahaX + 0.5 + 500 * vahaP) / vahaY), 5500, (float)((-5500 * vahaX + 0.5 + 500 * vahaP) / vahaY));
                else
                    graphicsObj.DrawLine(myPen, (float)((5000 * vahaY + 0.5 + 500 * vahaP) / vahaX), -5000, (float)((-5500 * vahaY + 0.5 + 500 * vahaP) / vahaX), 5500);
            }
            else
            {
                MessageBox.Show("Je potrebne zadat nejake body!");
            }
        }

        private void button2_Click(object sender, EventArgs e) //Nahodne nastavenie vah
        {
            // Randomizuj vahy.
            Random r = new Random();
            vahaX = r.NextDouble();
            vahaY = r.NextDouble();
            vahaP = r.NextDouble(); //Vaha na prahu
            // Prefarbi celu plochu na pociatocnu farbu
            System.Drawing.Graphics graphicsObj;
            graphicsObj = panel1.CreateGraphics();
            Pen myPen = new Pen(System.Drawing.Color.White, 600);
            Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
            graphicsObj.DrawRectangle(myPen, myRectangle);
            // Opatovne vykresli zadane body
            for (int p = 0; p < pocitadlo; p++)
            {
                graphicsObj = panel1.CreateGraphics();
                if (body[p, 2] == 0) //cerveny
                    myPen = new Pen(System.Drawing.Color.Red, 2);
                else //modry
                    myPen = new Pen(System.Drawing.Color.Blue, 2);
                myRectangle = new Rectangle((int)(body[p, 0]), (int)(body[p, 1]), 2, 2);
                graphicsObj.DrawRectangle(myPen, myRectangle);
            }
            MessageBox.Show("Vahy boli nastavene na nahodne hodnoty!");
            // Vymaz celkove iteracie
            celkove_iteracie = 0;
            // Oznac vahy ako nastavene
            nastavene_vahy = true;
        }

        private void button3_Click(object sender, EventArgs e) //Vymazanie bodov
        {
            // Prefarbi celu plochu na pociatocnu farbu
            System.Drawing.Graphics graphicsObj;
            graphicsObj = panel1.CreateGraphics();
            Pen myPen = new Pen(System.Drawing.Color.White, 600);
            Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
            graphicsObj.DrawRectangle(myPen, myRectangle);
            MessageBox.Show("Vsetky body boli odstranene!");
            // Resetuj pocitadlo
            pocitadlo = 0;
            celkove_iteracie = 0;
        }

        private void panel1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                int x_sur = e.X;
                int y_sur = e.Y;
                System.Drawing.Graphics graphicsObj;
                graphicsObj = panel1.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.Red, 2);
                Rectangle myRectangle = new Rectangle(x_sur, y_sur, 2, 2);
                graphicsObj.DrawRectangle(myPen, myRectangle);
                body[pocitadlo, 0] = x_sur; //x-suradnica bodu
                body[pocitadlo, 1] = y_sur; //y-suradnica bodu
                body[pocitadlo, 2] = 0;     //trieda bodu (cerveny)
                pocitadlo++;
            }
            if (e.Button == MouseButtons.Right)
            {
                int x_sur = e.X;
                int y_sur = e.Y;
                System.Drawing.Graphics graphicsObj;
                graphicsObj = panel1.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.Blue, 2);
                Rectangle myRectangle = new Rectangle(x_sur, y_sur, 2, 2);
                graphicsObj.DrawRectangle(myPen, myRectangle);
                body[pocitadlo, 0] = x_sur; //x-suradnica bodu
                body[pocitadlo, 1] = y_sur; //y-suradnica bodu
                body[pocitadlo, 2] = 1;     //trieda bodu (modry)
                pocitadlo++;
            }
        }
    }
}
