from typing import Union

from fastapi import FastAPI

from fastapi_mqtt import FastMQTT, MQTTConfig

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
async def message(client, topic, payload, qos , properties):
    print(f"Mensagem recebida: ", topic, payload.decode(), qos, properties)
    return 0

@mqtt.on_disconnect()
def disconnect(client, packet, exc=None):
    print("Desconectado")

@mqtt.on_subscribe()
def subscribe(client, mid, qos , properties):
    print("Inscrito", client, mid, qos , properties)
  
@mqtt.subscribe("MaiDai/Uva")
async def message_to_topic(client, topic, payload, qos, properties):
    '''print("Mensagem recebida no tópico: ", topic, payload.decode(), qos, properties)'''

@app.get("/")
async def func():
    mqtt.publish("MaiDai/Uva", "teste")

    return {"resultado": True, "Testando": "testando TESTE"}


'''
@app.get("/items/{item_id}")
def read_item(item_id: int, q: Union[str, None] = None):
    return {"item_id": item_id, "q": q}
'''


