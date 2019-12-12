#include<stdio.h>
#include<stdlib.h>

#define HLBKA 6

typedef struct stav{
    char usporiadanie[9]; /*Usporiadanie v danom uzle stavoveho priestoru*/
    struct stav *dieta[4]; /*Stavy vychadzajuce z uzla*/
    int vytvorene[4]; /*Potvrdenie, ze dane stavy boli objavene*/
    int cesta; /*Cesta k cielu, v pripade ze uz bol objaveny*/
    int duplicita; /*Potvrdenie ze dany uzol uz bol objaveny najmenej dvakrat*/
} STAV;

int hladaj(STAV *zaciatok,STAV *uzol,int hlbka,char *ciel);
int pohyb(STAV *uzol,int smer);
int duplicitny_uzol(STAV *aktualny_uzol,STAV *uzol);
void vypis_cestu(STAV *uzol,char *ciel);

main(){
    int i;
    char ciel[9];
    STAV *uzol;
    int hlbka=HLBKA;
    int najdene=0;
    
    /* Alokacia pamate pre korenovy uzol*/
    if ((uzol=(STAV*)malloc(sizeof(STAV)))==NULL){
        printf ("\nChyba pri alokacii pamate!");
        getchar();
        exit(-1);
    }
    /* Nastavime duplicitu korenoveho uzla na 0*/
    uzol->duplicita=0;
        
    printf("Zadaj vychodzi stav v tvare:\n0 1 2\n3 4 5\n6 7 8\n");
    for(i=0;i<9;i++){
        printf("Pozicia %d: ",i);
        scanf("%c",&(uzol->usporiadanie[i]));
        getchar();
    }
    
    printf("Zadaj cielovy stav v tvare:\n0 1 2\n3 4 5\n6 7 8\n");
    for(i=0;i<9;i++){
        printf("Pozicia %d: ",i);
        scanf("%c",&(ciel[i]));
        getchar();
    }
    
    printf("\nVychodzi a cielovy stav:\n");
    for (i=0;i<3;i++)
        printf("%c ",uzol->usporiadanie[i]);
    printf("  ");
    for (i=0;i<3;i++)
        printf("%c ",ciel[i]);
    printf("\n");
    for (i=3;i<6;i++)
        printf("%c ",uzol->usporiadanie[i]);
    printf("  ");
    for (i=3;i<6;i++)
        printf("%c ",ciel[i]);
    printf("\n");
    for (i=6;i<9;i++)
        printf("%c ",uzol->usporiadanie[i]);
    printf("  ");
    for (i=6;i<9;i++)
        printf("%c ",ciel[i]);
    printf("\n\n");
    
    printf("Hladam... Hlbka hladania je %d",HLBKA);
    while (najdene==0){ /*Kym sme nenasli hladame*/
        if ((najdene=hladaj(uzol,uzol,hlbka,ciel))==0){
            hlbka+=HLBKA;
            printf("\nMenim hlbku hladania na %d...",hlbka);
        }
    }
    if (najdene==1){
        printf("\nCesta k cielu bola najdena!");
        getchar();
        vypis_cestu(uzol,ciel);
        printf("\nKoniec.");
    }else
        printf("\nCesta k cielu NEBOLA najdena!!!");
    getchar();
}
    
int hladaj(STAV *zaciatok,STAV *uzol,int hlbka,char *ciel){
    int i,j;
    int smevcieli=1;
    int vysledok;
    int poc_slepych=0;
    for (i=0;i<9;i++)
        if (uzol->usporiadanie[i]!=ciel[i])
            smevcieli=0;
    if (smevcieli==1) /*Ak sme nasli ciel*/
        return 1; /*Ohlasime najdenie ciela*/
    if (hlbka==0){ /*Ak sme natrafili na maximalnu hlbku cyklu prehladavania*/
        return 0; /*Backtracking*/
    }
    /*Ak je dany uzol potvrdeny ako duplicitny*/
    if (uzol->duplicita==1)
        return 2; /*Backtracking s oznamenim slepej cesty*/
    /*Ak novy uzol este nema deti a nie je overene, ci je duplicitny, overime*/
    if ((uzol->duplicita!=2) && (duplicitny_uzol(zaciatok,uzol)==1))
        return 2; /*Backtracking s oznamenim slepej cesty*/
    /*Uzol nie je duplicitny a bude mat deti, preto pri dalsom hladani */
    /*duplicitu neoverujeme                                            */
    uzol->duplicita=2;
    for (i=0;i<4;i++){ /*Pre vsetky deti/listy*/        
        if (pohyb(uzol,i)==0){
            poc_slepych++;
            continue;
        }
        /*Prehladavame dalsi list*/
        vysledok=hladaj(zaciatok,uzol->dieta[i],hlbka-1,ciel);
        if (vysledok==1){ /*Ak sme nasli ciel */
            uzol->cesta=i; /*Nastavime cestu k cielu v tomto uzle*/
            return 1; /*Ohlasime najdenie ciela*/
        }
        if (vysledok==2){ /*Ak sme narazili na slepu cestu*/
            poc_slepych++;
        }
    } /*Neuspesne sme prehladali vsetky listy v ramci aktualnej hlbky vnorenia*/
    if (poc_slepych==4)
        return 2; /*Backtracking s oznamenim slepej cesty*/
    return 0;/*Backtracking*/
}

