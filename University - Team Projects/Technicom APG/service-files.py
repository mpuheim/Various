from modules import utils, fileio, messaging
from modules.database import Database
from modules.generator import Generator
from modules.delimiter import Delimiter
from flask import Flask, request, Response, json
app = Flask(__name__)

@app.route("/")
def indexPage():
    return "Service is deployed at /service"

@app.route("/service", methods=['GET', 'POST'])
def servicePage():
    if request.method == 'POST':
        #get files from POST
        file1 = request.files['customers']
        file2 = request.files['purchases']
        #load tables
        customers = fileio.loadTable(file1)
        purchases = fileio.loadTable(file2)
        #generate attributes
        database = Database([customers,purchases],'customer_id')
        generator = Generator(database)
        generator.generate()
        target_attribute = 'properties.birth_year'
        safe_list = ['properties.first_name','properties.last_name','properties.gender']
        delimiter = Delimiter(database)
        delimiter.eliminate(target_attribute,safe_list)
        #generate message
        otype = 'CSV' if '.csv' in file1.filename else 'JSON'
        mtype = 'text/csv'  if '.csv' in file1.filename else 'text/json'
        output = messaging.marshall(database.customers,otype)
        resp = Response(output, status=200, mimetype=mtype)
        return resp
    else:
        return "Please use POST HTTP method."

if __name__ == "__main__":
    app.run(host='0.0.0.0')
