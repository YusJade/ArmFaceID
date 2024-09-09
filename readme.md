# ArmFaceID

This cpp project is aiming to impl a system of face detection and face recognition via opencv, seetaface2, using Qt6 as GUI tool.

## How to build this project

The project contains the following parts:

- proto: define the protobuf\`s message and gRPC \`s service, this file will be complied by special generator to `.cc` and `.h`, which be invoked by other\`s parts.
- server: provides rpc services, impls such as face register, recognition and persistence of data.
- client: provides face detection, use rpc services provided by server.
- model: nothing but put some models\` file loaded by opencv\`s classifier and seetaface\`s engine.

run the following commands to build:

```bash
cmake -B ./build
cmake --build ./build/
```
