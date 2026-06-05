#!/bin/sh
cd "$(dirname "$0")/../sensor_collector" || exit 1
exec ./sensor_collector ../config/collector.conf

