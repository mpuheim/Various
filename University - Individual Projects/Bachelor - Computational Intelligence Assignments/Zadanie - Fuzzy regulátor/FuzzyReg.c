//Fuzzy regulator - verzia 1.0 - M.PUHEIM 2011

#include <stdlib.h>
#include <stdio.h>

float minimum(float a, float b)
{
      if (a > b)
         return b;
      else
         return a;
}

main()
{
      /* Deklaracia premennych */
      int i;
      float jedlo_in, servis_in;
      float jedlo_m_zle, jedlo_m_dobre, jedlo_m_super;
      float servis_m_zle, servis_m_dobre, servis_m_super;
      float w[9],out[9],w_sum,out_sum,output;
      
      /* Nacitani vstupov */
      printf("Zadajte uroven kvality jedla ako hodnotu na intervale [0..1]: ");
      scanf("%f",&jedlo_in);
      printf("Zadajte uroven kvality servisu ako hodnotu na intervale [0..1]: ");
      scanf("%f",&servis_in);
      
      /* Vypocet hodnot funkcii prislusnosti */
      jedlo_m_zle = 0;
      jedlo_m_dobre = 0;
      jedlo_m_super = 0;
      if (jedlo_in < 0.5)
      {
         jedlo_m_zle = 1 - (jedlo_in/0.5);
         if (jedlo_in > 0.25)
            jedlo_m_dobre = (jedlo_in - 0.25)/0.25;
      }
      if (jedlo_in >= 0.5)
      {
         jedlo_m_super = (jedlo_in - 0.5)/0.5;
         if (jedlo_in < 0.75)
            jedlo_m_dobre = 1 - ((jedlo_in - 0.25)/0.25);
      }
      printf ("Hodnoty funkcii prislusnosti pre jedlo: zle=%0.2f dobre=%0.2f super =%0.2f\n",jedlo_m_zle,jedlo_m_dobre,jedlo_m_super);
      servis_m_zle = 0;
      servis_m_dobre = 0;
      servis_m_super = 0;
      if (servis_in < 0.5)
      {
         servis_m_zle = 1 - (servis_in/0.5);
         if (servis_in > 0.25)
            servis_m_dobre = (servis_in - 0.25)/0.25;
      }
      if (servis_in >= 0.5)
      {
         servis_m_super = (servis_in - 0.5)/0.5;
         if (servis_in < 0.75)
            servis_m_dobre = 1 - ((servis_in - 0.5)/0.25);
      }
      printf ("Hodnoty funkcii prislusnosti pre servis: zle=%0.2f dobre=%0.2f super =%0.2f\n",servis_m_zle,servis_m_dobre,servis_m_super);
      getchar();getchar();
      
      /* Pravidla */
      for (i = 0; i < 9; i++)
      {
          w[i] = 0;
          out[i] = 0;
      }
      if ((jedlo_m_zle>0)&&(servis_m_zle>0))
      {
         w[0]=minimum(jedlo_m_zle,servis_m_zle);
         out[0]=0;
         printf("Pravidlo 0: w=%0.2f out=%0.2f w*out=%0.2f\n",w[0],out[0],w[0]*out[0]);
      }
      if ((jedlo_m_zle>0)&&(servis_m_dobre>0))
      {
         w[1]=minimum(jedlo_m_zle,servis_m_dobre);
         out[1]=0.25;
         printf("Pravidlo 1: w=%0.2f out=%0.2f w*out=%0.2f\n",w[1],out[1],w[1]*out[1]);
      }
      if ((jedlo_m_zle>0)&&(servis_m_super>0))
      {
         w[2]=minimum(jedlo_m_zle,servis_m_super);
         out[2]=0.5;
         printf("Pravidlo 2: w=%0.2f out=%0.2f w*out=%0.2f\n",w[2],out[2],w[2]*out[2]);
      }
      if ((jedlo_m_dobre>0)&&(servis_m_zle>0))
      {
         w[3]=minimum(jedlo_m_dobre,servis_m_zle);
         out[3]=0.25;
         printf("Pravidlo 3: w=%0.2f out=%0.2f w*out=%0.2f\n",w[3],out[3],w[3]*out[3]);
      }
      if ((jedlo_m_dobre>0)&&(servis_m_dobre>0))
      {
         w[4]=minimum(jedlo_m_dobre,servis_m_dobre);
         out[4]=0.5;
         printf("Pravidlo 4: w=%0.2f out=%0.2f w*out=%0.2f\n",w[4],out[4],w[4]*out[4]);
      }
      if ((jedlo_m_dobre>0)&&(servis_m_super>0))
      {
         w[5]=minimum(jedlo_m_dobre,servis_m_super);
         out[5]=0.75;
         printf("Pravidlo 5: w=%0.2f out=%0.2f w*out=%0.2f\n",w[5],out[5],w[5]*out[5]);
      }
      if ((jedlo_m_super>0)&&(servis_m_zle>0))
      {
         w[6]=minimum(jedlo_m_super,servis_m_zle);
         out[6]=0.5;
         printf("Pravidlo 6: w=%0.2f out=%0.2f w*out=%0.2f\n",w[6],out[6],w[6]*out[6]);
      }
      if ((jedlo_m_super>0)&&(servis_m_dobre>0))
      {
         w[7]=minimum(jedlo_m_super,servis_m_dobre);
         out[7]=0.75;
         printf("Pravidlo 7: w=%0.2f out=%0.2f w*out=%0.2f\n",w[7],out[7],w[7]*out[7]);
      }
      if ((jedlo_m_super>0)&&(servis_m_super>0))
      {
         w[8]=minimum(jedlo_m_super,servis_m_super);
         out[8]=1;
         printf("Pravidlo 8: w=%0.2f out=%0.2f w*out=%0.2f\n",w[8],out[8],w[8]*out[8]);
      }
      
      /* Vypocet */
      w_sum = 0;
      out_sum = 0;
      for (i = 0; i < 9; i++)
      {
          w_sum += w[i];
          out_sum += w[i]*out[i];
      }
      output = (out_sum)/w_sum;
      
      printf("Vystup je %0.2f",output);
      getchar();
}
