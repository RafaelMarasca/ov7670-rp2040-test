# ov7670-rp2040-test
This repository implements a ov7670 camera module library for rp2040.

##Usage guide

>⚠️ **To use the lirary it's necessary to have the pico sdk previously installed and configured.**

For a project structured as depicted below, the following steps may be followed to make correct use of this library::

```
.
└── My_Project/
    ├── lib/
    │   ├── CMakeLists.txt  
    │   └── ov7670/
    ├── src/
    │   ├── CMakeLists.txt
    │   └── main.c
    │
    ├── pico_sdk_import.cmake
    └── CMakeFiles.txt
```


* Clone this repository into a folder called ov7670 and place it under your lib folder

* Add the following piece of code into the lib folder's CMakeLists.txt:

```
add_subdirectory(ov7670)

```

* Place the following piece of code into the My_project's CMakeLists.txt:

```
include(pico_sdk_import.cmake)

project(test)

pico_sdk_init()

add_subdirectory(lib)
```

* Include "ov7670.h" in main.c 

* Last but not least, include libov7670 in the src's CMakeLists.txt:

```
target_link_libraries(<target> PRIVATE libov7670 [<other_libraries>])
```
