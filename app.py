#!/usr/bin/python3
from flask import (Flask, Markup, Response, config, flash, redirect,
                   render_template, request, session, url_for)

# setup the app
app = Flask(__name__)
app.config['DEBUG'] = True


####  setup routes  ####
import redis
redisDB0=redis.StrictRedis(db=0,decode_responses=True)
redisDB0.set("MPI",0)
#redisDB0.set_response_callback('HGET', float)


@app.route('/gethost')
def gethost():
    id=int(redisDB0.get("MPI"))
    if id%2==0:
        return "gpu-0-0.local"
    else:
        return "gpu-0-0cat.local"


@app.route('/status')
def status():
    id=int(redisDB0.get("MPI"))
    id=id+1
    redisDB0.set("MPI",id)
    
    if id%50==0:
        return "EXIT"
    if id%2==0:
        return "ADD"
    
    if id%3==0:
        return "REMOVE"
    
    
    return "OK"

if __name__ == "__main__":
    # change to app.run(host="0.0.0.0"), if you want other machines to be able to reach the webserver.
    app.run(host="0.0.0.0",port=5000)
