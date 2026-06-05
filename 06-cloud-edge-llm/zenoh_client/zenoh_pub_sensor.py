#!/usr/bin/env python3
"""Publish sample IMX6ULL sensor data through Zenoh."""

import json
import random
import time

import zenoh


KEY = "demo/imx6ull/sensor"


def main():
    session = zenoh.open(zenoh.Config())
    pub = session.declare_publisher(KEY)
    try:
        while True:
            payload = {
                "device": "imx6ull-pro",
                "timestamp": int(time.time()),
                "temperature": round(random.uniform(25.0, 38.0), 2),
                "humidity": round(random.uniform(45.0, 70.0), 2),
                "key_state": 0,
                "led_state": 0,
            }
            data = json.dumps(payload)
            pub.put(data)
            print(f"[zenoh pub] {KEY} {data}")
            time.sleep(1)
    finally:
        session.close()


if __name__ == "__main__":
    main()

