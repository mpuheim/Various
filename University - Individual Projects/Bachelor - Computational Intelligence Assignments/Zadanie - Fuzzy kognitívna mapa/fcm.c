//Fuzzy cognitive map - Sample version - M.PUHEIM 2011

#include <stdio.h>


//Definicia struktury kognitivnej mapy
typedef struct
{
    double uroven_zrazok;
    double uroven_zatravnenia;
    double pocet_nosorozcov;
    double pocet_pytliakov;
    double pocet_ochranarov;
} cm_t;

//Definicia vplyvov medzi uzlami kognitivnej mapy
#define zrazky_zatravnenie       0.8
#define nosorozce_zatravnenie   -0.5
#define zatravnenie_nosorozce     0.5
#define pytliaci_nosorozce      -0.5
#define nosorozce_pytliaci       0.2
#define ochranari_pytliaci      -0.1
#define nosorozce_ochranari     -0.1
#define pytliaci_ochranari       0.1

//Prepocet hodnot fuzzy kognitivnej mapy
cm_t aktualizacia_fcm(cm_t cm)
{
    //generuj novu mapu
    cm_t nova_mapa;
    nova_mapa.uroven_zrazok = cm.uroven_zrazok;
    nova_mapa.uroven_zatravnenia = cm.uroven_zatravnenia + cm.uroven_zrazok * zrazky_zatravnenie + cm.pocet_nosorozcov * nosorozce_zatravnenie;
    nova_mapa.pocet_nosorozcov = cm.pocet_nosorozcov + cm.uroven_zatravnenia * zatravnenie_nosorozce  + cm.pocet_pytliakov * pytliaci_nosorozce;
    nova_mapa.pocet_pytliakov = cm.pocet_pytliakov + cm.pocet_nosorozcov * nosorozce_pytliaci + cm.pocet_ochranarov * ochranari_pytliaci;
    nova_mapa.pocet_ochranarov = cm.pocet_ochranarov + cm.pocet_nosorozcov * nosorozce_ochranari + cm.pocet_pytliakov * pytliaci_ochranari;
    //ohranicenia
    if (nova_mapa.uroven_zrazok > 100)
        nova_mapa.uroven_zrazok = 100;
    if (nova_mapa.uroven_zrazok < 0)
        nova_mapa.uroven_zrazok = 0;
    if (nova_mapa.uroven_zatravnenia > 100)
        nova_mapa.uroven_zatravnenia = 100;
    if (nova_mapa.uroven_zatravnenia < 0)
        nova_mapa.uroven_zatravnenia = 0;
    if (nova_mapa.pocet_nosorozcov < 0)
        nova_mapa.pocet_nosorozcov = 0;
    if (nova_mapa.pocet_pytliakov < 0)
        nova_mapa.pocet_pytliakov = 0;
    if (nova_mapa.pocet_ochranarov < 0)
        nova_mapa.pocet_ochranarov = 0;
    //prepis hodnoty starej mapy hodnotami novej mapy
    return nova_mapa;
}

//Hlavna funkcia
main()
{
    int max_gen,i;
    cm_t cm;
    //Nacitanie vstupnych udajov
    printf("Fuzzy kognitivna mapa. Michal Puheim 2011.\nZadajte vstupne udaje:\n");
    printf("Uroven zrazok (0-100): ");
    while (scanf("%lf",&(cm.uroven_zrazok))==0||cm.uroven_zrazok<0||cm.uroven_zrazok>100)
    {
        getchar();
        printf("Uroven zrazok (0-100): ");
    }
    getchar();
    printf("Uroven zatravnenia (0-100): ");
    while (scanf("%lf",&(cm.uroven_zatravnenia))==0||cm.uroven_zatravnenia<0||cm.uroven_zatravnenia>100)
    {
        getchar();
        printf("Uroven zatravnenia (0-100): ");
    }
    getchar();
    printf("Pocet nosorozcov: ");
    while (scanf("%lf",&(cm.pocet_nosorozcov))==0||cm.pocet_nosorozcov<0)
    {
        getchar();
        printf("Pocet nosorozcov: ");
    }
    getchar();
    printf("Pocet pytliakov: ");
    while (scanf("%lf",&(cm.pocet_pytliakov))==0||cm.pocet_pytliakov<0)
    {
        getchar();
        printf("Pocet pytliakov: ");
    }
    getchar();
    printf("Pocet ochranarov: ");
    while (scanf("%lf",&(cm.pocet_ochranarov))==0||cm.pocet_ochranarov<0)
    {
        getchar();
        printf("Pocet ochranarov: ");
    }
    getchar();
    printf("Pocet generacii evolucie kognitivnej mapy: ");
    while (scanf("%d",&max_gen)==0||max_gen<=0)
    {
        getchar();
        printf("Pocet generacii evolucie kognitivnej mapy: ");
    }
    getchar();
    
    //Informacny vypis
    printf("\nKognitivna mapa bude spustena s nasledovnymi parametrami:\n");
    printf("Uroven zrazok:      %4.0lf\n",cm.uroven_zrazok);
    printf("Uroven zatravnenia: %4.0lf\n",cm.uroven_zatravnenia);
    printf("Pocet nosorozcov:   %4.0lf\n",cm.pocet_nosorozcov);
    printf("Pocet pytliakov:    %4.0lf\n",cm.pocet_pytliakov);
    printf("Pocet ochranarov:   %4.0lf\n",cm.pocet_ochranarov);
    printf("Pocet cyklov adaptacie: %d\n",max_gen);
    getchar();
    
    //Adaptacia mapy
    printf("Spustam adaptaciu mapy: ");
    for (i=0; i<max_gen; i++)
    {
        //Informacny vypis
        printf("%d. generacia:\n",i);
        printf("Uroven zrazok:      %4.0lf\n",cm.uroven_zrazok);
        printf("Uroven zatravnenia: %4.0lf\n",cm.uroven_zatravnenia);
        printf("Pocet nosorozcov:   %4.0lf\n",cm.pocet_nosorozcov);
        printf("Pocet pytliakov:    %4.0lf\n",cm.pocet_pytliakov);
        printf("Pocet ochranarov:   %4.0lf\n\n",cm.pocet_ochranarov);
        //Adaptacia mapy v dalsej generacii
        cm = aktualizacia_fcm(cm);
    }
    //Informacny vypis
    printf("%d. generacia:\n",i);
    printf("Uroven zrazok:      %4.0lf\n",cm.uroven_zrazok);
    printf("Uroven zatravnenia: %4.0lf\n",cm.uroven_zatravnenia);
    printf("Pocet nosorozcov:   %4.0lf\n",cm.pocet_nosorozcov);
    printf("Pocet pytliakov:    %4.0lf\n",cm.pocet_pytliakov);
    printf("Pocet ochranarov:   %4.0lf\n",cm.pocet_ochranarov);
    printf("\nHotovo.");
    
    getchar();
}
