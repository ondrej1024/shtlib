# SHTlib
#### C library for reading the family of SHT sensors

### About

SHTlib is a C library that can be used to read the Sensirion SHT temperature and humidity sensors on Single Board Computers running Linux 
(e.g. RaspberryPi). The program using this library typically runs in user space. This library does not use the SHT sensor specific kernel drivers [SHT21](http://cateee.net/lkddb/web-lkddb/SENSORS_SHT21.html) or [SHT15](http://cateee.net/lkddb/web-lkddb/SENSORS_SHT15.html) but implements the communication protocol itself.  

### Features

- Support for SHT21 sensor
- Communication mode: simulated I2C over GPIO
- Multiple sensors support via separate GPIO pins
- Provided as C library to be included in your own project
- Example code for library usage provided  

### Coming soon
- Communication mode: native I2C

### Nice to have
- Replace RPi specific GPIO handling with Linux sysfs interface
- Support for SHT7x sensors

### Credits

The implementation of this library is based on the excellent code from Martin Steppuhn  
http://emsystech.de/raspi-sht21/

### Installation

Get source code from Github:

    git clone https://github.com/ondrej1024/shtlib.git

Build library:

    cd shtlib
    make

Install library:

    sudo make install

### Usage

You find an example application using the library in example/shtsensor.c

Modify the default SDA and SCL pins in the example program source code if needed, then build the prpogram (library needs to be built and installed first):  

    cd shtlib/example
    make

Run example program:  

    ./shtsensor

### Sensor wiring

The sensor chips SDA and SCL lines can be wired to any available GPIO pins. Please add 10K pullups to these pins.