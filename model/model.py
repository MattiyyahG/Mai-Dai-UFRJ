from pydantic import BaseModel

class uva(BaseModel):
    author_id: int
    dado_coletado: float
    tempo_de_coleta: int


