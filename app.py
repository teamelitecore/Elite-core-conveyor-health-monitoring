from flask import Flask, request, jsonify
from datetime import datetime
import sqlite3
import os

app = Flask(__name__)

# ============================================================
# DATABASE
# ============================================================

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATABASE = os.path.join(BASE_DIR, "sensor_data.db")


def init_database():
    connection = sqlite3.connect(DATABASE)

    cursor = connection.cursor()

    cursor.execute("""
        CREATE TABLE IF NOT EXISTS sensor_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            belt_id TEXT NOT NULL,
            temperature REAL,
            vibration REAL,
            load REAL,
            speed REAL,
            acoustic REAL,
            current REAL
        )
    """)

    connection.commit()
    connection.close()


# Create database/table when Flask starts
init_database()


# ============================================================
# LATEST SENSOR DATA
# ============================================================

latest_data = {
    "belt_id": "CV-01",
    "timestamp": None,
    "temperature": None,
    "vibration": None,
    "load": None,
    "speed": None,
    "acoustic": None,
    "current": None
}


# ============================================================
# HOME
# ============================================================

@app.route("/")
def home():
    return "ELITE CORE Backend is running"


# ============================================================
# LIVE DATA
# ============================================================

@app.route("/api/live", methods=["GET"])
def get_live_data():
    return jsonify(latest_data)


# ============================================================
# RECEIVE SENSOR DATA
# ============================================================

@app.route("/api/sensor-data", methods=["POST"])
def receive_sensor_data():

    global latest_data

    data = request.get_json()

    if not data:
        return jsonify({
            "success": False,
            "message": "No JSON data received"
        }), 400

    timestamp = datetime.now().isoformat()

    latest_data = {
        "belt_id": data.get("belt_id", "CV-01"),
        "timestamp": timestamp,
        "temperature": data.get("temperature"),
        "vibration": data.get("vibration"),
        "load": data.get("load"),
        "speed": data.get("speed"),
        "acoustic": data.get("acoustic"),
        "current": data.get("current")
    }


    # ========================================================
    # SAVE SENSOR READING TO SQLITE
    # ========================================================

    connection = sqlite3.connect(DATABASE)

    cursor = connection.cursor()

    cursor.execute("""
        INSERT INTO sensor_data (
            timestamp,
            belt_id,
            temperature,
            vibration,
            load,
            speed,
            acoustic,
            current
        )
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    """, (
        timestamp,
        latest_data["belt_id"],
        latest_data["temperature"],
        latest_data["vibration"],
        latest_data["load"],
        latest_data["speed"],
        latest_data["acoustic"],
        latest_data["current"]
    ))

    connection.commit()
    connection.close()


    return jsonify({
        "success": True,
        "message": "Sensor data received and stored",
        "data": latest_data
    })


# ============================================================
# START FLASK
# ============================================================

if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )