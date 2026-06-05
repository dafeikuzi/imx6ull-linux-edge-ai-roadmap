#!/usr/bin/env python3
"""Subscribe IMX6ULL Zenoh data, store it in SQLite, and print LLM advice.

This is a PC-side prototype. Keep IMX6ULL focused on collection and control.
"""

import json
import sqlite3
import time

import requests
import zenoh


SENSOR_KEY = "demo/imx6ull/sensor"
CONTROL_KEY = "demo/imx6ull/control/led"
DB_PATH = "../database/imx6ull_edge.db"
OLLAMA_URL = "http://127.0.0.1:11434/api/generate"
OLLAMA_MODEL = "qwen2.5:1.5b"


def init_db():
    conn = sqlite3.connect(DB_PATH)
    conn.execute(
        """
        create table if not exists sensor_records (
            id integer primary key autoincrement,
            ts integer not null,
            device text not null,
            temperature real,
            humidity real,
            key_state integer,
            led_state integer,
            raw_json text not null
        )
        """
    )
    conn.commit()
    return conn


def store_record(conn, payload):
    conn.execute(
        "insert into sensor_records(ts, device, temperature, humidity, key_state, led_state, raw_json) values (?, ?, ?, ?, ?, ?, ?)",
        (
            int(payload.get("timestamp", time.time())),
            payload.get("device", "unknown"),
            payload.get("temperature"),
            payload.get("humidity"),
            payload.get("key_state"),
            payload.get("led_state"),
            json.dumps(payload, ensure_ascii=False),
        ),
    )
    conn.commit()


def local_rule(payload):
    temp = float(payload.get("temperature", 0))
    if temp >= 35:
        return {"led": 1, "reason": "temperature_high"}
    return {"led": 0, "reason": "normal"}


def ask_ollama(payload):
    prompt = (
        "你是嵌入式Linux边缘设备诊断助手。"
        "请根据这条IMX6ULL传感器数据判断是否异常，并给出一句简短建议："
        f"{json.dumps(payload, ensure_ascii=False)}"
    )
    try:
        resp = requests.post(
            OLLAMA_URL,
            json={"model": OLLAMA_MODEL, "prompt": prompt, "stream": False},
            timeout=8,
        )
        resp.raise_for_status()
        return resp.json().get("response", "").strip()
    except Exception as exc:
        return f"LLM unavailable, fallback rule used: {exc}"


def main():
    conn = init_db()
    session = zenoh.open(zenoh.Config())
    control_pub = session.declare_publisher(CONTROL_KEY)

    def callback(sample):
        text = sample.payload.to_string()
        try:
            payload = json.loads(text)
        except json.JSONDecodeError:
            print(f"[skip] invalid json: {text}")
            return
        store_record(conn, payload)
        decision = local_rule(payload)
        advice = ask_ollama(payload)
        control_payload = {"led": decision["led"], "reason": decision["reason"], "advice": advice}
        control_pub.put(json.dumps(control_payload, ensure_ascii=False))
        print(f"[sensor] {payload}")
        print(f"[control] {control_payload}")

    sub = session.declare_subscriber(SENSOR_KEY, callback)
    print(f"[service] subscribed {SENSOR_KEY}; publishing control to {CONTROL_KEY}")
    try:
        while True:
            time.sleep(1)
    finally:
        sub.undeclare()
        session.close()
        conn.close()


if __name__ == "__main__":
    main()

