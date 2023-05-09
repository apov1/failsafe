# Failsafe 
Prototype of an endpoint DLP agent for Linux.

## License
This project is licensed under the terms of the MIT license, with the exception of `KernFSHook` kernel module, which is licensed under the terms of GPL-3.0 license.

## Disclaimer
**RUN WITH CAUTION AND ONLY IN VIRTUAL MACHINE ENVIROMENT.**

The agent is just a prototype and therefore may not be stable. It runs with superuser privileges and also loads a custom kernel module.

**I do not take any liability if the agent causes any damage to the workstation hardware, operating system or any data.**

Tested on Ubuntu Linux 22.04 Jammy Jellyfish.

## External Dependencies
- `libnl-3`, `libnl-genl3` - Netlink communication (https://www.infradead.org/~tgr/libnl/)
- `nlohmann json` - JSON parsing (present in repository `Failsafe/ThirdParty/nlohmann/`, https://github.com/nlohmann/json)
- `cxxopts` - command line option parsing (present in repository `Failsafe/ThirdParty/cxxopts/`, https://github.com/jarro2783/cxxopts)
- `spdlog` - debug logging (https://github.com/gabime/spdlog)
- `SQLiteCpp` - SQLite wrapper (https://github.com/SRombauts/SQLiteCpp)
- `Catch2` - unit tests (https://github.com/catchorg/Catch2)

## How to compile
To build the project, you need CMake at least version 3.1

- Create `build` directory
- Run `cmake ..` from the `build` directory to generate build system
- Run `make` to compile the project

- To execute unit tests run `ctest`

## How to run
To run the failsafe compile it first according to instructions above. The `failsafe` binary will be present in `build/Failsafe/` directory. 

The binary takes the following arguments:
- `<1st positional argument>` -- path to FSHook kernel module to be loaded (after build located in `build/Libraries/FSHook/KernFSHook/kern_fshook.ko`)
- `--policy <path>` -- path to JSON file with DLP policies
- `--incidents <path>` -- path to database file for incident logging (will be created if it doesn't exist)

It needs to be ran with superuser privileges. 

An example how to run the binary from `build` set as working directory:
```
sudo ./Failsafe/failsafe ./Libraries/FSHook/KernFSHook/kern_fshook.ko --policy ../Failsafe/ExampleRuleFile.json --incidents ./incidents.db
```
The command will use compiled kernel module, use the example DLP policy file and create incident database called `incidents.db` in the `build/` directory.

## Project structure
* `Failsafe` - contains the source code of the Failsafe
* `Libraries` - contains the source code of associated libraries
    - `FSHook` 
        * `FSHook` - source code of FSHook library
        * `KernFSHook` - source code of FSHook kernel module
    - `USBControl` - source code of USBControl library
* `TestData` - example DLP policies for test scenarios
* `Tests` - scripts for performance testing
* `CMakeLists.txt` - CMake build system specification
* `RuleSchema.json` - JSON schema for DLP policies
