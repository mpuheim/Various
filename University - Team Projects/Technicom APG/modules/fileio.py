import csv
import json
from table import Table

def asTable(dct):
    table_name = dct['name']
    table_head = list(dct['head'])
    table_data = list(dct['body'])
    return Table(table_name,table_head,table_data)

def loadTable(file,separator=','):
    #open file if name is provided
    if type(file) is str:
        if '.csv' in file:
            #load CSV file
            with open(file, 'rb') as csvfile:
                #get the line reader ready
                rowreader = csv.reader(csvfile, delimiter=separator)
                #set name of the table
                table_name = file.replace('.csv','')
                #load the head of the table
                table_head = list(rowreader.next())
                #load the data
                table_data = list();
                for row in rowreader:
                    table_data.append(list(row))
                #create the table
                table_object = Table(table_name,table_head,table_data)
                #return the loaded table
                return table_object
        elif '.json' in file:
            #load JSON FILE
            with open(file, 'rb') as jsonfile:
                return json.loads(jsonfile.read(), object_hook=asTable)
        else:
            print 'Error while loading file "' + file + '".'
            return None;
    #read from file object (Flask FileStorage)
    else:
        if '.csv' in file.filename:
            #load CSV file
            rowreader = csv.reader(file, delimiter=separator)
            #set name of the table
            table_name = file.filename.replace('.csv','')
            #load the head of the table
            table_head = list(rowreader.next())
            #load the data
            table_data = list();
            for row in rowreader:
                table_data.append(list(row))
            #create the table
            table_object = Table(table_name,table_head,table_data)
            #return the loaded table
            return table_object
        elif '.json' in file.filename:
            #load JSON FILE
            return json.loads(file.read(), object_hook=asTable)
        else:
            print 'Error while loading file "' + file.filename + '".'
            return None;

def saveTable(table_object,filename,separator=','):
    if '.json' in filename:
        #save as JSON
        with open(filename, 'wb') as jsonfile:
            jsonfile.write('{\n')
            jsonfile.write('  "name": "' + table_object.name + '",\n')
            jsonfile.write('  "head": ["' + '", "'.join(table_object.head) + '"],\n')
            jsonfile.write('  "body":\n  [\n')
            data = list();
            for row in table_object.body:
                atributes = [attr.replace('"','').replace(',','') for attr in row]
                data.append('    ["'.encode('utf-8') + ('", "'.encode('utf-8')).join(atributes) + '"]'.encode('utf-8'))
            jsonfile.write((',\n'.encode('utf-8')).join(data))
            jsonfile.write('\n  ]\n')
            jsonfile.write('}')
    else: 
        #save as CSV
        with open(filename, 'wb') as csvfile:
            #write the head of the table
            csvfile.write((separator.join(table_object.head) + '\n').encode('utf-8'))
            #write the data
            for row in table_object.body:
                atributes = [attr.replace(separator,'') for attr in row]
                csvfile.write((separator.join(row) + '\n').encode('utf-8'))
