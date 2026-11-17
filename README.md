# Tiva Bluetooth IoT

Bare-metal smart-home firmware for a Texas Instruments Tiva C TM4C123GH6PM
microcontroller. The project is designed to communicate with a Bluetooth
module over UART, control household loads through relays, and report door and
temperature information.

## What the project does

The firmware provides the building blocks for a Bluetooth-controlled home
controller:

- Controls a lamp and a plug through relay outputs.
- Receives newline-terminated commands from a Bluetooth module.
- Sends acknowledgements and status updates back over Bluetooth.
- Tracks door state and reports `DOOR:OPEN` or `DOOR:CLOSED`.
- Tracks temperature and reports it as `TEMP:XX.X`.
- Activates a temperature alarm when the measured temperature exceeds a
  configurable limit.

The current `main.c` is a small GPIO demonstration that turns the blue
on-board LED on while the PF0 switch is pressed. The Bluetooth protocol and
peripheral modules are implemented as separate components, but they still need
to be initialized and connected from the application loop before the complete
smart-home behavior runs on hardware.

## Repository structure

```text
.
├── firmware/
│   ├── CMakeLists.txt          # Cross-compilation and firmware build rules
│   ├── inc/                    # Public headers and TM4C123 register definitions
│   │   ├── board_pins.h        # Board pin mapping
│   │   ├── door_sensor.h       # Door sensor interface
│   │   ├── protocol.h          # Home state and Bluetooth protocol interface
│   │   ├── relay.h             # Lamp and plug relay interface
│   │   ├── temp_sensor.h       # Temperature sensor interface
│   │   ├── timer.h             # Timer interface
│   │   ├── tm4c123gh6pm.h      # MCU peripheral register definitions
│   │   └── uart_bt.h           # Bluetooth UART interface
│   ├── src/                    # C application and peripheral implementations
│   │   ├── main.c              # Current GPIO demonstration entry point
│   │   ├── protocol.c          # Command parsing and home-state management
│   │   ├── uart_bt.c           # UART1 Bluetooth transport
│   │   ├── relay.c             # Relay driver implementation
│   │   ├── door_sensor.c       # Door sensor driver implementation
│   │   ├── temp_sensor.c       # Temperature sensor driver implementation
│   │   └── timer.c             # Timer driver implementation
│   └── startup/
│       ├── startup.c           # Reset handler and interrupt vector table
│       └── tm4c123gh6pm.ld     # TM4C123 linker script
└── .gitignore
```

## Bluetooth protocol

Commands are ASCII lines terminated with `\n`:

| Command | Effect |
| --- | --- |
| `LAMP:1` / `LAMP:0` | Turn the lamp relay on/off |
| `PLUG:1` / `PLUG:0` | Turn the plug relay on/off |
| `LIMIT:<value>` | Set the temperature alarm limit |
| `STATUS?` | Send the complete current state |

UART1 uses PB0 (RX) and PB1 (TX), 9600 baud, 8 data bits, no parity, and one
stop bit. Responses include acknowledgements such as `ACK:LAMP:1` and status
lines such as `LAMP:1`, `TEMP:25.0`, and `ALARM:0`.

## Building

Install an ARM GNU toolchain and CMake, then configure and build from the
firmware directory:

```powershell
cd firmware
cmake -S . -B build -G Ninja
cmake --build build
```

The CMake configuration targets an ARM Cortex-M4 with hardware floating point.
It produces:

- `tiva_smart_home.elf` - debug-symbol firmware image
- `tiva_smart_home.bin` - raw binary suitable for a programmer
- `tiva_smart_home.map` - linker map file

The toolchain must provide `arm-none-eabi-gcc` and
`arm-none-eabi-objcopy`.

## Hardware target

The firmware targets the TM4C123GH6PM MCU and uses its memory-mapped
peripheral registers directly. The startup code initializes the data and BSS
sections, installs the interrupt vector table, and calls `main`; no operating
system or vendor HAL is used.
