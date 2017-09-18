# A simple web server demo for CloudABI

In this repository you can find a relatively simple web server written
in C++. It is designed to run on [CloudABI](https://nuxi.nl/cloudabi/),
making use of [Flower](https://github.com/NuxiNL/flower) for networking.

## Building

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-x86_64-cloudabi.cmake ..
make
```

## Running

```bash
flower_switchboard /tmp/switchboard &
cloudabi-run cloudabi-demo-webserver << EOF &
%TAG ! tag:nuxi.nl,2015:cloudabi/
---
http_switchboard: !flower_switchboard_handle
    switchboard_path: /tmp/switchboard
    constraints:
        rights: [SERVER_START]
        in_labels:
            prog: webserver
logger_output: !fd stdout
html_response: <marquee>It works!</marquee>
EOF
sudo flower_ingress_accept 0.0.0.0:80 /tmp/switchboard '{"prog": "webserver"}' &
```