int duplicitny_uzol(STAV *aktualny_uzol,STAV *uzol){
    int i,vysledok;
    if (uzol==aktualny_uzol) /*Ak je aktualne porovnavany uzol rovnaky ako...*/
        return 0;           /*...testovany uzol, nie je to duplicita        */
    vysledok=1;
    for(i=1;i<9;i++)
        if (uzol->usporiadanie[i]!=aktualny_uzol->usporiadanie[i]){
            vysledok=0;
            break;
        }
    if (vysledok==1){
        uzol->duplicita=1;
        return 1;
    }
    for (i=0;i<4;i++){
        if (aktualny_uzol->vytvorene[i]==2)
            if (duplicitny_uzol(aktualny_uzol->dieta[i],uzol)==1)
                return 1;
    }
    return 0;
}

void vypis_cestu(STAV *uzol,char *ciel){
    int i;
    int smevcieli=1;
    for (i=0;i<9;i++)
        if (uzol->usporiadanie[i]!=ciel[i])
            smevcieli=0;
    for (i=0;i<9;i++){
        printf("%c ",uzol->usporiadanie[i]);
        if (i==2 || i==5 || i==8)
            printf("\n");
    }
    if (smevcieli==0){
        getchar();
        vypis_cestu(uzol->dieta[uzol->cesta],ciel);
    }
}

