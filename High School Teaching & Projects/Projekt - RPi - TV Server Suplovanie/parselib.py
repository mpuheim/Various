#!/usr/bin/env python
# -*- coding: utf-8 -*-

import requests

def get_substitution_data():

    """Funkcia vracia precistene data zo stranky https://gymes.edupage.org/substitution/"""

    # vytvorime HTTP GET dopyt na stranku suplovania
    try:
        response = requests.get("https://gymes.edupage.org/substitution/").text

    # pri chybe pripojenia vratime chybovu spravu
    except requests.exceptions.RequestException as e:
        return u"Problém so sieťovým pripojením\n"
    
    #odstranime vsetky html znacky, nechame len cisty text
    cisty = ""
    i=response.find('Suplovanie - ')    #zistime, kde na stranke zacina suplovanie
    end=response.find('<script>')       #zistime, kde na stranke konci suplovanie
    while i < end:                      #prejdeme cez vsetky znaky html suboru
        if response[i:i+6] == "<style":
            #print("style: " + response[i:i+50])
            while response[i:i+8] != "</style>":    #preskakujeme obsah medzi <style...</style>
                i = i+1
        if response[i] == "<":
            while response[i] != ">":               #vynechavame vnutro vsetkych znaciek <...>
                i = i+1
            i = i+1
            if len(cisty) > 0 and cisty[-1] != "\n":
                cisty = cisty + "\n"
        else:                                       #vsetok obsah ostatnych znaciek ukladame
            cisty = cisty + response[i]
            i = i+1
    """
    #ulozime si vycisteny dokument ako textovy subor
    with open("out.txt","w",encoding="UTF-8") as f:
        f.write(cisty)
        print("OK")
    """
    
    return cisty
    
    
