from modules.worker import respondRequest
from flask import Flask, request, Response, json
app = Flask(__name__)

@app.route("/")
def indexPage():
    return "Service is deployed at /service"

@app.route("/service", methods=['GET', 'POST'])
def servicePage():
    if request.method == 'POST':
        resp = respondRequest(str(request.data))
        return resp
    else:
        return "Please use POST HTTP method."

if __name__ == "__main__":
    app.run(host='0.0.0.0',debug=True)
