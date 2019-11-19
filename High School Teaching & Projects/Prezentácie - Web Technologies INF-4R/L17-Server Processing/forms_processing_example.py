# use flask to create webserver and request to handle inputs
from flask import Flask, request
# setup server app
app = Flask(__name__)
# index webpage template as multiline string
input_webpage='''

<!DOCTYPE html>
<html>
<head>
	<title>HTML Forms Processing Example</title>
</head>
<body>
    <form action="/server_processing" method="get">
        <fieldset>
        <legend>Personal information:</legend>
        First name:<br>
        <input type="text" name="firstname" value="Mickey"><br>
        Last name:<br>
        <input type="text" name="lastname" value="Mouse"><br><br>
        <input type="submit" value="Submit via GET HTTP method">
        </fieldset>
    </form>
    <form action="/server_processing" method="post">
        <fieldset>
        <legend>Personal information:</legend>
        First name:<br>
        <input type="text" name="firstname" value="Mickey"><br>
        Last name:<br>
        <input type="text" name="lastname" value="Mouse"><br><br>
        <input type="submit" value="Submit via POST HTTP method">
        </fieldset>
    </form>
</body>
</html>

'''

# serve index with an HTML form at http://localhost:80/
@app.route('/')
def index():
    return input_webpage

# process the form with user inputs using either GET or POST HTTP method
@app.route('/server_processing', methods=['GET', 'POST'])
def server_processing():

    if request.method == 'GET':
        first_name = request.args['firstname']
        last_name = request.args['lastname']
        return "GET: Hello "+first_name+" "+last_name+"!"
        
    elif request.method == 'POST':
        first_name = request.form['firstname']
        last_name = request.form['lastname']
        return "POST: Hello "+first_name+" "+last_name+"!"
        
    else:
        return 'Error - Unsupported HTTP method!'

# start the server app
if __name__ == "__main__":
    app.run(host='0.0.0.0',port=80,debug=True)
