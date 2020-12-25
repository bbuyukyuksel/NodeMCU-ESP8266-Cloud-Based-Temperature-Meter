from flask import Flask, redirect, url_for, request
import json
app = Flask(__name__)

@app.route('/')
def home():
    return 'Welcome to home page'

@app.route('/success/<name>')
def success(name):
    return 'welcome %s' % name

@app.route('/login',methods = ['POST', 'GET'])
def login():
    if request.method == 'POST':
        print(request.form)
        api = request.form['api']
        if api == 'nodewithme001':
            device_name = request.form['device_name']
        return json.dumps({"device_name":device_name})
        
    elif request.method == 'GET':
        user = request.args.get('api')
        return redirect(url_for('success',name = user))


if __name__ == '__main__':
   app.run(debug = True, host='0.0.0.0')