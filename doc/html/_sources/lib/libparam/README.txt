# Gomspace Universal Parameter System (GUPS)
GUPS is a light-weight parameter system designed for gomspace satellite subsystems. It is based around a logical memory architecture, where every parameter is referenced directly by its logical address. A backend system takes care of translating addresses into physical addresses. The features of this system includes: 

- Direct memory access for quick parameter reads
- Simple datatypes: uint, int, float, double, string and hex
- Arrays of data
- Simple static parameter table and setup
- Multiple backends for non-volatile storage:
    - FRAM
    - FILE
    - FLASH (not yet finished)
- Scratch memory and running memory configurations
- Remote client with support for, GET, SET queries and COPY command
- Simple data-set serialization and deserialization
- Supports both little and big-endian systems
- Gomspace Shell (GOSH) commands for both local and remote access
- Built-in CSP parameter server
- Compile-time configuration of parameter system