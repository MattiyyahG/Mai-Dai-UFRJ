#.\venv\scripts\activate
#uvicorn main2_0:app --reload

from typing import Union

from fastapi import FastAPI, HTTPException

from fastapi_mqtt import FastMQTT, MQTTConfig

from routes.routes import endPoints

from motor.motor_asyncio import AsyncIOMotorClient

from model.model import uva

import json

MONGO_URL = "mongodb+srv://maidai:123maidai456@maidai.e3qdm2x.mongodb.net/?retryWrites=true&w=majority&appName=MaiDai"

client = AsyncIOMotorClient(MONGO_URL)

database = client["MaiDai"]

collection = database["c_Uva"]

app = FastAPI()

app.include_router(endPoints)

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

    #data = topic, payload.decode(encoding='UTF-8'), qos

    global collection

    data = json.loads(payload.decode(encoding='UTF-8'))

    print(f"Mensagem recebida: ", data)

    collection.insert_one(data)


@mqtt.on_disconnect()
def disconnect(client, packet, exc=None):
    print("Desconectado")

@mqtt.on_subscribe()
def subscribe(client, mid, qos, properties):
    print("Inscrito", client, mid, qos , properties)
  
@mqtt.subscribe("MaiDai/Uva")
async def message_to_topic(client, topic, payload, qos, properties):
    '''print("Mensagem recebida no tópico: ", topic, payload.decode(), qos, properties)'''

'''@app.get("/")
async def func():
    mqtt.publish("MaiDai/Uva", "teste")

    return {"resultado": "message"}
'''
'''
@app.get("/items/{item_id}")
def read_item(item_id: int, q: Union[str, None] = None):
    return {"item_id": item_id, "q": q}
'''

@app.post("/items/", response_model=uva)
async def create_item(item: uva):
    '''result = await'''
    collection.insert_one(item.model_dump())
    #item.id = str(result.inserted_id)
    #return item


@app.get("/items/{item_id}", response_model=uva)
async def read_item(item_id: str):
    item = await collection.find_one({"_id": item_id})
    if item:
        return item
    raise HTTPException(status_code=404, detail="Item not found")


@app.put("/items/{item_id}", response_model=uva)
async def update_item(item_id: str, item:  uva):
    update_item = await collection.find_one_and_update(
        { "_id": item_id }, {"$set": item.model_dump()}
    )
    if update_item:
        return item
    raise HTTPException(status_code=404, detail="Item not found")

@app.delete("/items/{item_id}", response_model=uva)
async def delete_item(item_id: str):
    deleted_item = await collection.find_one_and_delete({"_id": item_id})
    if deleted_item:
        return deleted_item
    raise HTTPException(status_code=404, detail="Item not found")


@endPoints.get("/")
def home():
    return{

    }