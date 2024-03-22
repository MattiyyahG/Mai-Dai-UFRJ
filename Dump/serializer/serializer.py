def convertUva(uva) -> dict:
    return {
        "id": str(uva["_id"]),
        "sensor_id": uva['author_id'],
        "dado coletado": uva['dado_coletado'],
        "tempo de coleta": uva['tempo_de_coleta']
    }

def convertdb_uva(db_uva) -> list:
    return[convertUva(uva) for uva  in db_uva]