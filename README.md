# fsae-2024
The control-box firmware of the NCKU formula

## Project's File Structure

```
.
├── Core
│   ├── ...
│   ├── Inc             # Implementation of the core functions
│   ├── Src             # Source code files
│   └── SEGGER_RTT      # The Segger RTT library source code and header files
├── Drivers
│   ├── ...
│   └── Sensors         # [*] All sensors driver with interface and BSP code
│       └── LDPS        #  └── Driver for Linear DisPlacment Sensor
├── FATFS
│   ├── App
│   └── Target
└── Middlewares
    └── Third_Party
```

## Requirements

- [make](https://www.gnu.org/software/make/)
- [cmake](https://cmake.org/)
- [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [ninja](https://ninja-build.org/)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- [pyocd](https://pyocd.io/)

## Build

```
$ make
```

The compiled binary file will be `build/fsae-2024.bin` or `build/fsae-2024.elf`

## Debug

```
$ pyocd gdbserver -t stm32f07vgtx
```

## Flash

```
$ make flash
```

## Monitor

This firmware use [Segger RTT](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/) library with default CB (control block) address `0x20000000`.

```
$ make monitor
```

## License

Not yet
