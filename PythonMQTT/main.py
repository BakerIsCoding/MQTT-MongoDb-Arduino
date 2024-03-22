import paho.mqtt.client as mqtt
from datetime import datetime
from pymongo import *

# CONFIG
#La ip de tu pc
mqtt_server = "10.1.105.26"
#La ip de la base de datos mongodb
mongodb_ip = "127.0.0.1"

def on_connect(client, userdata, flags, rc):
    print(f"Conectado con código de resultado {rc}")
    # Me subsscribo a los temas
    client.subscribe("ff/temp")

def on_message(client, userdata, msg):
    now = datetime.now()
    current_time = now.strftime("%Y-%m-%d %H:%M:%S")


    if msg.topic == "ff/temp":
        try:
            splitMsg = msg.payload.decode()
            total = splitMsg.split("-")

            remitente = total[2]
            temperaturaN = total[0] + "°C"
            humedadN = total[1] + "%"

            # Imprimir los valores individuales
            print("temperatura:", temperaturaN)
            print("humedad:", humedadN)
            print("remitente:", remitente)

            sendToMongo(remitente, temperaturaN, humedadN)
        except Exception as e:
            print("Ha ocurrido un error: " + str(e))


    else:
        print("El topic es incorrecto, " + msg.topic)


def sendToMongo(remitente, temperatura, humedad):
    # Nombre de usuario y contraseña de mongo
    nombre_usuario = 'mongoadmin'
    contrasena = 'secret'

    #Me conecto
    client = MongoClient('mongodb://{}:{}@'+mongodb_ip+':27017/'.format(nombre_usuario, contrasena))
    db = client['mqtt']  #Nombre de la base de datos
    coleccion = db[f'registro_{remitente}']  #Nombre del registro

    now = datetime.now()
    current_time = now.strftime("%Y-%m-%d %H:%M:%S")

    documento = {
        'temperatura': temperatura,
        'humedad': humedad,
        'fecha': current_time
    }

    # Insertar el registro
    resultado = coleccion.insert_one(documento)
    print(f'Documento insertado con el ID: {resultado.inserted_id}')

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
client.on_connect = on_connect
client.on_message = on_message

client.connect(mqtt_server, 1883, 60)
client.loop_forever()


