# 1. otvorte napr program Notepad
# 2. v správcovi úloh na záložke podrobnosti zistite
#    identifikátor procesu a prepíšte hodnotu nižšie

ID=4

import os
import signal
os.kill(ID, signal.SIGTERM)
