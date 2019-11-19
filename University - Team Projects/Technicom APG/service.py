from modules import worker
from flask import Flask, request, Response, json, url_for
import threading
import uuid

app = Flask(__name__)

# dictionary which stores currently processed requests
background_tasks = {}

# default index page
@app.route("/")
def indexPage():
    return "Service is deployed at /service"

# service page
@app.route("/service", methods=['GET', 'POST'])
def servicePage():
    if request.method == 'POST':
        # get message data
        data = str(request.data)
        # set task id
        id = str(uuid.uuid4())
        # prepare new task
        background_tasks[id] = None
        task = threading.Thread(target=worker.processRequest, args=(data,background_tasks,id))
        # run task in separate thread
        task.start()
        # return retrieval information
        return 'Retrieve your data from <a href="http://localhost:5000/retrieve?id=' + str(id) + '" target="_blank">here</a>.'
    else:
        return "Please use POST HTTP method."

# response page
@app.route("/retrieve", methods=['GET', 'POST'])
def retrievePage():
    if request.method == 'GET':
        # get task id
        id = request.args.get('id')
        # check if task is being processed
        if id in background_tasks:
            # present data if task is finished
            if background_tasks[id] != None:
                resp = background_tasks[id]
                del background_tasks[id]
                return resp
            # prompt to wait if task is not finished yet
            else:
                return "Task (ID=" + id + ") is being processed. Try again later."
        # inform if the task does not exist
        else:
            return "Sorry, there is no task with ID=" + id
    else:
        return "Please use GET HTTP method."

# entry point for the application
if __name__ == "__main__":
    app.run(host='0.0.0.0',debug=True)
