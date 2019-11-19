# use flask to create webserver and request to handle inputs
from flask import Flask, request
# setup server app
app = Flask(__name__)
# simple calculator template
calculator_page='''

<form action="/compute" method="get">
    num1:<input type="text" name="num1" value=""><br>
    num2:<input type="text" name="num2" value=""><br>
    <input type="submit" value="+" name="sum">
    <input type="submit" value="*" name="product">
</form>

'''

# show a calculator at http://localhost:80/calculator
@app.route('/calculator', methods=['GET'])
def calculator():
    return calculator_page

# display results at http://localhost:80/compute
@app.route('/compute', methods=['GET'])
def compute():
    if "sum" in request.args:
        num1=int(request.args["num1"])
        num2=int(request.args["num2"])
        sum=num1+num2
        return str(num1)+"+"+str(num2)+"="+str(sum)
    elif "product" in request.args:
        num1=int(request.args["num1"])
        num2=int(request.args["num2"])
        product=num1*num2
        return str(num1)+"+"+str(num2)+"="+str(product)
    else:
        return "Error - Unsupported operation!"

# start the server app
if __name__ == "__main__":
    app.run(host='0.0.0.0',port=80,debug=True)
