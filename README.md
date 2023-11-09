# fsae-2024
The control-box firmware of the NCKU formula

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

This firmware uses [Segger RTT](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/) library. If pyocd encounters a `Control Block` error, update the `RTT_ADDR` in makefile to match the `_SEGGER_RTT` address in `build/fsae-2024.map`.

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
