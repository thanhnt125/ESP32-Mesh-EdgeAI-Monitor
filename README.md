# ESP32 Mesh Edge-AI Monitor

> **Status: In progress (week 6)** — coursework project for *Telecommunication Systems* (HUST).

Wireless temperature/humidity monitoring system: ESP32 nodes read multiple **SHT30** sensors, run **Edge AI anomaly detection** on-device (TinyML, Edge Impulse), and report to a server over the network. The final target is a **3-node ESP32 mesh** (painlessMesh) with one node acting as root/gateway.

## Current progress

| Week 6 result | Detail |
|---|---|
| 2 × SHT30 on one ESP32 | Read simultaneously using the ESP32's **two hardware I2C buses** (`Wire`, `Wire1`) — SHT30 only has two addresses (0x44 / 0x45) |
| Web dashboard | Live temperature/humidity for sensors S1 and S2 |
| Edge AI dataset | Two time-series datasets collected with heating/cooling and humid/dry cycles (see below) |

**Blocker:** a third SHT30 cannot be added directly (only 2 hardware I2C buses, shared address). **Planned fix:** TCA9548A I2C multiplexer — 8 isolated channels, one sensor per channel, sensors identified by channel number.

## Dataset (for Edge Impulse)

| File | Samples | Range | Columns |
|---|---|---|---|
| `data/nhiet_02.csv` | 3 707 | 36.9 – 97.5 °C | `timestamp` (ms), `temperature`, `humidity` |
| `data/am_2.csv` | 5 402 | 22.6 – 94.1 %RH | `timestamp` (ms), `temperature`, `humidity` |

Used to train an anomaly-detection model on Edge Impulse, then deploy it to the ESP32.

## Design requirements (from the supervisor) — next steps

1. **Sensor IDs** — each sensor has a fixed `id`, a `location` group and a mux `channel`; every message carries its `id`.
2. **Interval, round-robin sending** — instead of real-time streaming, read and send **one sensor per slot**, cycling through all sensors.
3. **Switching interval** — one SHT30 high-repeatability measurement takes ≈ 15 ms (+ ~1–2 ms mux switching), so hardware is not the limit. The gap between two sends is chosen as

   $$\Delta = \frac{T}{N}$$

   where *T* is the desired period per sensor and *N* the number of sensors. Example: *N* = 3, *T* = 30 s → **Δ = 10 s**, and 10 000 ms ≫ 17 ms.
4. **Same-location merging** — sensors sharing a `location` are averaged and sent as **one** message (redundancy + noise reduction, no duplicate traffic).
5. **Two Edge AI ideas**
   - *Per-sensor anomaly detection* on each sensor's own time series.
   - *Cross-sensor anomaly detection* — flag one sensor deviating from the others in the same area (faulty sensor or local incident).

## Architecture (target)

```
[Node 1: ESP32 + SHT30s + TinyML] ─┐
                                   ├─ painlessMesh ─► [Root: ESP32] ─ WiFi/MQTT ─► [Laptop server: broker + dashboard]
[Node 2: ESP32 + SHT30s + TinyML] ─┘
```

Future work: **task offloading (MEC)** — a node decides whether to run inference locally or offload heavier models to the root/server.

## Roadmap

- [x] Single ESP32 + SHT30 with local threshold alert (`firmware/archive/milestone1_single_sht30`)
- [x] Two SHT30 on one ESP32 via dual I2C buses
- [x] Web dashboard for S1/S2
- [x] Collect temperature/humidity datasets for Edge AI
- [ ] Third sensor via TCA9548A
- [ ] Sensor IDs, round-robin sending (Δ = T/N), same-location merging
- [ ] Train anomaly-detection model on Edge Impulse and deploy to ESP32
- [ ] Cross-sensor anomaly detection
- [ ] 3-node painlessMesh with root bridge to MQTT

## Repository layout

```
firmware/   ESP32 code (archive/ holds earlier milestones)
data/       sensor datasets (CSV) for Edge Impulse
docs/       weekly technical journals, progress reports, diagrams, photos
```

## Hardware

ESP32 DevKit (30-pin) · SHT30 (I2C) · TCA9548A I2C multiplexer (planned) · LED + buzzer for local alerts

## References

Sensirion SHT3x-DIS datasheet · Espressif ESP32 Series Datasheet · NXP UM10204 I2C specification · OASIS MQTT specification
