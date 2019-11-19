#!/usr/bin/env python
# -*- coding: utf-8 -*-

from parselib import get_substitution_data

def get_page():
    #nacitame udaje zo stranky
    data = get_substitution_data()
    #rozdelime udaje na jednotlive riadky
    riadky = data.split("\n")

    #pripravime HTML subor
    page  = ""
    page += "<!DOCTYPE html>"                       +"\n"
    page += "<html>"                                +"\n"
    page += "<meta charset='UTF-8'>"                +"\n"

    #pridame prvy riadok (nadpis Suplovanie a Informacie pre studentov)
    page += "<h1>"+riadky[0]+" - "+riadky[1]+"</h1>"+"\n"

    #pridame info o chybajucich (triedach, uciteloch, ucebniach)
    i = 3 if len(riadky)>3 else 1
    while u"Chýbajú" in riadky[i]:
        page += "<p>"+riadky[i]+"</p>"      +"\n"
        i = i+1
        
    #zapiseme hlavicku tabulky
    if len(riadky[i])>0 and riadky[i] != u"Na tento deň nie sú zadané žiadne suplovania.":
        page += "<table>"                               +"\n"
        page += "  <tr>"                                +"\n"
        page += "    <th>TRIEDA</th>"                   +"\n"
        page += "    <th>HODINA</th>"                   +"\n"
        page += "    <th>ZMENA </th>"                   +"\n"
        page += "  </tr>"                               +"\n"

    #pomocny zoznam tried
    triedy=["I.A","II.A","III.A","IV.A","V.A",
            "I.B","II.B","III.B","IV.B","V.B",
            "I.C","II.C","III.C","IV.C","V.C",
            "I.D","II.D","III.D","IV.D","V.D",
            "I.E","II.E","III.E","IV.E","V.E",
            "I.O","II.O","III.O","IV.O"]
            
    #nacitame pokyny pre jednotlive triedy
    while len(riadky[i])>0:
        
        if riadky[i] == "www.asctimetables.com":    # ak sme presli cez vsetky data
            break                                   # ukoncime cyklus naplnania tabulky
            
        if riadky[i] == u"Na tento deň nie sú zadané žiadne suplovania.":
            page += "<p>"+riadky[i]+"</p>"          +"\n"
            i = i+1
            break
        
        # ak i-ty riadok zacina triedou:
        if riadky[i] in triedy:   
            # zapiseme ju do noveho riadku tabulky
            page += "  <tr>"                        +"\n"
            page += "    <td style='text-align: center;'>"
            page +=            riadky[i]+"</td>"    +"\n"
            # a zvysime pocitadlo riadkov
            i = i+1
        
        # ak nezacina triedou, vlozime prazdnu bunku
        else:
            page += "  <tr>"                        +"\n"
            page += "    <td> </td>"                +"\n"
            
        # zapiseme hodinu
        page += "    <td style='text-align: center;'>"
        page +=            riadky[i]+"</td>"        +"\n"
        i = i+1
        
        # zapiseme zmenu
        page += "    <td>"+riadky[i]+"</td>"        +"\n"
        i = i+1
        page += "  </tr>"                           +"\n"

    #ukoncime tabulku
    page += "</table>"                              +"\n"
    page += "<p>"+riadky[i]+"</p>"                  +"\n"

    #pridame CSS (centrovanie textu, ohranicenie tabulky)
    page += """
    <style>
        html {              
            zoom: 200%;
        } 
        table, th, td {
            border: 1px solid black;   
            border-collapse: collapse; 
        }
        h1, p {                     
            text-align: center;        
        }
        table {                      
            margin-left:auto;          
            margin-right:auto;         
        }                            
    </style>  
    """  

    #pridame JavaScript (automaticke skrolovanie & reload stranky)
    page += """
    <script>                              
        function scroll_down() {
            clearInterval(scroll);
            scroll = setInterval(function()   
                {window.scrollBy(0, 1);      
                console.log('scroll_down started');}, 10);
            setTimeout(function(){ window.scrollTo(0, 0);location.reload(); }, 40000);
        }
        window.onload = function() {          
            setTimeout(scroll_down,10000);
        }
    </script>  
    """                           

    #ukoncime HTML
    page += "</html>"

    """
    #zapiseme tabulku do HTML suboru v UTF-8 formate
    with open("page.html","w",encoding="UTF-8") as file:
        file.write(page)
    """
    return page
