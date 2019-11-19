from table import Table

class Database:
    # constructor
    def __init__(self,tables,key):
        self.tables = tables
        self.key = key
        self.customers = tables[0]
        if len(tables)>1:
            self.events = tables[1:]
        else:
            self.events = None