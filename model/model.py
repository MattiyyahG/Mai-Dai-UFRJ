from pydantic import BaseModel

class uva(BaseModel):
    id: int  = None
    #sensor_id: int
    dado_coletado: float
    #tempo_de_coleta: int


