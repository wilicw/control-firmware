# fsae-2024
The control-box firmware of the NCKU formula

## Requirements

- [make](https://www.gnu.org/software/make/)
- [cmake](https://cmake.org/)
- [AArch32 bare-metal target (arm-none-eabi)](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
- [ninja](https://ninja-build.org/)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)

## Build

```
$ make
```

The compiled binary file will be `build/fsae-2024.bin` or `build/fsae-2024.elf`

## Debug

In order to debug this firmware you need to attach the probe to the board first. The debug config file will be `openocd_*.cfg` dependents on the type of the debug probe.

For the Daplink

```
$ openocd -f $(pwd)/openocd_daplink.cfg
```

## Flash

Change the programmer type in `Makefile` and run 

```
$ make flash
```

Or specify the programmer/debugger type in command

```
$ make flash DEBUGGER=stlink
```

## License

Not yet
