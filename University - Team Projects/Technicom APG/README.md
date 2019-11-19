# Automatic Predictor Generator

Automatický generátor prediktorov (APG) je program,
ktorý umožňuje automaticky generovať agregované atribúty
z údajov o správaní zákazníkov.

Program pracuje s dvoma typmi tabuliek. Prvou je tabuľka
definujúca atribúty zákazníkov, ako je napr. ich meno,
priezvisko a pod. Táto tabuľka je len jedna a unikátna
a každému zákazníkovi v nej prislúcha jeden riadok.
Ďalším typom tabuliek sú tabuľky udalostí. Týchto tabuliek
môže byť aj viacero (napr. nákupy zákazníkov, ich účasť
na reklamných akciách a pod.). Jednému zákazníkovi môže
v tomto prípade prislúchať aj viacero riadkov (záznamov)
v tabuľke, pričom konkrétny zákazník je identifikovaný
pomocou jeho kľúča (ID) z tabuľky zákazníkov.

Cieľom programu je vytvárať nové atribúty v tabuľke
zákazníkov prostredníctvom agregácie údajov z tabuliek
udalostí. Táto agregácia je vždy vykonaná vzhľadom k
nejakému už existujúcemu cieľovému atribútu v tabuľke
zákazníkov, teda cieľom je vytvárať atribúty, ktoré
blízko korelujú s nejakým už existujúcim atribútom.
Napríklad ak má nejaký zákazník atribút "verný zákazník"
s hodnotou "áno", program sa snaží vygenerovať atribút,
ktorý bude napríklad založený na priemernej hodnote jeho
nákupov v tabuľke s nákupmi. Cieľom je identifikácia
(resp. predikcia) atribútov vhodných k vytvoreniu
rozhodovacieho stromu, ktorý bude rozhodovať, či
nejaký zákazník má potenciál dosiahnuť určitú hodnotu
cieľového atribútu (napr. stať sa "verným zákazníkom").

Bližšie informácie o programe:

- [docs/puheim_2015_11_cinti.pdf](docs/puheim_2015_11_cinti.pdf)

- [docs/attachments/assignment.pdf](docs/attachments/assignment.pdf)

## Author

* **Michal Puheim** - [mpuheim](https://github.com/mpuheim)

## Acknowledgement

This program was created within the Project implementation: 
University Science Park TECHNICOM for Innovation
Applications Supported by Knowledge Technology, ITMS:
26220220182, supported by the Research & Development
Operational Programme funded by the ERDF. "We support
research activities in Slovakia/this project is being co-financed
by the European Union.

## Licence

These documents and software may be read and copied but NOT modified under the terms of the [LICENSE.md](LICENSE.md)
