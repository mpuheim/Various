using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Zadanie_ZNS_Kohonen
{
    public partial class Form1 : Form
    {
        int poc_iteracii = 0; //celkovy pocet iteracii
        int poc_bodov = 0;        
        double[,] body = new double[10000, 2];
        double[,] vystupna_vrstva_x_y = new double[100, 100];
        int rozmer_vystupu_x, rozmer_vystupu_y;
        double[, ,] vaha_v_x_y = new double[2, 100, 100];
        int inicializovane = 0;
        double ri_t; //polomer susednosti
        double h_t;  //adaptacna vyska
        double gama; //parameter ucenia

        public Form1()
        {
            InitializeComponent();
            this.panel1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.panel1_MouseClick);
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Redraw);
        }

        private void panel1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                int x_sur = e.X;
                int y_sur = e.Y;
                System.Drawing.Graphics graphicsObj;
                graphicsObj = panel1.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.Black, 2);
                Rectangle myRectangle = new Rectangle(x_sur, y_sur, 2, 2);
                graphicsObj.DrawRectangle(myPen, myRectangle);
                body[poc_bodov, 0] = (float)x_sur / 500; //x-suradnica bodu
                body[poc_bodov, 1] = (float)y_sur / 450; //y-suradnica bodu
                poc_bodov++;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            rozmer_vystupu_x = (int)this.numericUpDown1.Value;
            rozmer_vystupu_y = (int)this.numericUpDown2.Value;
            //Inicializacia vah
            // -nahodna
            /*
            Random r = new Random();
            for (int V = 0; V < 2; V++)
            {
                for (int X = 0; X < vystup_X; X++)
                {
                    for (int Y = 0; Y < vystup_Y; Y++)
                    {
                        vaha_v_x_y[V, X, Y] = r.NextDouble();
                    }
                }
            }
            */
            // -pravidelna
            for (int X = 0; X < rozmer_vystupu_x; X++)
            {
                for (int Y = 0; Y < rozmer_vystupu_y; Y++)
                {
                    vaha_v_x_y[0, X, Y] = (float)(X + 0.5) / (float)rozmer_vystupu_x;
                    vaha_v_x_y[1, X, Y] = (float)(Y + 0.5) / (float)rozmer_vystupu_y;
                }
            }
            inicializovane = 1;
            //Vykreslenie vystupnej mriezky
            panel1_Redraw(sender, e);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (inicializovane == 0)
            {
                MessageBox.Show("Pred učením je potrebné inicializovať váhy.");
                return;
            }           
            //Nacitanie parametrov
            gama = (double)this.numericUpDown3.Value;
            poc_iteracii = (int)this.numericUpDown5.Value;
            ri_t = (double)this.numericUpDown4.Value;
            h_t = (double)this.numericUpDown6.Value;
            progressBar1.Maximum = poc_iteracii / 10;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            this.numericUpDown1.Value = rozmer_vystupu_x;
            this.numericUpDown2.Value = rozmer_vystupu_y;
            //Ucenie NS
            int i,j,v,x,y,akt_bod,x_vit,y_vit,pom;
            int[] shuffle = new int [10000];
            double dj; //vzdialenost medzi neuronmi
            for (int iteracia = 0; iteracia < poc_iteracii; iteracia++) // opakuj pre vsetky cykly ucenia
            {
                // Nahodne poprehadzovanie poradia vstupnych bodov
                for (i = 0; i < 10000; i++)
                {
                    shuffle[i] = -1;
                }
                Random ra = new Random();
                for (i = 0; i < poc_bodov; i++)
                {
                    pom = ra.Next(poc_bodov-1);
                    int opakuje_sa = 0;
                    for (j = 0; j < poc_bodov; j++)
                    {
                        if ((pom == shuffle[j]) && (j != i))
                            opakuje_sa = 1;
                    }
                    if (opakuje_sa == 1)
                    {
                        j = 0;
                        while (shuffle[j] != -1)
                            j++;
                        pom = j;
                    }
                    shuffle[i] = pom;
                }
                // Vypocet vystupu
                for (akt_bod = 0; akt_bod < poc_bodov; akt_bod++) //Opakuj pre vsetky body
                {
                    //Nastavenie vystupov na 0 
                    for (x = 0; x < rozmer_vystupu_x; x++)
                    {
                        for (y = 0; y < rozmer_vystupu_y; y++)
                        {
                            vystupna_vrstva_x_y[x, y] = 0;
                        }
                    }
                    //Hladanie vitazneho neuronu
                    x_vit = 0;
                    y_vit = 0;
                    for (x = 0; x < rozmer_vystupu_x; x++)
                    {
                        for (y = 0; y < rozmer_vystupu_y; y++)
                        {
                            for (v = 0; v < 2; v++)
                            {
                                vystupna_vrstva_x_y[x, y] += System.Math.Pow((vaha_v_x_y[v, x, y] - body[shuffle[akt_bod], v]), 2);
                            }
                            vystupna_vrstva_x_y[x, y] = System.Math.Sqrt(vystupna_vrstva_x_y[x, y]);

                            if (vystupna_vrstva_x_y[x, y] < vystupna_vrstva_x_y[x_vit, y_vit])
                            {
                                x_vit = x;
                                y_vit = y;
                            }
                        }
                    }
                    //Zmena vah
                    for (x = 0; x < rozmer_vystupu_x; x++)
                    {
                        for (y = 0; y < rozmer_vystupu_y; y++)
                        {
                            dj = System.Math.Pow((double)(x - x_vit), 2) + System.Math.Pow((double)(y - y_vit), 2);
                            for (v = 0; v < 2; v++)
                            {
                                vaha_v_x_y[v, x, y] += gama * h_t * System.Math.Exp(-System.Math.Pow(dj, 2) / ri_t) * (body[shuffle[akt_bod], v] - vaha_v_x_y[v, x, y]);
                            }
                        }
                    }
                    //DEBUG MessageBox.Show(string.Format("DEBUG X:{0} | Y:{1} \n akt_bod: {2} | shuffle[akt_bod]: {3} \n vit_x= {4}| vit_y = {5}", (int)(500 * body[shuffle[akt_bod], 0]), (int)(450 * body[shuffle[akt_bod], 1]), akt_bod, shuffle[akt_bod], x_vit, y_vit));
                    //DEBUG MessageBox.Show(string.Format("vaha_v_x_y[0,0] = {0} | {1} \nvaha_v_x_y[0,1] = {2} | {3} \nvaha_v_x_y[1,0] = {4} | {5} \nvaha_v_x_y[1,1] = {6} | {7} \n", vaha_v_x_y[0, 0, 0], vaha_v_x_y[1, 0, 0], vaha_v_x_y[0, 0, 1], vaha_v_x_y[0, 0, 1], vaha_v_x_y[0, 1, 0], vaha_v_x_y[0, 1, 0], vaha_v_x_y[0, 1, 1], vaha_v_x_y[0, 1, 1]));
                }
                if (iteracia % 10 == 1)
                    progressBar1.PerformStep();
            }
            // Vykreslenie vystupu na obrazovku
            panel1_Redraw(sender, e);
        }

        private void panel1_Redraw(object sender, EventArgs e)
        {
            
            System.Drawing.Graphics graphicsObj;
            graphicsObj = panel1.CreateGraphics();
            Rectangle myRectangle;
            // Prefarbenie plochy na pociatocnu farbu
            Pen myPen = new Pen(System.Drawing.Color.White, 600);
            myRectangle = new Rectangle(0, 0, 600, 600);
            graphicsObj.DrawRectangle(myPen, myRectangle);
            //Vykreslenie bodov
            int i;
            for (i = 0; i < poc_bodov; i++)
            {
                int x_sur = (int)(System.Math.Round(500 * body[i, 0]));
                int y_sur = (int)(System.Math.Round(450 * body[i, 1]));                
                myPen = new Pen(System.Drawing.Color.Black, 2);
                myRectangle = new Rectangle(x_sur, y_sur, 2, 2);
                graphicsObj.DrawRectangle(myPen, myRectangle);
            }
            //Vykreslenie vystupnej mriezky
            if (inicializovane == 0)
                return;            
            int X_sur, Y_sur;
            int X_sur_vlavo, Y_sur_vlavo;
            int X_sur_hore, Y_sur_hore;
            Pen myPen1 = new Pen(System.Drawing.Color.Blue, 1);
            Pen myPen2 = new Pen(System.Drawing.Color.Blue, 2);
            for (int X = 0; X < rozmer_vystupu_x; X++)
            {
                for (int Y = 0; Y < rozmer_vystupu_y; Y++)
                {
                    X_sur = (int)System.Math.Round(500 * vaha_v_x_y[0, X, Y]);
                    Y_sur = (int)System.Math.Round(450 * vaha_v_x_y[1, X, Y]);
                    //Vykreslenie bodu mriezky
                    myRectangle = new Rectangle(X_sur, Y_sur, 1, 1);
                    graphicsObj.DrawRectangle(myPen2, myRectangle);
                    //Vykreslenie ciary mriezky
                    if (X > 0)
                    {
                        X_sur_vlavo = (int)System.Math.Round(500 * vaha_v_x_y[0, X - 1, Y]);
                        Y_sur_vlavo = (int)System.Math.Round(450 * vaha_v_x_y[1, X - 1, Y]);
                        graphicsObj.DrawLine(myPen1, X_sur, Y_sur, X_sur_vlavo, Y_sur_vlavo);
                    }
                    if (Y > 0)
                    {
                        X_sur_hore = (int)System.Math.Round(500 * vaha_v_x_y[0, X, Y - 1]);
                        Y_sur_hore = (int)System.Math.Round(450 * vaha_v_x_y[1, X, Y - 1]);
                        graphicsObj.DrawLine(myPen1, X_sur, Y_sur, X_sur_hore, Y_sur_hore);
                    }
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            // Reset premennych
            poc_iteracii = 0;
            poc_bodov = 0;
            inicializovane = 0;
            // Reset kontroliek
            this.numericUpDown1.Value = 5;
            this.numericUpDown2.Value = 5;
            this.numericUpDown3.Value = (decimal)1;
            this.numericUpDown4.Value = (decimal)0.3;
            this.numericUpDown5.Value = 100;
            this.numericUpDown6.Value = 1;
            progressBar1.Maximum = 100;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            // Vymazanie obrazu
            panel1_Redraw(sender, e);
        }
    }
}
