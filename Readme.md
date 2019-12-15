# Project Title

Simple FIX protocol parser.

### Description

To be done

### Important information

*Project status - in development.* 

CLI is not fully functioning. FIX parser supports only a few messages and is not fully working.   

### Prerequisites

* CMake build system (>= 3.7) is used to compile the project. Compiling tested with gcc6.3.0.
* Conan package manager is required to build the project.
 
* Compiler support for C++11 standard is required.
* boost (>= 1.70.0) is required to build the project.
* googletest (>= 1.8.1) is required to build the project.
* log4cxx (>=0.10.0) is required to build the project.

### Getting Started

* Download or clone the project's repository.
* Create build directory inside project folder and compile the sources. Building in source tree directory is forbidden.

```
    $ conan remote add jgsogo-conan-packages https://api.bintray.com/conan/jgsogo/conan-packages
    $ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
    $ mkdir .build && cd .build
    $ conan install ..
    $ cmake ..
    $ make
```
* Getting help:

```
    $ cd .build/bin
    $ ./fixparser_cli -h
    Usage: ./fixparser_cli [options]... 
    Options:
      -l [ --log-properties ] arg (=log4cxx.properties)
                                            File path with log4cxx settings.
      -d [ --data-file ] arg (=fix_data.txt)
                                            File path with fix protocol data.
      -s [ --size ] arg (=5)                The size of order book items to print.
      -h [ --help ]                         As it says.
```

### Usage

* Start the command line utility with the required arguments:

```
    $ cd .build/bin
    $ ./fixparser_cli --data-file test_data.txt
```

### Running the tests
  CMake build framework might be used to run available unit tests. Just run 'ctest' inside build directory after building the sources.

```
    $ ctest
    $ ctest --verbose
```
  *More tests to be done*

##$ Authors
Oleg Fedorov, fedorov.ftf@gmail.com

##$ License

To be done...