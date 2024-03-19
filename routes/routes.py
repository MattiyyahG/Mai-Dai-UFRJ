from fastapi import APIRouter
from config.config import maidaiCollection
from model.model import uva
from serializer.serializer import convertdb_uva, convertUva
from bson import ObjectId

endPoints = APIRouter()

@endPoints.get("/all/db_uva")
def getAllDb_uva():
    db_uva = maidaiCollection.find()
    convertdb_uva = convertdb_uva(db_uva)
    return {
        "status": "Ok",
        "data" : convertdb_uva
    }

@endPoints.get("/uva/{id}")
def getUva(id:str):
    uva = maidaiCollection.find_one({"_id" : ObjectId(id)})
    convertedUva = convertUva(uva)
    return {
        "status" : "Ok",
        "data" : convertedUva
    }

