#!/usr/bin/env python
# -*- coding: utf-8 -*-
from flask import Flask
from pagelib import get_page
app = Flask(__name__)

@app.route('/')
def main():
    return get_page()
    
if __name__ == "__main__":
    app.run(host='127.0.0.1',port=80,debug=True)
