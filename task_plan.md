# Task Plan

## Goal

Create a reusable IMX6ULL embedded Linux internship roadmap repository with staged project structure, learning documents, reproducible checklists, and starter code for Linux system bring-up, drivers, user-space apps, MQTT cloud, edge AI, and cloud-edge LLM demos.

## Phases

| Phase | Status | Output |
|---|---|---|
| Inspect workspace | complete | Confirmed workspace contains board materials and no root git repo. |
| Create repository structure | complete | Created `imx6ull-linux-edge-ai-roadmap/` with staged directories. |
| Add roadmap documentation | complete | Added README, learning route, architecture, interview notes, troubleshooting, evidence template. |
| Add starter implementations | complete | Added C user app, LED control, MQTT Python bridge, Zenoh/LLM Python skeletons, driver/device-tree templates. |
| Verify files | complete | Listed project files and checked key source files with Python/C syntax checks. |
| Expand 01 Linux system sprint | complete | Added one-week sprint plan, full knowledge guide, boot/kernel/rootfs/U-Boot notes, NFS workflow, hello cross-compile exercise, and evidence guidance. |
| Expand 02 drivers core sprint | complete | Added Stage 2 knowledge guide, sprint plan, per-day directories, LED/KEY/UART/I2C templates, device-tree notes, and interview review material. |

## Decisions

- This repository is a portfolio/learning roadmap, not a full BSP clone.
- IMX6ULL handles Linux, drivers, collection, control, light AI, and communication.
- PC/server handles ThingsBoard, databases, Zenoh router, Ollama/LocalAI, and heavier AI.
- Keep code small and readable so each stage can be reproduced and explained in interviews.
- Stage 1 uses Ubuntu virtual machine + IMX6ULL_Pro real board as the default environment.
- Stage 1 follows an "theory first, daily evidence" one-week sprint.
- Use 韦东山 IMX6ULL_Pro BSP first for bring-up; Buildroot remains an enhancement path after the first closed loop.
- Stage 2 focuses on the core driver loop rather than every possible IMX6ULL peripheral: LED, KEY IRQ/poll, Device Tree, UART, I2C, and dmesg.
- Every Stage 2 sub-stage must keep its source, device-tree snippets, logs, and evidence in its own directory; do not place experiment artifacts in `02-drivers/` root.

## Errors

| Error | Attempt | Resolution |
|---|---|---|
| `git status` failed because workspace root is not a git repository. | Checked root status. | Treat new roadmap folder as standalone project; do not rely on git metadata. |
| `python` command returned Windows Store placeholder exit code 9009. | Tried `python -m py_compile`. | Used `py -3 -m py_compile` instead. |
| Full Windows GCC compile returned exit 1 during backend/assembly stage with no useful diagnostic. | Tried direct compile in project path and ASCII temp path. | Used `gcc -fsyntax-only` to verify C source syntax; final target compile should be done in Linux/WSL/cross toolchain. |
