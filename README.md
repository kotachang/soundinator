# Soundinator

BEHOLD.... THE SOUNDINATOR

A basic implementation of a BLE speaker to gain a deeper understanding of BLE concepts

![image](https://github.com/user-attachments/assets/b3a942c7-d6fa-4635-9fce-437d58e84522)

## Materials
- ESP32 WROOM (comes with BLE and Wifi capabilities)
- MAX98357A amplifier
- 4/8 Ohm speaker that meets power requirements of the amplifier
- Breadboard
- Wires

## Implementation Steps
1. Base BLE server as a FreeRTOS task implemented on the ESP32. Able to read and write from an existing BLE scanner app
2. ESP32 Interface with the amplifier. Structure communication between the BLE task and the audio interface task. Play a note based on user input from the BLE scanner * **we are here** *
3. Send chunks of raw audio (.wav) from the phone and play from the ESP32 speaker
4. Implement a custom BLE client app on the phone to send any audio recording to the ESP32

## Build and Flash Instructions for ESP32
Following are the instructions for when you FIRST build the project. After building once, only steps 8-10 are required. Note that the very first build process can take a while. 

1. VSdCode Command Pallete (`Ctrl+Shift+P` on Windows) -> `ESP-IDF: Open Project Configuration`. Create a new profile with name of your choice.
2. Under 'Build Directory path', put [directory-to-the-repo]\soundinator\ESP32\build
3. Under 'sdkconfig file path', put [directory-to-the-repo]\soundinator\ESP32\
4. Under 'OpenOCD' 'Debug Level', put INFO
5. Command Pallete -> `ESP-IDF: Select project configuration`. Select the configuration you just made.
6. Command Pallete -> `ESP-IDF: Open ESP-IDF Terminal`. This should open a terminal on the bottom of VS Code. Go to [directory-to-the-repo]\soundinator\ESP32\ directory in the terminal.
7. Run `idf.py save-defconfig`
8. Run `idf.py build`
9. Run `idf.py flash` (It should automatically detect the ESP32, but you can also specify the port with -p)
10. Command Pallete -> `ESP-IDF: Monitor Device` to read logs from ESP32


<!-- | Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |

# Hello World Example

Starts a FreeRTOS task to print "Hello World".

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

Follow detailed instructions provided specifically for this example.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)


## Example folder contents

The project **hello_world** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── pytest_hello_world.py      Python script used for automated testing
├── main
│   ├── CMakeLists.txt
│   └── hello_world_main.c
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Technical support and feedback

Please use the following feedback channels:

* For technical queries, go to the [esp32.com](https://esp32.com/) forum
* For a feature request or bug report, create a [GitHub issue](https://github.com/espressif/esp-idf/issues)

We will get back to you as soon as possible. -->
