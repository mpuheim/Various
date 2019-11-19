using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Zadanie_ZNS_Backprop
{
    public partial class Form1 : Form
    {        
        string Train_Pat, Test_Pat; //retazce do ktorych budu nahrane subory vzoriek
        int Train_Input_Pocet, Test_Input_Pocet; // pocet vzoriek
        double[,] Train_Input = new double [100000,4]; //trenovacia mnozina (pole najviac 100000 vzoriek - pre kazdu vzorku su 4 hodnoty - suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
        double[,] Test_Input = new double[100000, 4]; //testovacia mnozina (pole najviac 100000 vzoriek - pre kazdu vzorku su 4 hodnoty - suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
        int[] shuffle = new int [100000];
        int N0, N1, N2, N3; // pocty neuronov na jednotlivych vrstvach
        double[] N0_vystup = new double[2];
        double[] N1_vstup = new double[100];
        double[] N1_aktivacia = new double[100];
        double[] N1_vystup = new double[100];
        double[] N2_vstup = new double[100];
        double[] N2_aktivacia = new double[100];
        double[] N2_vystup = new double[100];
        double[] N3_vstup = new double[2];
        double[] N3_aktivacia = new double[2];
        double[] N3_vystup = new double[2];
        double[,] w_N1_N0 = new double[100, 2];    //vahy medzi neuronmi vrstiev N1 a N0
        double[,] w_N2_N1 = new double[100, 100]; //vahy medzi neuronmi vrstiev N2 a N1
        double[,] w_N3_N2 = new double[2, 100];   //vahy medzi neuronmi vrstiev N3 a N2
        double[] w_N1_prah = new double[100];     //vahy na prahu neuronov vrstvy N1
        double[] w_N2_prah = new double[100];     //vahy na prahu neuronov vrstvy N2
        double[] w_N3_prah = new double[2];       //vahy na prahu neuronov vrstvy N3
        double[,] Dw_N1_N0 = new double[100, 2];   //zmena vahy medzi neuronmi vrstiev N1 a N0
        double[,] Dw_N2_N1 = new double[100, 100]; //zmena vahy medzi neuronmi vrstiev N2 a N1
        double[,] Dw_N3_N2 = new double[2, 100];   //zmena vahy medzi neuronmi vrstiev N3 a N2
        double[] Dw_N1_prah = new double[100];     //zmena vahy na prahu neuronov vrstvy N1
        double[] Dw_N2_prah = new double[100];     //zmena vahy na prahu neuronov vrstvy N2
        double[] Dw_N3_prah = new double[2];       //zmena vahy na prahu neuronov vrstvy N3
        double[] delta_N1_N0 = new double[100];      //chybovy signa pre neurony vrstvy N1
        double[] delta_N2_N1 = new double[100];      //chybovy signa pre neurony vrstvy N2
        double[] delta_N3_N2 = new double[100];      //chybovy signa pre neurony vrstvy N3
        double[] chyba = new double[100000];      //pole chyby ucenia v kazdom cykle ucenia
        double alfa; //parameter sigmoidalnej aktivacnej funkcie alfa
        double gama; //parameter ucenia gama
        int cyklov;  //celkovy pocet cyklov ucenia
        double presnost_percent;
        int poc_spravne_klasifikovanych, poc_nespravne_klasifikovanych, poc_ciastocne_spravne_klasifikovanych;

        public Form1()
        {
            InitializeComponent();
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Redraw);
            this.panel2.Paint += new System.Windows.Forms.PaintEventHandler(this.panel2_Redraw);
            this.panel3.Paint += new System.Windows.Forms.PaintEventHandler(this.panel3_Redraw);
            this.panel4.Paint += new System.Windows.Forms.PaintEventHandler(this.panel4_Redraw);
        }

        private double der_funkcie(double vstup)
        {
            return ((alfa * System.Math.Exp(-alfa * vstup)) / ((1 + System.Math.Exp(-alfa * vstup)) * (1 + System.Math.Exp(-alfa * vstup))));
        }

        private void LoadTrainPatToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Train_Pat_Path;

            // Otvorenie okna pre výber súboru s trénovacou množinou
            OpenTrainPatOpenFileDialog.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            if (OpenTrainPatOpenFileDialog.ShowDialog() == DialogResult.Cancel)
                return;
            Train_Pat_Path = OpenTrainPatOpenFileDialog.FileName;
            //DEBUG// MessageBox.Show(string.Format("DEBUG \n {0}", Train_Pat_Path));

            // Nacitanie suboru
            if (System.IO.File.Exists(Train_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Train_Pat_Path);
                Train_Pat = objReader.ReadToEnd();                
                objReader.Close();
            }
            else
            {
                MessageBox.Show("Súbor neexistuje.");
                return;
            }
            //DEBUG// MessageBox.Show(string.Format("DEBUG - Vypis casti suboru:\n {0} \n \n ............", Train_Pat.Substring(0, 100)));

            // Nacitanie premenych z obsahu suboru
            System.IO.StringReader strReader;
            strReader = new System.IO.StringReader(Train_Pat);
            string line;
            // Nacitanie poctu vzoriek
            if ((line = strReader.ReadLine()) == null)
            {
                MessageBox.Show("Súbor je prázdny.");
                return;
            }
            try
            {
                Train_Input_Pocet = Convert.ToInt32(line);
            }
            catch (FormatException) {
                MessageBox.Show(string.Format("Súbor nebol načítaný. Nemožno konvertovať reťazec '{0}' na dátový typ Double.",line));
                return;
            }               
            catch (OverflowException) {
                MessageBox.Show(string.Format("Súbor nebol načítaný. Hodnota '{0}' je mimo rozsahu dátového typu Double.",line));
                return;
            }

            if (Train_Input_Pocet > 100000)
            {
                MessageBox.Show("Súbor nemohol byť načítaný, pretože počet vzoriek je vyšší, ako horná hranica programu (100 000).");
                return;
            }
            // Preskocenie 4 riadkov
            for (int i = 0; i < 4; i++)
                line = strReader.ReadLine();
            // Nacitanie vzoriek (Pre kazdu vzorku su 4 hodnoty - suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
            int input_number = 0; //cislo vzorky
            int input_data = 0;   //parameter urcujuci druh hodnoty (v poradi od 0 do 3 je to: suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
            while ((line = strReader.ReadLine()) != null)
            {
                if (input_data == 2)
                    line = line.Substring(0, 1);
                try
                {
                    Train_Input[input_number, input_data] = Convert.ToDouble(line.Replace(".", ","));
                }
                catch (FormatException)
                {
                    MessageBox.Show(string.Format("Súbor nebol načítaný. Nemožno konvertovať reťazec '{0}' na dátový typ Double.", line));
                    return;
                }
                catch (OverflowException)
                {
                    MessageBox.Show(string.Format("Súbor nebol načítaný. Hodnota '{0}' je mimo rozsahu dátového typu Double.", line));
                    return;
                }
                if (input_data++ >= 2)
                {
                    Train_Input[input_number, 3] = System.Math.Abs(1 - Train_Input[input_number, 2]); // Trieda vzorky pre 2. vystupny neuron je opacna k triede 1. neuronu
                    input_data = 0;
                    input_number++;
                    line = strReader.ReadLine(); // Preskocenie prazdneho riadku
                }
            }
            strReader.Close();
            if (Train_Input_Pocet != input_number)
            {
                MessageBox.Show(string.Format("Súbor je poškodený. Počet vzoriek v súbore ({0}) nesúhlasí s počtom deklarovaným na jeho začiatku ({1}).",input_number,Train_Input_Pocet));
                return;
            }

            // Vykreslenie vzoriek na obrazovku
            panel1_Redraw(sender, e);
        }

        private void LoadTestPatToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string Test_Pat_Path;

            // Otvorenie okna pre výber súboru s testovacou množinou
            OpenTestPatOpenFileDialog.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            if (OpenTestPatOpenFileDialog.ShowDialog() == DialogResult.Cancel)
                return;
            Test_Pat_Path = OpenTestPatOpenFileDialog.FileName;
            //DEBUG// MessageBox.Show(string.Format("DEBUG \n {0}", Test_Pat_Path));

            // Nacitanie suboru
            if (System.IO.File.Exists(Test_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Test_Pat_Path);
                Test_Pat = objReader.ReadToEnd();
                objReader.Close();
            }
            else
            {
                MessageBox.Show("Súbor neexistuje.");
                return;
            }
            //DEBUG// MessageBox.Show(string.Format("DEBUG - Vypis casti suboru:\n {0} \n \n ............", Test_Pat.Substring(0, 100)));

            // Nacitanie premenych z obsahu suboru
            System.IO.StringReader strReader;
            strReader = new System.IO.StringReader(Test_Pat);
            string line;
            // Nacitanie poctu vzoriek
            if ((line = strReader.ReadLine()) == null)
            {
                MessageBox.Show("Súbor je prázdny.");
                return;
            }
            try
            {
                Test_Input_Pocet = Convert.ToInt32(line);
            }
            catch (FormatException)
            {
                MessageBox.Show(string.Format("Súbor nebol načítaný. Nemožno konvertovať reťazec '{0}' na dátový typ Double.", line));
                return;
            }
            catch (OverflowException)
            {
                MessageBox.Show(string.Format("Súbor nebol načítaný. Hodnota '{0}' je mimo rozsahu dátového typu Double.", line));
                return;
            }

            if (Test_Input_Pocet > 100000)
            {
                MessageBox.Show("Súbor nemohol byť načítaný, pretože počet vzoriek je vyšší, ako horná hranica programu (100 000).");
                return;
            }
            // Preskocenie 4 riadkov
            for (int i = 0; i < 4; i++)
                line = strReader.ReadLine();
            // Nacitanie vzoriek (Pre kazdu vzorku su 4 hodnoty - suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
            int input_number = 0; //cislo vzorky
            int input_data = 0;   //parameter urcujuci druh hodnoty (v poradi od 0 do 3 je to: suradnica x, suradnica y, vystup 1. vystupneho neuronu, vystup 2. vystupneho neuronu)
            while ((line = strReader.ReadLine()) != null)
            {
                if (input_data == 2)
                    line = line.Substring(0, 1);
                try
                {
                    Test_Input[input_number, input_data] = Convert.ToDouble(line.Replace(".", ","));
                }
                catch (FormatException)
                {
                    MessageBox.Show(string.Format("Súbor nebol načítaný. Nemožno konvertovať reťazec '{0}' na dátový typ Double.", line));
                    return;
                }
                catch (OverflowException)
                {
                    MessageBox.Show(string.Format("Súbor nebol načítaný. Hodnota '{0}' je mimo rozsahu dátového typu Double.", line));
                    return;
                }
                if (input_data++ >= 2)
                {
                    Test_Input[input_number, 3] = System.Math.Abs(1 - Test_Input[input_number, 2]); // Trieda vzorky pre 2. vystupny neuron je opacna k triede 1. neuronu
                    input_data = 0;
                    input_number++;
                    line = strReader.ReadLine(); // Preskocenie prazdneho riadku
                }
            }
            strReader.Close();
            if (Test_Input_Pocet != input_number)
            {
                MessageBox.Show(string.Format("Súbor je poškodený. Počet vzoriek v súbore ({0}) nesúhlasí s počtom deklarovaným na jeho začiatku ({1}).", input_number, Test_Input_Pocet));
                return;
            }

            // Vykreslenie vzoriek na obrazovku
            panel2_Redraw(sender, e);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int i, j, cislo_vzorky, cyklus_ucenia,pom;
            double temp_vstup;

            if (Test_Input_Pocet < 1)
            {
                MessageBox.Show("Je potrebné načítať súbor s testovacou vzorkou.");
                return;
            }
            if (Train_Input_Pocet < 1)
            {
                MessageBox.Show("Je potrebné načítať súbor s trénovacou vzorkou.");
                return;
            }

            //Nacitanie parametrov
            N0 = 2;
            N1 = (int)this.numericUpDown3.Value;
            N2 = (int)this.numericUpDown4.Value;
            N3 = 2;
            alfa =(double)this.numericUpDown1.Value;
            gama =(double)this.numericUpDown2.Value;
            cyklov = (int)this.numericUpDown5.Value;
            progressBar1.Maximum = cyklov / 10;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            //DEBUG// MessageBox.Show(string.Format("DEBUG - Vypis premennych:\n N1={0} \n N2={1} \n alfa={2} \n gama={3} \n cyklov = {4}.", N1, N2, alfa, gama, cyklov));

            //Inicializacia vah
            Random r = new Random();
            //Vahy medzi N0 a N1
            for (i = 0; i < N1; i++)
            {
                for (j = 0; j < N0; j++)
                {
                    w_N1_N0[i,j] = 1 - 2 * r.NextDouble();
                }
            }
            //Prah na N1
            for (i = 0; i < N1; i++)
                {
                    w_N1_prah[i] = 1 - 2 * r.NextDouble();
                }
            //Vahy medzi N1 a N2
            for (i = 0; i < N2; i++)
            {
                for (j = 0; j < N1; j++)
                {
                    w_N2_N1[i, j] = 1 - 2 * r.NextDouble();
                }
            }
            //Prah na N2
            for (i = 0; i < N2; i++)
                {
                    w_N2_prah[i] = 1 - 2 * r.NextDouble();
                }
            //Vahy medzi N2 a N3
            for (i = 0; i < N3; i++)
            {
                for (j = 0; j < N2; j++)
                {
                    w_N3_N2[i, j] = 1 - 2 * r.NextDouble();
                }
            }
            //Prah na N3
            for (i = 0; i < N3; i++)
                {
                    w_N3_prah[i] = 1 - 2 * r.NextDouble();
                }

            // Ucenie NS
            presnost_percent = 0;
            poc_spravne_klasifikovanych = 0;
            poc_ciastocne_spravne_klasifikovanych = 0;
            poc_nespravne_klasifikovanych = 0;
            for (cyklus_ucenia = 0; cyklus_ucenia < cyklov; cyklus_ucenia++) // opakuj pre vsetky cykly ucenia
            {
                chyba[cyklus_ucenia] = 0;
                // Poprehadzovanie poradia vzoriek
                for (i = 0; i < 100000; i++)
                {
                    shuffle[i] = -1;
                }
                Random ra = new Random();
                for (i = 0; i < Train_Input_Pocet; i++)
                {                    
                    pom = ra.Next(Train_Input_Pocet-1);
                    int opakuje_sa = 0;
                    for (j = 0; j < Train_Input_Pocet; j++)
                    {
                            if ((pom == shuffle[j])&&(j!=i))
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
                for (cislo_vzorky = 0; cislo_vzorky < Train_Input_Pocet; cislo_vzorky++) // opakuj pre vsetky vzorky
                {
                    // Vstupna vrstva
                    for (i = 0; i < N0; i++) // opakuj pre vsetky neurony vo vrstve
                    {
                        N0_vystup[i] = Train_Input[shuffle[cislo_vzorky], i];
                    }
                    // 1. Skryta vrstva
                    for (i = 0; i < N1; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N0; j++)
                        {
                            temp_vstup = temp_vstup + w_N1_N0[i, j]*N0_vystup[j];
                        }
                        N1_vstup[i] = temp_vstup + (-1) * w_N1_prah[i];
                        N1_aktivacia[i] = 1 / (1 + System.Math.Exp(-N1_vstup[i] * alfa));
                        N1_vystup[i] = N1_aktivacia[i];
                    }
                    // 2. Skryta vrstva
                    for (i = 0; i < N2; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N1; j++)
                        {
                            temp_vstup = temp_vstup + w_N2_N1[i, j] * N1_vystup[j];
                        }
                        N2_vstup[i] = temp_vstup + (-1) * w_N2_prah[i];
                        N2_aktivacia[i] = 1 / (1 + System.Math.Exp(-N2_vstup[i] * alfa));
                        N2_vystup[i] = N2_aktivacia[i];
                    }
                    // Vystupna vrstva
                    for (i = 0; i < N3; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N2; j++)
                        {
                            temp_vstup = temp_vstup + w_N3_N2[i, j] * N2_vystup[j];
                        }
                        N3_vstup[i] = temp_vstup + (-1) * w_N3_prah[i];
                        N3_aktivacia[i] = 1 / (1 + System.Math.Exp(-N3_vstup[i] * alfa));
                        N3_vystup[i] = N3_aktivacia[i];
                    }
                    // Vypocet chyby pre tuto vzorku             
                    for (i = 0; i < N3; i++)
                    {
                        chyba[cyklus_ucenia] = chyba[cyklus_ucenia] + System.Math.Abs(Train_Input[shuffle[cislo_vzorky], i + N3] - N3_vystup[i]);
                    }                                        
                    // Backpropagation
                    // Vypocet chybovych Delta signalov pred vystupnou vrstvou
                    for (i = 0; i < N3; i++)
                    {
                        delta_N3_N2[i] = 0;
                        delta_N3_N2[i] = (Train_Input[shuffle[cislo_vzorky], i + N3] - N3_vystup[i]) * der_funkcie(N3_vstup[i]);
                    }
                    // Vypocet chybovych Delta signalov pred druhou skrytou vrstvou
                    for (i = 0; i < N2; i++)
                    {
                        delta_N2_N1[i] = 0;
                        for (j = 0; j < N3; j++)
                        {
                            delta_N2_N1[i] = delta_N2_N1[i] + delta_N3_N2[j] * w_N3_N2[j, i];
                        }
                        delta_N2_N1[i] = der_funkcie(N2_vstup[i]) * delta_N2_N1[i];
                    }
                    // Vypocet chybovych Delta signalov pred prvou skrytou vrstvou
                    for (i = 0; i < N1; i++)
                    {
                        delta_N2_N1[i] = 0;
                        for (j = 0; j < N2; j++)
                        {
                            delta_N1_N0[i] = delta_N1_N0[i] + delta_N2_N1[j] * w_N2_N1[j, i];
                        }
                        delta_N1_N0[i] = der_funkcie(N1_vstup[i]) * delta_N1_N0[i];
                    }
                    // Zmena vah pred vystupnou vrstvou                    
                    for (i = 0; i < N3; i++)
                    {
                        for (j = 0; j < N2; j++)
                        {
                            Dw_N3_N2[i, j] = gama * delta_N3_N2[i] * N2_vystup[j];
                        }
                        Dw_N3_prah[i] = gama * delta_N3_N2[i] * -1;
                    }
                    // Zmena vah pred 2. skrytou vrstvou                    
                    for (i = 0; i < N2; i++)
                    {
                        for (j = 0; j < N1; j++)
                        {
                            Dw_N2_N1[i, j] = gama * delta_N2_N1[i] * N1_vystup[j];
                        }
                        Dw_N2_prah[i] = gama * delta_N2_N1[i] * -1;
                    }
                    // Zmena vah pred 1. skrytou vrstvou                    
                    for (i = 0; i < N1; i++)
                    {
                        for (j = 0; j < N0; j++)
                        {
                            Dw_N1_N0[i, j] = gama * delta_N1_N0[i] * N0_vystup[j];
                        }
                        Dw_N1_prah[i] = gama * delta_N1_N0[i] * -1;
                    }
                    // Prepis vah
                    for (i = 0; i < N1; i++)
                    {
                        for (j = 0; j < N0; j++)
                        {
                            w_N1_N0[i, j] = w_N1_N0[i, j] + Dw_N1_N0[i,j];
                        }
                        w_N1_prah[i] = w_N1_prah[i] + Dw_N1_prah[i];
                    }
                    for (i = 0; i < N2; i++)
                    {
                        for (j = 0; j < N1; j++)
                        {
                            w_N2_N1[i, j] = w_N2_N1[i, j] + Dw_N2_N1[i, j];
                        }
                        w_N2_prah[i] = w_N2_prah[i] + Dw_N2_prah[i];
                    }
                    for (i = 0; i < N3; i++)
                    {

                        for (j = 0; j < N2; j++)
                        {
                            w_N3_N2[i, j] = w_N3_N2[i, j] + Dw_N3_N2[i, j];
                        }
                        w_N3_prah[i] = w_N3_prah[i] + Dw_N3_prah[i];
                    }
                }
                // Vypocet chyby v danom cykle ucenia
                chyba[cyklus_ucenia] = chyba[cyklus_ucenia]/(Train_Input_Pocet/N3);
                if (cyklus_ucenia % 10 == 1)
                    progressBar1.PerformStep();
                //DEBUG//MessageBox.Show(string.Format("DEBUG - Chyba v {0}. cykle ucenia = {1}.", cyklus_ucenia, chyba[cyklus_ucenia]));
            }
            // Vykreslenie vystupu na obrazovku
            panel3_Redraw(sender, e);
            // Informacny vypis
            MessageBox.Show(string.Format("Učenie neurónovej siete bolo ukončené: \nPočet testovaných vzoriek: {0} \nPočet správne klasifikovaných vzoriek: {1} \nPočet čiastočne správne klasifikovaných vzoriek: {2} \nPočet nesprávne klasifikovaných vzoriek: {3} \nPresnosť klasifikácie: {4:0.00}% \nKonečná hodnota chyby: {5}", Test_Input_Pocet, poc_spravne_klasifikovanych, poc_ciastocne_spravne_klasifikovanych, poc_nespravne_klasifikovanych, presnost_percent, chyba[cyklus_ucenia - 1]));
        }

        private void panel1_Redraw(object sender, EventArgs e)
        {
            progressBar1.Maximum = Train_Input_Pocet / 100;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            // Vykreslenie vzoriek na obrazovku
            int x_sur;
            int y_sur;
            int input_number; //cislo vzorky
            // Vykreslenie pre trenovaciu mnozinu
            if (Train_Input_Pocet > 0)
            {
                // Prefarbenie plochy na pociatocnu farbu
                System.Drawing.Graphics graphicsObj1;
                graphicsObj1 = panel1.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.White, 600);
                Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
                graphicsObj1.DrawRectangle(myPen, myRectangle);
                // Vykreslenie zadanych vzoriek
                for (input_number = 0; input_number < Train_Input_Pocet; input_number++)
                {
                    x_sur = (int)System.Math.Round(System.Math.Abs(200 * Train_Input[input_number, 0]));
                    y_sur = (int)System.Math.Round(System.Math.Abs(200 * Train_Input[input_number, 1]));
                    graphicsObj1 = panel1.CreateGraphics();
                    if (Train_Input[input_number, 2] == 0) //cerveny
                        myPen = new Pen(System.Drawing.Color.Red, 1);
                    else //modry
                        myPen = new Pen(System.Drawing.Color.Blue, 1);
                    myRectangle = new Rectangle(x_sur - 2, y_sur - 2, 1, 1);
                    graphicsObj1.DrawRectangle(myPen, myRectangle);
                    if (input_number % 100 == 1)
                        progressBar1.PerformStep();
                }
            }
            //DEBUG MessageBox.Show("Prekreslujem stranu.");
        }

        private void panel2_Redraw(object sender, EventArgs e)
        {
            progressBar1.Maximum = Test_Input_Pocet / 100;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            // Vykreslenie vzoriek na obrazovku
            int x_sur;
            int y_sur;
            int input_number; //cislo vzorky
            // Vykreslenie pre testovaciu mnozinu
            if (Test_Input_Pocet > 0)
            {
                // Prefarbenie plochy na pociatocnu farbu
                System.Drawing.Graphics graphicsObj2;
                graphicsObj2 = panel2.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.White, 600);
                Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
                graphicsObj2.DrawRectangle(myPen, myRectangle);
                // Vykreslenie zadanych vzoriek
                for (input_number = 0; input_number < Test_Input_Pocet; input_number++)
                {
                    x_sur = (int)System.Math.Round(System.Math.Abs(200 * Test_Input[input_number, 0]));
                    y_sur = (int)System.Math.Round(System.Math.Abs(200 * Test_Input[input_number, 1]));
                    graphicsObj2 = panel2.CreateGraphics();
                    if (Test_Input[input_number, 2] == 0) //cerveny
                        myPen = new Pen(System.Drawing.Color.Red, 1);
                    else //modry
                        myPen = new Pen(System.Drawing.Color.Blue, 1);
                    myRectangle = new Rectangle(x_sur - 3, y_sur - 3, 1, 1);
                    graphicsObj2.DrawRectangle(myPen, myRectangle);
                    if (input_number % 100 == 1)
                        progressBar1.PerformStep();
                }
            }
        }

        private void panel3_Redraw(object sender, EventArgs e)
        {
            if (cyklov > 0)
            {
                progressBar1.Maximum = Test_Input_Pocet / 100;
                progressBar1.Minimum = 0;
                progressBar1.Value = 0;
                // Vykreslenie vzoriek na obrazovku
                int x_sur;
                int y_sur;
                int cislo_vzorky; //cislo vzorky
                // Prefarbenie plochy na pociatocnu farbu
                System.Drawing.Graphics graphicsObj;
                graphicsObj = panel3.CreateGraphics();
                Pen myPen = new Pen(System.Drawing.Color.White, 600);
                Rectangle myRectangle = new Rectangle(0, 0, 500, 500);
                graphicsObj.DrawRectangle(myPen, myRectangle);
                // Spracovanie vzoriek neuronovou sietou
                int i, j;
                double temp_vstup;
                for (cislo_vzorky = 0; cislo_vzorky < Test_Input_Pocet; cislo_vzorky++) // opakuj pre vsetky vzorky
                {
                    // Vstupna vrstva
                    for (i = 0; i < N0; i++) // opakuj pre vsetky neurony vo vrstve
                    {
                        N0_vystup[i] = Test_Input[cislo_vzorky, i];
                    }
                    // 1. Skryta vrstva
                    for (i = 0; i < N1; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N0; j++)
                        {
                            temp_vstup = temp_vstup + w_N1_N0[i, j] * N0_vystup[j];
                        }
                        N1_vstup[i] = temp_vstup + (-1) * w_N1_prah[i];
                        N1_aktivacia[i] = 1 / (1 + System.Math.Exp(-N1_vstup[i] * alfa));
                        N1_vystup[i] = N1_aktivacia[i];
                    }
                    // 2. Skryta vrstva
                    for (i = 0; i < N2; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N1; j++)
                        {
                            temp_vstup = temp_vstup + w_N2_N1[i, j] * N1_vystup[j];
                        }
                        N2_vstup[i] = temp_vstup + (-1) * w_N2_prah[i];
                        N2_aktivacia[i] = 1 / (1 + System.Math.Exp(-N2_vstup[i] * alfa));
                        N2_vystup[i] = N2_aktivacia[i];
                    }
                    // Vystupna vrstva
                    for (i = 0; i < N3; i++)
                    {
                        temp_vstup = 0;
                        for (j = 0; j < N2; j++)
                        {
                            temp_vstup = temp_vstup + w_N3_N2[i, j] * N2_vystup[j];
                        }
                        N3_vstup[i] = temp_vstup + (-1) * w_N3_prah[i];
                        N3_aktivacia[i] = 1 / (1 + System.Math.Exp(-N3_vstup[i] * alfa));
                        N3_vystup[i] = N3_aktivacia[i];
                    }
                    // Vykreslenie
                    x_sur = (int)System.Math.Round(System.Math.Abs((double)350 * N0_vystup[0]));
                    y_sur = (int)System.Math.Round(System.Math.Abs((double)350 * N0_vystup[1]));
                    graphicsObj = panel3.CreateGraphics();
                    if ((System.Math.Round(N3_vystup[0]) == 1)&&(System.Math.Round(N3_vystup[1]) == 1)) //zlty (chybna klasifikacia do oboch tried)
                        myPen = new Pen(System.Drawing.Color.Yellow, 3);
                    else if (System.Math.Round(N3_vystup[0]) == 1) //cerveny
                        myPen = new Pen(System.Drawing.Color.Blue, 3);
                    else if (System.Math.Round(N3_vystup[1]) == 1) //modry
                        myPen = new Pen(System.Drawing.Color.Red, 3);
                    myRectangle = new Rectangle(x_sur - 3, y_sur - 3, 1, 1);
                    graphicsObj.DrawRectangle(myPen, myRectangle);
                    // Vypocet presnosti
                    if ((System.Math.Round(N3_vystup[0]) == Test_Input[cislo_vzorky, N0]) && (System.Math.Round(N3_vystup[1]) == Test_Input[cislo_vzorky, N0 + 1]))
                    {
                        poc_spravne_klasifikovanych++;
                        presnost_percent++; //za 1. triedu
                        presnost_percent++; //za 2. triedu
                    }
                    else if (System.Math.Round(N3_vystup[0]) == Test_Input[cislo_vzorky, N0])
                    {
                        poc_ciastocne_spravne_klasifikovanych++;
                        presnost_percent++;
                    }
                    else if (System.Math.Round(N3_vystup[1]) == Test_Input[cislo_vzorky, N0 + 1])
                    {
                        poc_ciastocne_spravne_klasifikovanych++;
                        presnost_percent++;
                    }
                    else
                        poc_nespravne_klasifikovanych++;
                }
                presnost_percent = 100 * presnost_percent / (Test_Input_Pocet * N3);
                Font myFont = new System.Drawing.Font("Arial", 10, FontStyle.Regular);
                Brush myBrush = new SolidBrush(System.Drawing.Color.Black);
                graphicsObj.DrawString(string.Format("Presnost: {0:0.00}%", presnost_percent), myFont, myBrush, 240, 10);
            }
        }

        private void panel4_Redraw(object sender, EventArgs e)
        {
            int i;
            progressBar1.Maximum = cyklov / 100;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            // Prefarbenie plochy na pociatocnu farbu
            System.Drawing.Graphics graphicsObj;
            graphicsObj = panel4.CreateGraphics();
            Pen myPen = new Pen(System.Drawing.Color.White, 600);
            Rectangle myRectangle = new Rectangle(0, 0, 600, 600);
            graphicsObj.DrawRectangle(myPen, myRectangle);
            // Vykreslenie hranic
            myPen = new Pen(System.Drawing.Color.Black);
            graphicsObj.DrawLine(myPen, 30, 0, 30, 450); //lava
            graphicsObj.DrawLine(myPen, 30, 449, 629, 449); //dolna
            graphicsObj.DrawLine(myPen, 629, 0, 629, 450); //prava
            graphicsObj.DrawLine(myPen, 30, 0, 630, 0); //horna
            // Vykreslenie mriezky
            myPen = new Pen(System.Drawing.Color.Gray, 1);
            // Vodorovne
            for (i = 0; i < 450; i = i + 10)
            {
                graphicsObj.DrawLine(myPen, 25, i - 1, 628, i - 1);
            }
            // Zvislo
            for (i = 40; i < 630; i = i + 10)
            {
                graphicsObj.DrawLine(myPen, i - 1, 0, i - 1, 448);
            }
            // Hodnoty suradnic
            Font myFont = new System.Drawing.Font("Arial", 9, FontStyle.Regular);
            Brush myBrush = new SolidBrush(System.Drawing.Color.Black);
            for (i = 10; i < 450; i = i + 20)
            {
                graphicsObj.DrawString(string.Format("{0:0.00}", 4.5 * (float)i / (float)450), myFont, myBrush, 0, 441 - i);
            }
            if (cyklov > 0)
            {
                // Vykreslenie bodov na obrazovku
                int x_sur, x_sur_pr;
                int y_sur, y_sur_pr;
                // Vykreslenie (niektorych) bodov chyby
                x_sur = 30;
                y_sur = 450;
                x_sur_pr = 30;
                y_sur_pr = 450;
                myPen = new Pen(System.Drawing.Color.Black, 1);
                for (i = 0; i < cyklov; i++)
                {
                    x_sur = (int)System.Math.Round(30+(float)i*(float)600/(float)cyklov);
                    y_sur = (int)System.Math.Round(450 - 100*chyba[i]);
                    graphicsObj.DrawLine(myPen, x_sur_pr, y_sur_pr, x_sur, y_sur);
                    x_sur_pr = x_sur;
                    y_sur_pr = y_sur;
                    if (i % 100 == 1)
                        progressBar1.PerformStep();
                }
                // Zobrazenie poctu cyklov
                label9.Text = string.Format("Počet cyklov: {0}", cyklov);
            }
        }
    }
}
