# ReadLuz.py

import paho.mqtt.client as mqtt
import pyrebase

MQTT_ADRESS = '192.168.177.139'
MQTT_USER = 'carlos'
MQTT_PASSWORD = 'carlos'

config = {
        "apiKey": "AIzaSyBglTt6aNPMnp_9hoS3Y4h0DyOFlUtFCzc",
        "authDomain": "bolando-e081f.firebaseapp.com",
        "databaseURL": "https://bolando-e081f-default-rtdb.europe-west1.firebasedatabase.app/",
        "storageBucket": "bolando-e081f.appshot.com"
}

firebase = pyrebase.initialize_app(config)

database = firebase.database()

def on_connect(client, userdata, flags, rc):
    print('Connected with result code ' + str(rc))

client = mqtt.Client()
client.username_pw_set(MQTT_USER,MQTT_PASSWORD)
client.on_connect = on_connect
client.connect(MQTT_ADRESS, 1883)

def stream_handler(message):
    luz = message["data"]
    print(luz)
    client.publish('bolando/Cullera/Iluminacion', payload=luz, qos=0, retain = False)
    print('sent ' + luz  + ' to bolando/Cullera/Iluminacion')

my_stream = database.child("Cullera").child("Iluminacion").stream(stream_handler)