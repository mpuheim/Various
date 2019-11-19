# Suplovanie na TV

Program pre zobrazenie suplovania podľa stránky Edupage na školskej TV.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Pokyny pre automatické spustenie servra a zobrazenie suplovania po reštarte počítača R-Pi:

1. Otvorte terminál

2. Otvorte súbor crontab:

crontab -e

4. Na koniec súboru pridajte riadky:

@reboot sudo python "/home/pi/Documents/Suplovanie/server.py"

@reboot sleep 10 && export DISPLAY=:0.0 && chromium-browser localhost --start-fullscreen

5. Zavrite a uložte súbor (CTRL+X -> Y -> Enter)

## Author

* **Michal Puheim** - [mpuheim](https://github.com/mpuheim)

## Licence

These documents and software may be read and copied but NOT modified under the terms of the [LICENSE.md](LICENSE.md)
