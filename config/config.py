from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi

uri = "mongodb+srv://maidai:123maidai456@maidai.e3qdm2x.mongodb.net/?retryWrites=true&w=majority&appName=MaiDai"

# Create a new client and connect to the server
client = MongoClient(uri, server_api=ServerApi('1'))

db = client.maiDai

maidaiCollection = db['db_uva']

#userCollection = db['users']

# Send a ping to confirm a successful connection
try:
    client.admin.command('ping')
    print("Pinged your deployment. You successfully connected to MongoDB!")
except Exception as e:
    print(e)