# Summary
* [Build Instructions](#Build-Instructions)
    * [Windows](#Windows)
    * [Linux](#Linux)
* [Running](#Running)
# Build Instructions
## Windows
### Prerequisite
* Visual Studio 2017
* premake5

### Building
1. Install premake5 and generate for vs2017
2. Build the visual studio solution

## Linux
### Prerequisite
* gcc or clang
* premake5
* SFML 

### Building
1. Generate the makefiles by running **premake5 gmake**
2. execute **make** to build all targets

# Running
In the **scripts** you can find scripts that demonstrate how to run the server and connect clients to it.