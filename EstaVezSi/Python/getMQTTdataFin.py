# -- Programa de python que actúa como intermediario entre la RaspberryPi y Firebase.

import paho.mqtt.client as mqtt
import pyrebase

MQTT_ADRESS = '192.168.66.86'
MQTT_USER = 'smartlibrary'
MQTT_PASSWORD = 'smartlibrary'
TOPIC_LUZ = 'smarlib/+/+'
TOPIC = 'smarlib/+/+'

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
        client.subscribe(TOPIC_LUZ)
        client.subscribe(TOPIC)

def stream_handler(message, client):
        luz = message["data"]
        print(luz)
        client.publish('smarlib/silla1/led', payload=luz, qos=0, retain = False)
        print('sent ' + str(luz)  + ' to smarlib/silla1/led')

def stream_handler2(message, client):
        luz = message["data"]
        print(luz)
        client.publish('smarlib/silla2/led', payload=luz, qos=0, retain = False)
        print('sent ' + str(luz)  + ' to smarlib/silla2/led')

def stream_handler3(message, client):
        luz = message["data"]
        print(luz)
        client.publish('smarlib/silla3/led', payload=luz, qos=0, retain = False)
        print('sent ' + str(luz)  + ' to smarlib/silla3/led')

def stream_handler4(message, client):
        luz = message["data"]
        print(luz)
        client.publish('smarlib/silla4/led', payload=luz, qos=0, retain = False)
        print('sent ' + str(luz)  + ' to smarlib/silla4/led')

def stream_handler5(message, client):
        luz = message["data"]
        print(luz)
        client.publish('smarlib/cabina/led', payload=luz, qos=0, retain = False)
        print('sent ' + str(luz) + ' to smarlib/cabina/led')


def on_message(client, userdata, msg):
        print(msg.topic +' '+ msg.payload.decode())
        if msg.topic == "smarlib/biblioteca/temperatura":
                data = {
                        "Temperatura" : msg.payload.decode()
                }
                database.update(data)
                print("Datos de la temperatura en Biblio enviados")

        elif msg.topic == "smarlib/silla1/ultrasonidos":
                data = {
                        "Ultrasonidos" : msg.payload.decode()
                        }
                database.child('Sillas').child('Silla1').update(data)
                print('Datos Ultrasonidos enviados')

        elif msg.topic == "smarlib/silla2/ultrasonidos":
                data = {
                        "Ultrasonidos" : msg.payload.decode()
                        }
                database.child('Sillas').child('Silla2').update(data)
                print('Datos Ultrasonidos2 enviados')

        elif msg.topic == "smarlib/silla3/ultrasonidos":
                data = {
                        "Ultrasonidos" : msg.payload.decode()
                        }
                database.child('Sillas').child('Silla3').update(data)
                print('Datos Ultrasonidos3 enviados')

        elif msg.topic == "smarlib/silla4/ultrasonidos":
                data = {
                        "Ultrasonidos" : msg.payload.decode()
                        }
                database.child('Sillas').child('Silla4').update(data)
                print('Datos Ultrasonidos4 enviados')

        elif msg.topic == 'smarlib/silla1/espalda':
                data = {
                        "Espalda" : msg.payload.decode()
                }
                database.child('Sillas').child('Silla1').update(data)
                print('Datos espalda enviados')

def main():
        client = mqtt.Client()
        client.username_pw_set('smartlibrary','smartlibrary')
        client.on_connect=on_connect
        client.on_message=on_message

        # Pass the `client` object as an argument to the stream handler functio>
        my_stream = database.child("Sillas").child("Silla1").child("led").stream(lambda message: stream_handler(message, client))
        my_stream2 = database.child("Sillas").child("Silla2").child("led").stream(lambda message: stream_handler2(message, client))
        my_stream3 = database.child("Sillas").child("Silla3").child("led").stream(lambda message: stream_handler3(message, client))
        my_stream4 = database.child("Sillas").child("Silla4").child("led").stream(lambda message: stream_handler4(message, client))
        my_stream5 = database.child("Cabinas").child("Cabina1").child("led").stream(lambda message: stream_handler4(message, client))
        

        client.connect(MQTT_ADRESS, 1883)
        client.loop_forever()

if __name__=='__main__':
        print('MQTT to INfluxDB bridge')
        main()