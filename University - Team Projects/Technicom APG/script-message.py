from modules import utils, fileio, messaging
from modules.database import Database
from modules.generator import Generator
from modules.delimiter import Delimiter
import json

#load message test file
with open('data/message.json', 'rb') as jsonfile:
    #open message as dictionary
    msg = json.loads(jsonfile.read())
#load customer table
customers = messaging.unmarshall(msg['customers'])
#load all event tables
msg_events = msg['events']
events = list()
for key in msg_events:
    events.append(messaging.unmarshall(msg_events[key]))
# create database
tables = [customers] + events
database = Database(tables,msg['database_key'])
# generate new attributes
generator = Generator(database)
generator.generate()
# eliminate attributes with low correlation with the target attribute
delimiter = Delimiter(database)
delimiter.eliminate(msg['target_attribute'],msg['safe_list'])
# save updated customer table
fileio.saveTable(database.customers,'customers_new.csv')