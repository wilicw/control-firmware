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

This firmware use [Segger RTT](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/) library. If pyocd have `Control Block` error, change the `RTT_ADDR` in makefile according to the `_SEGGER_RTT` address in the `build/fsae-2024.map`.

```
$ make monitor
```

or 

```
$ cat build/fsae-2024.map | grep _SEGGER_RTT
 .bss._SEGGER_RTT
                0x2000097c                _SEGGER_RTT

$ make monitor RTT_ADDR=0x2000097c
```

## License

Not yet
