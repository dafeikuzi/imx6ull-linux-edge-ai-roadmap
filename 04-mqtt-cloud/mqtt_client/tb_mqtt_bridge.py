#!/usr/bin/env python3
"""Publish IMX6ULL JSON telemetry to ThingsBoard over MQTT.

Install dependency:
    pip install paho-mqtt

Usage:
    python tb_mqtt_bridge.py --host 192.168.1.10 --token THINGSBOARD_TOKEN
    ./sensor_collector ../config/collector.conf | python tb_mqtt_bridge.py --host ... --token ...
"""

import argparse
import json
import sys
import time

import paho.mqtt.client as mqtt


TELEMETRY_TOPIC = "v1/devices/me/telemetry"
RPC_TOPIC = "v1/devices/me/rpc/request/+"


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", required=True, help="MQTT broker or ThingsBoard host")
    parser.add_argument("--port", type=int, default=1883)
    parser.add_argument("--token", required=True, help="ThingsBoard device access token")
    parser.add_argument("--client-id", default="imx6ull-edge-bridge")
    return parser.parse_args()


def on_connect(client, _userdata, _flags, rc):
    print(f"[mqtt] connected rc={rc}", file=sys.stderr)
    client.subscribe(RPC_TOPIC)


def on_message(_client, _userdata, msg):
    payload = msg.payload.decode("utf-8", errors="replace")
    print(f"[rpc] topic={msg.topic} payload={payload}", file=sys.stderr)


def main():
    args = parse_args()
    client = mqtt.Client(client_id=args.client_id)
    client.username_pw_set(args.token)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(args.host, args.port, keepalive=60)
    client.loop_start()

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        try:
            payload = json.loads(line)
        except json.JSONDecodeError as exc:
            print(f"[skip] invalid json: {exc}: {line}", file=sys.stderr)
            continue
        info = client.publish(TELEMETRY_TOPIC, json.dumps(payload), qos=1)
        info.wait_for_publish()
        print(f"[pub] {payload}", file=sys.stderr)
        time.sleep(0.05)

    client.loop_stop()
    client.disconnect()


if __name__ == "__main__":
    main()

