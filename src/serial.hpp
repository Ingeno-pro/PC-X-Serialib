#include <iostream>
#include <vector>

#if defined(__linux__)
	#include <fcntl.h>
	#include <termios.h>
	#include <unistd.h>
#elif defined(_WIN32)
	#include <windows.h>
#endif

#ifndef __SERIAL_HPP__
#define __SERIAL_HPP__

class Serial
{
	
	public:
		Serial(const char *port, unsigned int speed, char byte_size, char stopbits, char parity);
		~Serial();
		
		char uread();
		void uwrite(char byte);
		
		bool get_availability();
		
	private:
		bool is_available; 
	
		#if defined(__linux__)
			int serial_port;
			termios tty;
		#elif defined(_WIN32)
			HANDLE hSerial;
			DCB dcbSerialParams;
			DWORD bytesRead;
			DWORD bytes_written;
		#endif

		
	
};

void getAvailableSerialPort(std::vector<std::string>& port_array);


#endif