int pohyb(STAV *uzol,int smer){
    int i=0;
    char vymen;
    if (uzol->dieta[smer]==uzol) /*Ak uz bolo overene, ze smer je blokovany*/
        return 0; /*Presun nie je mozny*/
    if (uzol->vytvorene[smer]==2) /*Ak uz bolo vytvorene dieta pre dany smer*/
        return 1; /*Presun je mozny*/
    /*Inak vytvorime dieta*/
    /* Alokacia pamate pre dieta */
    if ((uzol->dieta[smer]=(STAV*)malloc(sizeof(STAV)))==NULL){
        printf ("\nChyba pri alokacii pamate!");
        getchar();
        exit(-1);
    }
    /* Zapamatame si ze sme uz pamat alokovali (pre pripad dalsieho hladania) */
    uzol->vytvorene[smer]=1;
    /* Nastavime duplicitu dietata na 0, kedze zatial nevieme ci je duplicitne*/
    uzol->dieta[smer]->duplicita=0;
    /* Skopirujeme predchadzajuci stav */
    for (i=0;i<9;i++)
        uzol->dieta[smer]->usporiadanie[i]=uzol->usporiadanie[i];
    /* Najdeme znak ('0'), ktorym pohybujeme */
    i=0;
    while(uzol->usporiadanie[i]!='0')
        i++;
    
    /* Pohyb medzi stavmi*/
    
    /* Matica       */
    /*  0   1   2   */
    /*  3   4   5   */
    /*  6   7   8   */
    
    /* Premenna smer urcuje nasledovne pohyby*/
    /* smer==0 - dolava  */
    /* smer==1 - hore    */
    /* smer==2 - doprava */
    /* smer==3 - dole    */
    
    switch(smer){
        
        /*Posun dolava*/
        case 0:
            switch (i){
                case 0: /*Neda sa ist dolava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 1: /*Vymena medzi 0 a 1*/
                    vymen=uzol->dieta[smer]->usporiadanie[0];
                    uzol->dieta[smer]->usporiadanie[0]=
                        uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 2: /*Vymena medzi 1 a 2*/
                    vymen=uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=
                        uzol->dieta[smer]->usporiadanie[2];
                    uzol->dieta[smer]->usporiadanie[2]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 3: /*Neda sa ist dolava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 4: /*Vymena medzi 3 a 4*/
                    vymen=uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=
                        uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 5: /*Vymena medzi 4 a 5*/
                    vymen=uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=
                        uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 6: /*Neda sa ist dolava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 7: /*Vymena medzi 6 a 7*/
                    vymen=uzol->dieta[smer]->usporiadanie[6];
                    uzol->dieta[smer]->usporiadanie[6]=
                        uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 8: /*Vymena medzi 7 a 8*/
                    vymen=uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=
                        uzol->dieta[smer]->usporiadanie[8];
                    uzol->dieta[smer]->usporiadanie[8]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
            }
            break;
        
        /*Posun hore*/
        case 1:
            switch (i){
                case 0: /*Neda sa ist hore*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 1: /*Neda sa ist hore*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 2: /*Neda sa ist hore*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 3: /*Vymena medzi 0 a 3*/
                    vymen=uzol->dieta[smer]->usporiadanie[0];
                    uzol->dieta[smer]->usporiadanie[0]=
                        uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 4: /*Vymena medzi 1 a 4*/
                    vymen=uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=
                        uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 5: /*Vymena medzi 2 a 5*/
                    vymen=uzol->dieta[smer]->usporiadanie[2];
                    uzol->dieta[smer]->usporiadanie[2]=
                        uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 6: /*Vymena medzi 3 a 6*/
                    vymen=uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=
                        uzol->dieta[smer]->usporiadanie[6];
                    uzol->dieta[smer]->usporiadanie[6]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 7: /*Vymena medzi 4 a 7*/
                    vymen=uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=
                        uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 8: /*Vymena medzi 5 a 8*/
                    vymen=uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=
                        uzol->dieta[smer]->usporiadanie[8];
                    uzol->dieta[smer]->usporiadanie[8]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
            }
            break;
        
        /*Posun doprava*/
        case 2:
            switch (i){
                case 0: /*Vymena medzi 0 a 1*/
                    vymen=uzol->dieta[smer]->usporiadanie[0];
                    uzol->dieta[smer]->usporiadanie[0]=
                        uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 1: /*Vymena medzi 1 a 2*/
                    vymen=uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=
                        uzol->dieta[smer]->usporiadanie[2];
                    uzol->dieta[smer]->usporiadanie[2]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 2: /*Neda sa ist doprava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 3: /*Vymena medzi 3 a 4*/
                    vymen=uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=
                        uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 4: /*Vymena medzi 4 a 5*/
                    vymen=uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=
                        uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 5: /*Neda sa ist doprava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 6: /*Vymena medzi 6 a 7*/
                    vymen=uzol->dieta[smer]->usporiadanie[6];
                    uzol->dieta[smer]->usporiadanie[6]=
                        uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 7: /*Vymena medzi 7 a 8*/
                    vymen=uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=
                        uzol->dieta[smer]->usporiadanie[8];
                    uzol->dieta[smer]->usporiadanie[8]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 8: /*Neda sa ist doprava*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
            }
            break;
        
        /*Posun dole*/
        case 3:
            switch (i){
                case 0: /*Vymena medzi 0 a 3*/
                    vymen=uzol->dieta[smer]->usporiadanie[0];
                    uzol->dieta[smer]->usporiadanie[0]=
                        uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 1: /*Vymena medzi 1 a 4*/
                    vymen=uzol->dieta[smer]->usporiadanie[1];
                    uzol->dieta[smer]->usporiadanie[1]=
                        uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 2: /*Vymena medzi 2 a 5*/
                    vymen=uzol->dieta[smer]->usporiadanie[2];
                    uzol->dieta[smer]->usporiadanie[2]=
                        uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 3: /*Vymena medzi 3 a 6*/
                    vymen=uzol->dieta[smer]->usporiadanie[3];
                    uzol->dieta[smer]->usporiadanie[3]=
                        uzol->dieta[smer]->usporiadanie[6];
                    uzol->dieta[smer]->usporiadanie[6]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 4: /*Vymena medzi 4 a 7*/
                    vymen=uzol->dieta[smer]->usporiadanie[4];
                    uzol->dieta[smer]->usporiadanie[4]=
                        uzol->dieta[smer]->usporiadanie[7];
                    uzol->dieta[smer]->usporiadanie[7]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 5: /*Vymena medzi 5 a 8*/
                    vymen=uzol->dieta[smer]->usporiadanie[5];
                    uzol->dieta[smer]->usporiadanie[5]=
                        uzol->dieta[smer]->usporiadanie[8];
                    uzol->dieta[smer]->usporiadanie[8]=vymen;
                    uzol->vytvorene[smer]=2;
                    return 1;
                case 6: /*Neda sa ist dole*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 7: /*Neda sa ist dole*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
                case 8: /*Neda sa ist dole*/
                    free((void*)uzol->dieta[smer]);
                    uzol->dieta[smer]=uzol;
                    return 0;
            }
            break;
    }
}
