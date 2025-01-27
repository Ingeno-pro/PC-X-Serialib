# Seriallib-X-PC library guide
## Information 
This library is provided for mingw / g++ compiler, this library can be use on Windows and Linux ( sorry MacOS ). 

## Install
In order to install this library, just put the source files in the source folder of your project

## Use 
As for use this library, you should import it by put `#include "serial.hpp"` at the begining your source file  

### Documentation  
The provided library is very easy to use. You have to first Create a `Serial`.  
You can find an example of usage in the examples folder.  
Here is some information about library's functions  
`char Serial::uread()` : Allow to read a character on a serial communication   
`void Serial::write(char byte)` : Send a byte on a serial communication   
`Serial::Serial(const char *port, unsigned int speed, char byte_size, char stopbits, char parity)` : Initialize the serial interface   
/!\ The port is "COMX" on windows and "/dev/ttyXXXX" on linux.
/!\ On linux, if the files corresponding to your serial ports can only be read in superuser mode, you have to launch your program with `sudo`

## To do 
-I have to add the possibility to set the byte size, the number of stop bit and the parity on linux  

## Changelog

27/01/2025 : Seriallib-X-PC  v0.9 release  
-Adding serial reading on linux 
-Adding serial writing on linux / windows
23/01/2025 : Seriallib-X-PC  v0.1 release  
-Adding serial reading on windows 

Please, if you have any suggest/question contact me on contact@ingeno.fr

