from fastapi import FastAPI
from pydantic import BaseModel
from typing import List, Optional
from datetime import datetime
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

# Allow frontend on same network
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # in production, restrict this
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class Telemetry(BaseModel):
    device_id: str
    status: str
    dust_level: int
    voc_level: int
    battery_level: int
    energy_used: float
    session_id: str
    recovered_batteries: int
    battery_condition: str
    timestamp: Optional[datetime] = None

# In-memory storage (for demo)
telemetry_store: List[Telemetry] = []


@app.post("/telemetry")
def receive_telemetry(data: Telemetry):
    # If timestamp not provided, use server time
    if data.timestamp is None:
        data.timestamp = datetime.utcnow()

    telemetry_store.append(data)

    return {"message": "telemetry received", "count": len(telemetry_store)}


@app.get("/telemetry")
def list_telemetry(limit: int = 100):
    # Return latest N records
    return telemetry_store[-limit:]


@app.get("/summary")
def get_summary():
    if not telemetry_store:
        return {
            "total_records": 0,
            "avg_dust_level": 0,
            "avg_voc_level": 0,
            "avg_battery_level": 0,
            "total_energy_used": 0,
            "total_recovered_batteries": 0,
        }

    n = len(telemetry_store)
    total_dust = sum(t.dust_level for t in telemetry_store)
    total_voc = sum(t.voc_level for t in telemetry_store)
    total_battery = sum(t.battery_level for t in telemetry_store)
    total_energy = sum(t.energy_used for t in telemetry_store)
    total_batteries = sum(t.recovered_batteries for t in telemetry_store)

    return {
        "total_records": n,
        "avg_dust_level": round(total_dust / n, 2),
        "avg_voc_level": round(total_voc / n, 2),
        "avg_battery_level": round(total_battery / n, 2),
        "total_energy_used": round(total_energy, 2),
        "total_recovered_batteries": total_batteries,
    }


@app.get("/")
def root():
    return {"message": "Eco-Guardian API running"}
