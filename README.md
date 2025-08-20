# esphome-lolan-bms

![GitHub actions](https://github.com/syssi/esphome-lolan-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-lolan-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-lolan-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-lolan-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor a Lolan Battery Management System via BLE

## Supported devices

* Lolan Smart BMS, 20240511003, 8S 45A advertised via BLE as `LOLAN8S45A`

## Requirements

* [ESPHome 2024.6.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32 board

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-lolan-bms@main
```

or just use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-lolan-bms.git
cd esphome-lolan-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
esphome run esp32-ble-example.yaml

```

## Example response all sensors enabled

```
[I][lolan_bms_ble:194]: Status frame received
[D][lolan_bms_ble:195]:   01.00.03.00.41.B9.06.15.00.00.00.00.00.00.00.00.41.D7.CE.40.41.CD.38.60.42.9D.93.20.42.47.9E.44.00.36.3E.50.00.01.00.27 (40)
[D][binary_sensor:026]: 'lolan-bms-ble discharging': New state is ON
[D][binary_sensor:026]: 'lolan-bms-ble charging': New state is ON
[D][switch:055]: 'lolan-bms-ble discharging': Sending state ON
[D][switch:055]: 'lolan-bms-ble charging': Sending state ON
[D][sensor:104]: 'lolan-bms-ble error bitmask': Sending state 0.00000  with 0 decimals of accuracy
[V][text_sensor:015]: 'lolan-bms-ble errors': Received new state 
[D][text_sensor:069]: 'lolan-bms-ble errors': Sending state ''
[D][sensor:104]: 'lolan-bms-ble total voltage': Sending state 23.12797 V with 2 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble current': Sending state 0.00000 A with 2 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble power': Sending state 0.00000 W with 2 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble charging power': Sending state 0.00000 W with 2 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble discharging power': Sending state 0.00000 W with 2 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble temperature 1': Sending state 26.97571 °C with 1 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble temperature 2': Sending state 25.65253 °C with 1 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble total discharged capacity': Sending state 78.78735 Ah with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble total charged capacity': Sending state 49.90456 Ah with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble total runtime': Sending state 3554896.00000 s with 0 decimals of accuracy
[V][text_sensor:015]: 'lolan-bms-ble total runtime formatted': Received new state 41d 3h
[D][text_sensor:069]: 'lolan-bms-ble total runtime formatted': Sending state '41d 3h'
[D][sensor:104]: 'lolan-bms-ble charging cycles': Sending state 1.00000  with 0 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble state of charge': Sending state 39.00000 % with 0 decimals of accuracy

[I][lolan_bms_ble:262]: Cell info frame received
[D][lolan_bms_ble:263]:   02.00.07.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.40.56.66.66.40.53.30.A4.40.53.3A.E1.40.53.45.1F.40.53.38.52.40.53.45.1F.40.53.45.1F.40.53.3D.71.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00 (88)
[I][lolan_bms_ble:274]:   Cell count: 7
[I][lolan_bms_ble:283]:   Balancing mask: 0
[D][sensor:104]: 'lolan-bms-ble balancer voltage': Sending state 3.35000 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 1': Sending state 3.29984 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 2': Sending state 3.30047 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 3': Sending state 3.30109 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 4': Sending state 3.30031 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 5': Sending state 3.30109 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 6': Sending state 3.30109 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble cell voltage 7': Sending state 3.30063 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble min cell voltage': Sending state 3.29984 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble max cell voltage': Sending state 3.30109 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble max voltage cell': Sending state 3.00000  with 0 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble min voltage cell': Sending state 1.00000  with 0 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble delta cell voltage': Sending state 0.00125 V with 3 decimals of accuracy
[D][sensor:104]: 'lolan-bms-ble average cell voltage': Sending state 3.30065 V with 4 decimals of accuracy

[I][lolan_bms_ble:349]: Settings frame received
[D][lolan_bms_ble:350]:   03.6C.00.BC.61.4E.0C.00.00.32.00.50.08.0F.07.01.FF.FA.00.01.42.51.09.41.40.56.66.66.3F.80.00.00.40.B7.95.81.40.B2.B8.52.41.9A.00.00.41.CC.00.00.40.30.00.00.40.69.99.9A.42.82.00.00.42.82.00.00.46.1C.40.00.CC.10.F1.52.01.34.8B.9A.40.30.00.00.40.53.33.33.3C.23.D7.0A.41.C0.CC.CD.40.5C.CC.CD.41.A2.66.66.40.39.99.9A.35.E2.5A.A5 (108)
[I][lolan_bms_ble:367]:   Unknown1: 108
[I][lolan_bms_ble:370]:   BLE password: 12345678
[I][lolan_bms_ble:373]:   Output voltage offset: 12
[I][lolan_bms_ble:376]:   Input voltage offset: 0
[I][lolan_bms_ble:379]:   Max charging current: 50 A
[I][lolan_bms_ble:382]:   Max discharging current: 80 A
[I][lolan_bms_ble:385]:   Charging current zero: 8
[I][lolan_bms_ble:388]:   Discharging current zero: 15
[I][lolan_bms_ble:391]:   Cell count: 7
[I][lolan_bms_ble:394]:   Battery type: 1
[I][lolan_bms_ble:397]:   mNC2: 255
[I][lolan_bms_ble:400]:   Cycle count: 1
[I][lolan_bms_ble:403]:   Battery capacity: 52.259
[I][lolan_bms_ble:406]:   Start balancing voltage: 3.350
[I][lolan_bms_ble:409]:   Unknown28: 1.000000
[I][lolan_bms_ble:412]:   Reference discharging voltage: 5.737 V
[I][lolan_bms_ble:415]:   Reference charging voltage: 5.585 V
[I][lolan_bms_ble:418]:   Pack undervoltage protection: 19.250 V
[I][lolan_bms_ble:421]:   Pack overvoltage protection: 25.500 V
[I][lolan_bms_ble:424]:   Cell undervoltage protection: 2.750 V
[I][lolan_bms_ble:427]:   Cell overvoltage protection: 3.650 V
[I][lolan_bms_ble:430]:   Mosfet overtemperature protection: 65.000 °C
[I][lolan_bms_ble:433]:   Circuit temperature protection: 65.000 °C
[I][lolan_bms_ble:436]:   Temperature adjustment: 10000.000
[I][lolan_bms_ble:439]:   Serial number: 3423662418
[I][lolan_bms_ble:442]:   Software date: 20220826
[I][lolan_bms_ble:445]:   Lowpower: 2.750 V
[I][lolan_bms_ble:448]:   Battery4 adjustment: 3.300
[I][lolan_bms_ble:451]:   Voltage Equal: 0.010 V
[I][lolan_bms_ble:454]:   Overcharge release voltage: 24.100 V
[I][lolan_bms_ble:457]:   Overcharge cell voltage: 3.450 V
[I][lolan_bms_ble:460]:   Discharge release voltage: 20.300 V
[I][lolan_bms_ble:463]:   Discharge cell voltage: 2.900 V
```

## Protocol

TBD.

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to increase the log level to see details about the BLE communication and incoming traffic:

```
logger:
  level: VERY_VERBOSE
  logs:
    esp32_ble: DEBUG
    esp32_ble_tracker: VERY_VERBOSE
    lolan_bms_ble: VERY_VERBOSE
    scheduler: DEBUG
    component: DEBUG
    sensor: DEBUG
    mqtt: INFO
    mqtt.idf: INFO
    mqtt.component: INFO
    mqtt.sensor: INFO
    mqtt.switch: INFO
    api.service: INFO
    api: INFO
    api: INFO
```

## References

None.
