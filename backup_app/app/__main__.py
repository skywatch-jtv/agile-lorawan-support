import paho.mqtt.client as mqtt
import psycopg2
import app.config as config
import json
import time
import datetime


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(config.mqtt_channel)


def on_disconnect():
    print("Disconnected")


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    json_str = msg.payload.decode("utf-8")
    payload = json.loads(json_str)
    deviceName = payload['deviceName']
    devEUI = payload['devEUI']
    rxInfo = payload['rxInfo'][0]
    obj = payload['object']

    time = rxInfo['time']
    rssi = rxInfo['rssi']
    loRaSNR = rxInfo['loRaSNR']

    temperatureSensor = obj['temperatureSensor']['2']
    humiditySensor = obj['humiditySensor']['3']

    if 'gpsLocation' in obj:
        latitude = obj['gpsLocation']['0']['latitude']
        longitude = obj['gpsLocation']['0']['longitude']
        altitude = obj['gpsLocation']['0']['altitude']
    else:
        latitude = float('NaN')
        longitude = float('NaN')
        altitude = float('NaN')

    ts = datetime.datetime.strptime(time[:-1], "%Y-%m-%dT%H:%M:%S.%f")
    print(ts)

    print(
        "deviceName: {deviceName}\n"
        "devEUI: {devEUI}\n"
        "time: {time}\n"
        "rssi: {rssi}\n"
        "loRaSNR: {loRaSNR}\n"
        "temperatureSensor: {temperatureSensor}\n"
        "humiditySensor: {humiditySensor}\n"
        "latitude: {latitude}\n"
        "longitude: {longitude}\n"
        "altitude: {altitude}\n"
        .format(
            deviceName=deviceName,
            devEUI=devEUI,
            time=ts,
            rssi=rssi,
            loRaSNR=loRaSNR,
            temperatureSensor=temperatureSensor,
            humiditySensor=humiditySensor,
            latitude=latitude,
            longitude=longitude,
            altitude=altitude,
        ))
    with psycopg2.connect(
        dbname='postgres',
        user='postgres',
        password='postgres',
        host='localhost') as conn:
        with conn.cursor() as cur:
            cur.execute('''INSERT INTO sensordata(
                deviceName,
                devEUI,
                time,
                rssi,
                loRaSNR,
                temperatureSensor,
                humiditySensor,
                latitude,
                longitude,
                altitude)
            VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)''', (
                deviceName,
                devEUI,
                time,
                rssi,
                loRaSNR,
                temperatureSensor,
                humiditySensor,
                latitude,
                longitude,
                altitude))


# Connect to database
with psycopg2.connect(
    dbname='postgres',
    user='postgres',
    password='postgres',
    host='localhost') as conn:
    with conn.cursor() as cur:
        print('PostgreSQL database version:')
        cur.execute('SELECT version()')
        db_version = cur.fetchone()
        print(db_version)

# Connect to MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_message = on_message
while True:
    try:
        client.connect(**config.mqtt_connection)
        client.loop_forever()
    except ConnectionRefusedError:
        time.sleep(5)
        pass
