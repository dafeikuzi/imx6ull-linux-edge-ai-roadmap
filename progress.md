# Progress Log

## 2026-06-05

- Created `imx6ull-linux-edge-ai-roadmap/`.
- Created staged directories from `01-linux-system/` through `06-cloud-edge-llm/` plus `final-demo/`.
- Started persistent planning files: `task_plan.md`, `findings.md`, `progress.md`.
- Added roadmap documents: root README, learning route, architecture, interview notes, troubleshooting, evidence template.
- Added stage README files for Linux system, drivers, user app, MQTT cloud, edge AI, cloud-edge LLM, and final demo.
- Added starter code:
  - `03-user-app/sensor_collector/sensor_collector.c`
  - `03-user-app/led_control/led_control.c`
  - `04-mqtt-cloud/mqtt_client/tb_mqtt_bridge.py`
  - `06-cloud-edge-llm/zenoh_client/zenoh_pub_sensor.py`
  - `06-cloud-edge-llm/data_service/edge_data_service.py`
- Verification:
  - Python syntax check passed with `py -3 -m py_compile`.
  - C syntax check passed with `gcc -fsyntax-only`.
  - Full Windows GCC build did not complete in this shell; use Linux/WSL/ARM cross toolchain for actual target binaries.
- Removed generated `__pycache__` files after verification.
