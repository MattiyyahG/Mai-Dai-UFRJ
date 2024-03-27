#.\venv\scripts\activate
#uvicorn main:app --reload

from typing import Union

from fastapi import FastAPI

from fastapi_mqtt import FastMQTT, MQTTConfig


from motor.motor_asyncio import AsyncIOMotorClient


MONGO_URL = "mongodb+srv://maidai:123maidai456@maidai.e3qdm2x.mongodb.net/?retryWrites=true&w=majority&appName=MaiDai"

client = AsyncIOMotorClient(MONGO_URL)

database = client["MaiDai"]

collection = database["uva"]

app = FastAPI()


mqtt_config = MQTTConfig(
    host = "test.mosquitto.org",
    port = '1883',
    keepalive = 60,
    #username = "ESP8266Client-",
    #password = "",
)

mqtt = FastMQTT(
    config=mqtt_config

)

mqtt.init_app(app)


@mqtt.on_connect()
def connect(client, flags, rc , properties):
    mqtt.client.subscribe("/MaiDai")
    print("Conectado: ", client, flags, rc, properties)

@mqtt.on_message()
async def message(client, topic, payload, qos, properties):

    global collection

    data = payload.decode(encoding='UTF-8')

    collection.insert_one({"topic": topic, "Umidade(%)" : data[:5], "ID": data[6]})

    print(f"Mensagem recebida: ", data, "id: ", data[6])

    return 0



@mqtt.on_disconnect()
def disconnect(client, packet, exc=None):
    print("Desconectado")

@mqtt.on_subscribe()
def subscribe(client, mid, qos, properties):
    print("Inscrito", client, mid, qos , properties)
  
@mqtt.subscribe("MaiDai/Uva")
async def message_to_topic(client, topic, payload, qos, properties):
    '''print("Mensagem recebida no tópico: ", topic, payload.decode(), qos, properties)'''

@app.get("/")
async def func():
    mqtt.publish("MaiDai/Uva", "teste")

    return {"Resultado": "Conectado"}

