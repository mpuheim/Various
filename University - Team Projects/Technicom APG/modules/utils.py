from os import mkdir
from sys import exc_clear
            
# make directory
def makedir(name):
    try:
        mkdir(name) # try to make directory
    except OSError:
        exc_clear() # don't show error if directory exists
        # print "Cannot create directory '" + name + "'. Directory already exists."