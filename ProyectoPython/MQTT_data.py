#get_MQTT_data.py

import paho.mqtt.client as mqtt
import pyrebase

MQTT_ADRESS = '192.168.40.86'
MQTT_USER = 'smartlibrary'
MQTT_PASSWORD = 'smartlibrary'
MQTT_TOPIC = 'smarlib/+/+'

config = {
        "apiKey": "AIzaSyCMo8-eWUCmHHRv2ej8QeKTh4Ez3Bm77CE",
        "authDomain": "smart-library-69561.firebaseapp.com",
        "databaseURL": "https://smart-library-69561-default-rtdb.firebaseio.com/",
        "storageBucket": "smart-library-69561.appshot.com"
}

firebase = pyrebase.initialize_app(config)

database = firebase.database()

def on_connect(client, userdata, flags, rc):
        print('Connected with result code ' + str(rc))
        client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
        print(msg.topic +' '+ msg.payload.decode())
        if msg.topic == 'smarlib/biblioteca/temperatura':
                data = {
                        'Temperatura': msg.payload.decode()
                }
                database.update(data)
                print('Temperatura enviada a la BD')

        elif msg.topic == 'smarlib/silla1/ultrasonidos':
                data = {
                        "Ultrasonidos" : msg.payload.decode()
                        }
                database.child('Sillas').child('Silla1').update(data)
                print('Datos Ultrasonidos enviados')

        elif msg.topic == 'smarlib/silla1/espalda':
                data = {
                        "Espalda" : msg.payload.decode()
                }
                database.child('Sillas').child('Silla1').update(data)
                print('Datos espalda enviados')

def stream_handler(message):
        luz = message["data"]
        print(luz)
        mqtt_client.publish('smarlib/silla1/led',payload = luz, qos = 0, retain = False)
        print('sent' + luz + 'to smarlib/silla1/led')

def main():
        mqtt_client = mqtt.Client()
        mqtt_client.username_pw_set('smartlibrary','smartlibrary')
        mqtt_client.on_connect=on_connect
        mqtt_client.on_message=on_message

        mqtt_client.connect(MQTT_ADRESS, 1883)
        mqtt_client.loop_forever()

        my_stream = database.child("Sillas").child("Silla1").child("led").stream(lambda message: stream_handler(message, client))

if __name__=='__main__':
        print('MQTT to INfluxDB bridge')
        main()