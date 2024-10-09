import paho.mqtt.client as mqtt
import pyodbc
import os

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("temperatura")
 
def on_message(client, userdata, msg):
    if msg.payload:
        print("INSERT INTO mqtt (payload) VALUES ('" + str(msg.payload).replace("'", "''") + "')")
        cnn = pyodbc.connect(f'DRIVER={{SQL Server}};SERVER={server};DATABASE={database};UID={username};PWD={password};')
    print("Connection successful!")
    cur = cnn.cursor()
    cur.execute("INSERT INTO mqtt (payload,capturedate) VALUES ('" + str(msg.payload).replace("b'", "").replace("'", "") + "',GETDATE())")
    cur.commit()


# Get database credentials from environment variables
server = os.getenv("DB_SERVER", "85.214.127.3")
database = os.getenv("DB_NAME", "IOT")
username = os.getenv("DB_USER", "dev_iot")
password = os.getenv("DB_PASS", "*****")

client =mqtt.Client()
client.username_pw_set("admin", "*****")
client.connect("85.214.127.3",1883,60)
client.on_connect = on_connect
client.on_message = on_message
client.loop_forever()

