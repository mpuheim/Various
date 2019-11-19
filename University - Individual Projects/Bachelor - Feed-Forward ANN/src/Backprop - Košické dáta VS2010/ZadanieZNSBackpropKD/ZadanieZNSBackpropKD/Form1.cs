using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ZadanieZNSBackpropKD
{
    public partial class Form1 : Form
    {
        const int MAX_PATTERNS = 1000000;
        int Train_Input_Pocet, Test_Input_Pocet, Cely_Input_Pocet; // pocet vzoriek
        double[,] Train_Input = new double[MAX_PATTERNS, 6]; //trenovacia mnozina VSTUP
        double[,] Train_Output = new double[MAX_PATTERNS, 7]; //trenovacia mnozina VYSTUP
        double[,] Test_Input = new double[MAX_PATTERNS, 6]; //testovacia mnozina VSTUP
        double[,] Test_Output = new double[MAX_PATTERNS, 7]; //trenovacia mnozina VYSTUP
        double[,] Cely_Input = new double[MAX_PATTERNS, 6]; //vstup zo suboru cely.txt
        double[,] Cely_Output = new double[MAX_PATTERNS, 7]; //vystup pre export
        int[] shuffle = new int[MAX_PATTERNS];
        int N0, N1, N2, N3; // pocty neuronov na jednotlivych vrstvach
        double[] N0_vystup = new double[6];
        double[] N1_vstup = new double[100];
        double[] N1_aktivacia = new double[100];
        double[] N1_vystup = new double[100];
        double[] N2_vstup = new double[100];
        double[] N2_aktivacia = new double[100];
        double[] N2_vystup = new double[100];
        double[] N3_vstup = new double[7];
        double[] N3_aktivacia = new double[7];
        double[] N3_vystup = new double[7];
        double[,] w_N1_N0 = new double[100, 6];    //vahy medzi neuronmi vrstiev N1 a N0
        double[,] w_N2_N1 = new double[100, 100]; //vahy medzi neuronmi vrstiev N2 a N1
        double[,] w_N3_N2 = new double[7, 100];   //vahy medzi neuronmi vrstiev N3 a N2
        double[] w_N1_prah = new double[100];     //vahy na prahu neuronov vrstvy N1
        double[] w_N2_prah = new double[100];     //vahy na prahu neuronov vrstvy N2
        double[] w_N3_prah = new double[7];       //vahy na prahu neuronov vrstvy N3
        double[,] Dw_N1_N0 = new double[100, 6];   //zmena vahy medzi neuronmi vrstiev N1 a N0
        double[,] Dw_N2_N1 = new double[100, 100]; //zmena vahy medzi neuronmi vrstiev N2 a N1
        double[,] Dw_N3_N2 = new double[7, 100];   //zmena vahy medzi neuronmi vrstiev N3 a N2
        double[] Dw_N1_prah = new double[100];     //zmena vahy na prahu neuronov vrstvy N1
        double[] Dw_N2_prah = new double[100];     //zmena vahy na prahu neuronov vrstvy N2
        double[] Dw_N3_prah = new double[7];       //zmena vahy na prahu neuronov vrstvy N3
        double[] delta_N1_N0 = new double[100];      //chybovy signa pre neurony vrstvy N1
        double[] delta_N2_N1 = new double[100];      //chybovy signa pre neurony vrstvy N2
        double[] delta_N3_N2 = new double[100];      //chybovy signa pre neurony vrstvy N3
        double[] chyba = new double[100000];      //pole chyby ucenia v kazdom cykle ucenia
        double alfa; //parameter sigmoidalnej aktivacnej funkcie alfa
        double gama; //parameter ucenia gama
        int cyklov;  //celkovy pocet cyklov ucenia
        double presnost_percent;
        int poc_nespravne_klasifikovanych;
        int pracujem = 0;

        public Form1()
        {
            InitializeComponent();
            this.panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.panel1_Redraw);
        }

        private double der_funkcie(double vstup)
        {
            return ((alfa * System.Math.Exp(-alfa * vstup)) / ((1 + System.Math.Exp(-alfa * vstup)) * (1 + System.Math.Exp(-alfa * vstup))));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (pracujem == 1)
                return;
            pracujem = 1;
            int i, j, cislo_vzorky, cyklus_ucenia, pom;
            double temp_vstup;
            //Otvorenie suboru s trenovacou mnozinou
            string Train_Pat_Path;
            Train_Pat_Path = System.Windows.Forms.Application.StartupPath + "\\train.pat";
            //MessageBox.Show(string.Format("DEBUG \n {0}", Train_Pat_Path));
            // Nacitanie suboru
            if (System.IO.File.Exists(Train_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Train_Pat_Path);
                string line;
                // Preskocenie prvych 7 riadkov
                for (i = 0; i < 7; i++)
                    line = objReader.ReadLine();
                // Nacitanie vzoriek
                int input_number = 0; //cislo vzorky
                int input_data = 0;
                while ((line = objReader.ReadLine()) != null)
                {
                    line = line.Replace(".", ",");
                    if (input_data == 0)
                    {
                        try
                        {
                            Train_Input[input_number, 0] = Convert.ToDouble(line.Substring(0, 8));
                            Train_Input[input_number, 1] = Convert.ToDouble(line.Substring(8, 8));
                            Train_Input[input_number, 2] = Convert.ToDouble(line.Substring(17, 8));
                            Train_Input[input_number, 3] = Convert.ToDouble(line.Substring(26, 8));
                            Train_Input[input_number, 4] = Convert.ToDouble(line.Substring(35, 8));
                            Train_Input[input_number, 5] = Convert.ToDouble(line.Substring(44, 8));
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
                    }
                    else if (input_data == 1)
                    {
                        try
                        {
                            Train_Output[input_number, 0] = Convert.ToDouble(line.Substring(0, 8));
                            Train_Output[input_number, 1] = Convert.ToDouble(line.Substring(8, 8));
                            Train_Output[input_number, 2] = Convert.ToDouble(line.Substring(17, 8));
                            Train_Output[input_number, 3] = Convert.ToDouble(line.Substring(26, 8));
                            Train_Output[input_number, 4] = Convert.ToDouble(line.Substring(35, 8));
                            Train_Output[input_number, 5] = Convert.ToDouble(line.Substring(44, 8));
                            Train_Output[input_number, 6] = Convert.ToDouble(line.Substring(53, 8));
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
                    }
                    if (input_data++ > 1)
                    {
                        input_data = 0;
                        input_number++;
                    }
                }
                objReader.Close();
                Train_Input_Pocet = input_number;
            }
            else
            {
                MessageBox.Show(string.Format("Súbor nebol nájdený v lokácii '{0}'", Train_Pat_Path));
                return;
            }
            //Nacitanie parametrov z kontroliek
            N0 = 6;
            N1 = (int)this.numericUpDown3.Value;
            N2 = (int)this.numericUpDown4.Value;
            N3 = 7;
            alfa = (double)this.numericUpDown1.Value;
            gama = (double)this.numericUpDown2.Value;
            cyklov = (int)this.numericUpDown5.Value;
            progressBar1.Maximum = cyklov / 10;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            //Inicializacia vah
            Random r = new Random();
            //Vahy medzi N0 a N1
            for (i = 0; i < N1; i++)
            {
                for (j = 0; j < N0; j++)
                {
                    w_N1_N0[i, j] = 1 - 2 * r.NextDouble();
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
            poc_nespravne_klasifikovanych = 0;
            for (cyklus_ucenia = 0; cyklus_ucenia < cyklov; cyklus_ucenia++) // opakuj pre vsetky cykly ucenia
            {
                chyba[cyklus_ucenia] = 0;
                // Poprehadzovanie poradia vzoriek
                for (i = 0; i < MAX_PATTERNS; i++)
                {
                    shuffle[i] = -1;
                }
                Random ra = new Random();
                for (i = 0; i < Train_Input_Pocet; i++)
                {
                    pom = ra.Next(Train_Input_Pocet - 1);
                    int opakuje_sa = 0;
                    for (j = 0; j < Train_Input_Pocet; j++)
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
                for (cislo_vzorky = 0; cislo_vzorky < Train_Input_Pocet; cislo_vzorky++) // opakuj pre vsetky vzorky
                {
                    Application.DoEvents();
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
                        //DEBUG MessageBox.Show(string.Format("Vystup[{0}]: {1}", i, N3_vystup[i]));
                    }
                    // Vypocet chyby pre tuto vzorku             
                    for (i = 0; i < N3; i++)
                    {
                        chyba[cyklus_ucenia] = chyba[cyklus_ucenia] + System.Math.Abs(Train_Output[shuffle[cislo_vzorky], i] - N3_vystup[i]);
                    }
                    // Backpropagation
                    // Vypocet chybovych Delta signalov pred vystupnou vrstvou
                    for (i = 0; i < N3; i++)
                    {
                        delta_N3_N2[i] = 0;
                        delta_N3_N2[i] = (Train_Output[shuffle[cislo_vzorky], i] - N3_vystup[i]) * der_funkcie(N3_vstup[i]);
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
                            w_N1_N0[i, j] = w_N1_N0[i, j] + Dw_N1_N0[i, j];
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
                chyba[cyklus_ucenia] = chyba[cyklus_ucenia] / (Train_Input_Pocet * N3);
                if (cyklus_ucenia % 10 == 1)  
                    progressBar1.PerformStep();
                //DEBUG//MessageBox.Show(string.Format("DEBUG - Chyba v {0}. cykle ucenia = {1}.", cyklus_ucenia, chyba[cyklus_ucenia]));
            }
            pracujem = 0;
            // Informacny vypis
            panel1_Redraw(sender, e);
            MessageBox.Show(string.Format("Učenie neurónovej siete bolo ukončené.\n\n Konečná hodnota chyby: {0:0.0000}", chyba[cyklus_ucenia - 1]));
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (pracujem == 1)
                return;
            pracujem = 1;
            int i, j, cislo_vzorky;
            double temp_vstup, err;
            //Otvorenie suboru s testovacou mnozinou
            string Test_Pat_Path;
            Test_Pat_Path = System.Windows.Forms.Application.StartupPath + "\\test.pat";
            //MessageBox.Show(string.Format("DEBUG \n {0}", Test_Pat_Path));
            // Nacitanie suboru
            if (System.IO.File.Exists(Test_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Test_Pat_Path);
                string line;
                // Preskocenie prvych 7 riadkov
                for (i = 0; i < 7; i++)
                    line = objReader.ReadLine();
                // Nacitanie vzoriek
                int input_number = 0; //cislo vzorky
                int input_data = 0;
                while ((line = objReader.ReadLine()) != null)
                {
                    line = line.Replace(".", ",");
                    if (input_data == 0)
                    {
                        try
                        {
                            Test_Input[input_number, 0] = Convert.ToDouble(line.Substring(0, 8));
                            Test_Input[input_number, 1] = Convert.ToDouble(line.Substring(8, 8));
                            Test_Input[input_number, 2] = Convert.ToDouble(line.Substring(17, 8));
                            Test_Input[input_number, 3] = Convert.ToDouble(line.Substring(26, 8));
                            Test_Input[input_number, 4] = Convert.ToDouble(line.Substring(35, 8));
                            Test_Input[input_number, 5] = Convert.ToDouble(line.Substring(44, 8));
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
                    }
                    else if (input_data == 1)
                    {
                        try
                        {
                            Test_Output[input_number, 0] = Convert.ToDouble(line.Substring(0, 8));
                            Test_Output[input_number, 1] = Convert.ToDouble(line.Substring(8, 8));
                            Test_Output[input_number, 2] = Convert.ToDouble(line.Substring(17, 8));
                            Test_Output[input_number, 3] = Convert.ToDouble(line.Substring(26, 8));
                            Test_Output[input_number, 4] = Convert.ToDouble(line.Substring(35, 8));
                            Test_Output[input_number, 5] = Convert.ToDouble(line.Substring(44, 8));
                            Test_Output[input_number, 6] = Convert.ToDouble(line.Substring(53, 8));
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
                    }
                    if (input_data++ > 1)
                    {
                        input_data = 0;
                        input_number++;
                    }
                }
                objReader.Close();
                Test_Input_Pocet = input_number;
            }
            else
            {
                MessageBox.Show(string.Format("Súbor nebol nájdený v lokácii '{0}'", Test_Pat_Path));
                return;
            }
            //Spracovanie vstupnych udajov v neuronovej sieti
            double akt_err = 0;
            err = 0;
            poc_nespravne_klasifikovanych = 0;
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
                // Vypocet chyby pre tuto vzorku
                akt_err = 0;
                for (i = 0; i < N3; i++)
                {
                    akt_err += System.Math.Abs(Test_Output[cislo_vzorky, i] - N3_vystup[i]);                                        
                }
                if (akt_err > 0.5)
                    poc_nespravne_klasifikovanych++;
                err = err + akt_err;
            }
            err = err / (Test_Input_Pocet * N3);
            presnost_percent = 100 * (Test_Input_Pocet - poc_nespravne_klasifikovanych) / Test_Input_Pocet;
            MessageBox.Show(string.Format("Testovanie neurónovej siete bolo ukončené s chybou: {0:0.0000} \nPresnosť klasifikácie: {1:0.00}% \n Počet nesprávne klasifikovaných vzoriek: {2}", err, presnost_percent, poc_nespravne_klasifikovanych));
            pracujem = 0;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (pracujem == 1)
                return;
            pracujem = 1;
            int i, j, cislo_vzorky;
            double temp_vstup;
            //Otvorenie suboru s testovacou mnozinou
            string Cely_Pat_Path;
            Cely_Pat_Path = System.Windows.Forms.Application.StartupPath + "\\cely.txt";
            //MessageBox.Show(string.Format("DEBUG \n {0}", Cely_Pat_Path));
            // Nacitanie suboru
            if (System.IO.File.Exists(Cely_Pat_Path) == true)
            {
                System.IO.StreamReader objReader;
                objReader = new System.IO.StreamReader(Cely_Pat_Path);
                string line;
                // Preskocenie prvych 7 riadkov
                for (i = 0; i < 7; i++)
                    line = objReader.ReadLine();
                // Nacitanie vzoriek
                int input_number = 0; //cislo vzorky
                int input_data = 0;
                while ((line = objReader.ReadLine()) != null)
                {
                    line = line.Replace(".", ",");
                    if (input_data == 0)
                    {
                        try
                        {
                            Cely_Input[input_number, 0] = Convert.ToDouble(line.Substring(0, 8));
                            Cely_Input[input_number, 1] = Convert.ToDouble(line.Substring(8, 8));
                            Cely_Input[input_number, 2] = Convert.ToDouble(line.Substring(17, 8));
                            Cely_Input[input_number, 3] = Convert.ToDouble(line.Substring(26, 8));
                            Cely_Input[input_number, 4] = Convert.ToDouble(line.Substring(35, 8));
                            Cely_Input[input_number, 5] = Convert.ToDouble(line.Substring(44, 8));
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
                    }
                    if (input_data++ > 1)
                    {
                        input_data = 0;
                        input_number++;
                    }
                }
                objReader.Close();
                Cely_Input_Pocet = input_number;
            }
            else
            {
                MessageBox.Show(string.Format("Súbor nebol nájdený v lokácii '{0}'", Cely_Pat_Path));
                return;
            }
            //Spracovanie vstupnych udajov v neuronovej sieti
            for (cislo_vzorky = 0; cislo_vzorky < Cely_Input_Pocet; cislo_vzorky++) // opakuj pre vsetky vzorky
            {
                // Vstupna vrstva
                for (i = 0; i < N0; i++) // opakuj pre vsetky neurony vo vrstve
                {
                    N0_vystup[i] = Cely_Input[cislo_vzorky, i];
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
                    Cely_Output[cislo_vzorky, i] = N3_vystup[i];
                }
            } 
            //Export do suboru
            this.saveFileDialog1.InitialDirectory = System.Windows.Forms.Application.StartupPath;
            this.saveFileDialog1.Filter = "(*.ppm)|*.ppm";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                progressBar1.Maximum = Cely_Input_Pocet / 100;
                progressBar1.Minimum = 0;
                progressBar1.Value = 0;
                System.IO.StreamWriter write = new System.IO.StreamWriter(saveFileDialog1.FileName);
                write.Write("P3\n775 475\n255\n");
                //MessageBox.Show(string.Format("DEBUG {0}", saveFileDialog1.FileName));
                for (cislo_vzorky = 0; cislo_vzorky < Cely_Input_Pocet; cislo_vzorky++)
                {
                    if (cislo_vzorky % 100 == 1)
                        progressBar1.PerformStep();
                    if (Cely_Output[cislo_vzorky, 0] > 0.5)
                    {
                        write.Write("210 26 26\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 1] > 0.5)
                    {
                        write.Write("0 0 0\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 2] > 0.5)
                    {
                        write.Write("30 232 25\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 3] > 0.5)
                    {
                        write.Write("162 126 29\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 4] > 0.5)
                    {
                        write.Write("243 236 45\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 5] > 0.5)
                    {
                        write.Write("49 124 47\n");
                        continue;
                    }
                    if (Cely_Output[cislo_vzorky, 6] > 0.5)
                    {
                        write.Write("26 26 188\n");
                        continue;
                    }
                    //Ak nevie klasifikovať
                    write.Write("255 255 255\n");

                }
                write.Close();
            }
            MessageBox.Show("Export bol ukončený");
            pracujem = 0;
        }

        private void panel1_Redraw(object sender, EventArgs e)
        {
            if (pracujem == 1)
                return;
            pracujem = 1;
            int i;
            progressBar1.Maximum = cyklov / 10;
            progressBar1.Minimum = 0;
            progressBar1.Value = 0;
            // Prefarbenie plochy na pociatocnu farbu
            System.Drawing.Graphics graphicsObj;
            graphicsObj = panel1.CreateGraphics();
            Pen myPen = new Pen(System.Drawing.Color.White, 600);
            Rectangle myRectangle = new Rectangle(0, 0, 600, 600);
            graphicsObj.DrawRectangle(myPen, myRectangle);
            // Vykreslenie hranic
            myPen = new Pen(System.Drawing.Color.Black);
            graphicsObj.DrawLine(myPen, 30, 0, 30, 450); //lava
            graphicsObj.DrawLine(myPen, 30, 449, 449, 449); //dolna
            graphicsObj.DrawLine(myPen, 449, 0, 449, 450); //prava
            graphicsObj.DrawLine(myPen, 30, 0, 450, 0); //horna
            // Vykreslenie mriezky
            myPen = new Pen(System.Drawing.Color.Gray, 1);
            // Vodorovne
            for (i = 0; i < 450; i = i + 10)
            {
                graphicsObj.DrawLine(myPen, 25, i - 1, 448, i - 1);
            }
            // Zvislo
            for (i = 40; i < 448; i = i + 10)
            {
                graphicsObj.DrawLine(myPen, i - 1, 0, i - 1, 448);
            }
            // Hodnoty suradnic
            Font myFont = new System.Drawing.Font("Arial", 9, FontStyle.Regular);
            Brush myBrush = new SolidBrush(System.Drawing.Color.Black);
            for (i = 10; i < 450; i = i + 20)
            {
                graphicsObj.DrawString(string.Format("{0:0.00}", 0.45 * (float)i / (float)450), myFont, myBrush, 0, 441 - i);
            }
            if (cyklov > 0)
            {
                // Vykreslenie bodov na obrazovku
                int x_sur, x_sur_pr;
                int y_sur, y_sur_pr;
                // Vykreslenie bodov chyby
                x_sur = 30;
                y_sur = 450;
                x_sur_pr = 30;
                y_sur_pr = 450;
                myPen = new Pen(System.Drawing.Color.Black, 1);
                for (i = 0; i < cyklov; i++)
                {
                    x_sur = (int)System.Math.Round(30 + (float)i * (float)450 / (float)cyklov);
                    y_sur = (int)System.Math.Round(450 - 1000 * chyba[i]);
                    graphicsObj.DrawLine(myPen, x_sur_pr, y_sur_pr, x_sur, y_sur);
                    x_sur_pr = x_sur;
                    y_sur_pr = y_sur;
                    if (i % 10 == 1)
                        progressBar1.PerformStep();
                }
                // Zobrazenie poctu cyklov
                label10.Text = string.Format("Počet cyklov: {0}", cyklov);
            }
            pracujem = 0;
        }
    }
}
