@echo off
.\build\client\client.exe -network_camera_url=http://10.201.64.11:4747/video ^
 -rpc_server_addr=localhost:50051 ^
 -model_path=D:/cmake-install-modules/opencv-4.10.0/etc/haarcascades/haarcascade_frontalface_alt2.xml
