from fastapi import FastAPI
from pymongo import MongoClient
import pika
import json
import os

app = FastAPI()

# Configurações do RabbitMQ
rabbitmq_user = 'fazenda'
rabbitmq_password = 'steffenthanos2020'
rabbitmq_host = os.getenv('RABBITMQ_HOST', 'host.docker.internal')
rabbitmq_queue = os.getenv('RABBITMQ_QUEUE', 'sensor_data_queue')

# Configurações do MongoDB
mongo_uri = 'mongodb+srv://maidai:123maidai456@maidai.e3qdm2x.mongodb.net/?retryWrites=true&w=majority&appName=MaiDai'  # Substitua pela URI do seu MongoDB
mongo_db = 'MaiDai'
mongo_collection = 'uva'

# Conectar ao MongoDB
mongo_client = MongoClient(mongo_uri)
db = mongo_client[mongo_db]
collection = db[mongo_collection]

# Função para processar mensagens do RabbitMQ
def callback(ch, method, properties, body):
    # Decodificar a mensagem
    message = body.decode('utf-8')
    umidade, id_sensor, coleta_d, coleta_h = message.split()

    # Criar o documento a ser inserido
    document = {
        'umidade': float(umidade),
        'id_sensor': int(id_sensor),
        'data': coleta_d,
        'hora': coleta_h
    }

    # Inserir no MongoDB
    collection.insert_one(document)
    print(f"Mensagem inserida no MongoDB: {document}")

# Configurações da conexão RabbitMQ
credentials = pika.PlainCredentials(rabbitmq_user, rabbitmq_password)
connection = pika.BlockingConnection(pika.ConnectionParameters(host=rabbitmq_host, credentials=credentials))
channel = connection.channel()
channel.queue_declare(queue=rabbitmq_queue)
channel.basic_consume(queue=rabbitmq_queue, on_message_callback=callback, auto_ack=True)

# Iniciar o consumo de mensagens
def start_consuming():
    print('Iniciando o consumo de mensagens...')
    channel.start_consuming()

@app.on_event("startup")
async def startup_event():
    # Iniciar o consumo de mensagens em uma thread separada
    import threading
    consumer_thread = threading.Thread(target=start_consuming)
    consumer_thread.daemon = True
    consumer_thread.start()

@app.get("/")
def read_root():
    return {"message": "API está funcionando e consumindo mensagens do RabbitMQ"}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
