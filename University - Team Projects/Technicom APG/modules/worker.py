from modules import utils, fileio, messaging
from modules.database import Database
from modules.generator import Generator
from modules.delimiter import Delimiter
import json

def processRequest(request_data,responses,id):
    # print task info
    print '- Starting task (ID=' + id + ')'
    # open message as dictionary
    msg = json.loads(request_data)
    # load customer table
    customers = messaging.unmarshall(msg['customers'])
    # load all event tables
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
    # return updated customer table
    responses[id] = messaging.marshall(database.customers)
    # print task info
    print '- Finished task (ID=' + id + ')'
    
def respondRequest(request_data):
    tasks = {}
    tasks['toResp'] = None
    processRequest(request_data,tasks,'toResp')
    return tasks['toResp']