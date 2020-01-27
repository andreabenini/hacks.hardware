Detect mDns devices where available on the network
```sh
avahi-browse --all --ignore-local --resolve
#+ enp0s31f6 IPv4 eWeLink_9999aa8888                            _ewelink._tcp        local
#= enp0s31f6 IPv4 eWeLink_9999aa8888                            _ewelink._tcp        local
#   hostname = [eWeLink_9999aa8888.local]
#   address = [192.168.1.188]
#   port = [8081]
#   txt = ["data1={"switch":"off","startup":"off","pulse":"off","sledOnline":"on","pulseWidth":500,"rssi":-37}"
#                  "seq=7" "apivers=1" "type=diy_plug" "id=9999aa8888" "txtvers=1"]
```

Get information from device
```sh
curl http://192.168.1.188:8081/zeroconf/info -XPOST --data '{"deviceid":"9999aa8888","data":{} }'
# Where '9999aa8888' is device id obtained from mDns query
```
Unlock OTA (Over The Air) update
```sh
curl http://192.168.1.188:8081/zeroconf/ota_unlock -XPOST --data '{"deviceid":"9999aa8888","data":{} }'
```
Get information from device again to see if OTA has changed
```sh
curl http://192.168.1.188:8081/zeroconf/info -XPOST --data '{"deviceid":"9999aa8888","data":{} }'
```
Get firmware checksum and upload it to the device
```sh
shasum -a 256 tasmota-lite.bin
# aa413f499233901f74502558561a1304e2a3d7ab72259740b377f52775adbb5d  tasmota-lite.bin
curl http://192.168.1.188:8081/zeroconf/ota_flash -XPOST --data '{"deviceid":"9999aa8888","data":{
     "downloadUrl": "http://192.168.1.20/tasmota-lite.bin", 
     "sha256sum": "aa413f499233901f74502558561a1304e2a3d7ab72259740b377f52775adbb5d"} }'
# 192.168.1.20 is the HTTP server where tasmota-lite.bin file is stored
```
