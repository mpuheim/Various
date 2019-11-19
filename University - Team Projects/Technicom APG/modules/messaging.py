from fileio import asTable
from table import Table
import csv
import json
import StringIO

# JSON to table
def unmarshall(msg):
    # message is string -> deserialize using json and fileio helper
    if type(msg) is str:
        return json.loads(msg, object_hook=asTable)
    # message is dictionary -> convert to table directly
    elif type(msg) is dict:
        #check for correct json object type
        if msg['type'] != u'table':
            print 'Error - cannot unmarshall table. Message JSON has incorrect type.'
            return None
        #set name of the table
        table_name = msg['name']
        #load the head of the table
        table_head = list(msg['head'])
        #load the data
        table_data = list(msg['body'])
        #create the table
        table_object = Table(table_name,table_head,table_data)
        #return the loaded table
        return table_object
    # message is of unknown type -> no conversion possible
    else:
        print 'Error - cannot unmarshall table. Message object is invalid'
        return None

# table to JSON/CSV
def marshall(table_object,type='JSON',separator=','):
    # 'dummy' file
    outfile = StringIO.StringIO()
    # export as CSV
    if type == 'CSV':
        #write the head of the table
        outfile.write((separator.join(table_object.head) + '\n').encode('utf-8'))
        #write the data
        for row in table_object.body:
            atributes = [attr.replace(separator,'') for attr in row]
            outfile.write(separator.join(row) + '\n'.encode('utf-8'))
    # export as JSON
    else:
        outfile.write('{\n')
        outfile.write('  "name": "' + table_object.name + '",\n')
        outfile.write('  "head": ["' + '", "'.join(table_object.head) + '"],\n')
        outfile.write('  "body":\n  [\n')
        data = list();
        for row in table_object.body:
            atributes = [attr.replace('"','').replace(',','') for attr in row]
            data.append('    ["'.encode('utf-8') + ('", "'.encode('utf-8')).join(atributes) + '"]'.encode('utf-8'))
        outfile.write((',\n'.encode('utf-8')).join(data))
        outfile.write('\n  ]\n')
        outfile.write('}')
    output = outfile.getvalue()
    outfile.close()
    # return string
    return output