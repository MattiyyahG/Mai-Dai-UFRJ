import pika
import json
import time
import random
from datetime import datetime

# Função para simular leitura de um sensor com formato específico
def read_sensor_data():
    umidade = random.uniform(30.0, 80.0)  # Simulação de umidade entre 30% e 80%
    id_sensor = 2  # ID do sensor, conforme o código C++
    
    # Capturar data e hora
    coleta_d = datetime.now().strftime("%Y-%m-%d")  # Data no formato YYYY-MM-DD
    coleta_h = datetime.now().strftime("%H:%M:%S")  # Hora no formato HH:MM:SS
    
    # Retorna os dados no formato: "umidade id_sensor coleta_d coleta_h"
    return f"{umidade:.1f} {id_sensor} {coleta_d} {coleta_h}"

# Configurações do RabbitMQ com autenticação
rabbitmq_host = 'localhost'  # Endereço do broker
rabbitmq_queue = 'sensor_data_queue'
rabbitmq_user = 'fazenda'  # Usuário
rabbitmq_password = 'steffenthanos2020'  # Senha

# Conexão com o RabbitMQ usando credenciais
credentials = pika.PlainCredentials(rabbitmq_user, rabbitmq_password)
connection = pika.BlockingConnection(pika.ConnectionParameters(host=rabbitmq_host, credentials=credentials))
channel = connection.channel()

# Declarar a fila
channel.queue_declare(queue=rabbitmq_queue)

# Enviar dados do sensor para a fila a cada 5 segundos
try:
    while True:
        sensor_data = read_sensor_data()
        channel.basic_publish(
            exchange='',
            routing_key=rabbitmq_queue,
            body=sensor_data  # Publicar dados como string, sem json.dumps
        )
        print(f"[x] Sent {sensor_data}")
        time.sleep(10)
except KeyboardInterrupt:
    print("Interrupted!")
finally:
    connection.close()
