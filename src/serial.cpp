#include "serial.hpp"


Serial::Serial(const char *port, unsigned int speed, char byte_size, char stopbits, char parity){
	
	#if defined(__linux__)
		this->serial_port = open(port, O_RDWR);
		if (this->serial_port < 0) {
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		//Get the current configuration
		if (tcgetattr(this->serial_port, &(this->tty)) != 0) {
			std::cerr << "Error : unable to get serial communication settings" << std::endl;
		}
		
		//Set input / output speed
		cfsetispeed(&(this->tty), speed); 
		cfsetospeed(&(this->tty), speed); 
		
		//Serial port configuration
		this->tty.c_cflag = (this->tty.c_cflag & ~CSIZE) | CS8; 
		this->tty.c_cflag |= (CLOCAL | CREAD);
		
		tty.c_iflag &= ~IGNBRK;    // Disable ignore break
        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flux control

        tty.c_lflag = 0; // No echo
        tty.c_oflag = 0; // No output processing

        // Timeout configuration
        tty.c_cc[VMIN] = 1;  // Minimum 1 caractère
        tty.c_cc[VTIME] = 5; // Timeout de 0.5 seconde
		
		//Done the configuration by apply it to the serial port
		tcsetattr(this->serial_port, TCSANOW, &(this->tty));
	#elif defined(_WIN32)
	
		this->hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (this->hSerial == INVALID_HANDLE_VALUE) {
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		this->dcbSerialParams = {0};
		this->dcbSerialParams.DCBlength = sizeof(this->dcbSerialParams);
		
		if (!GetCommState(this->hSerial, &(this->dcbSerialParams))) {
			std::cerr << "Error : unable to get serial communication settings" << std::endl;
		}
	
		//Serial configuration
		this->dcbSerialParams.BaudRate = speed;  
		this->dcbSerialParams.ByteSize = byte_size;      
		this->dcbSerialParams.StopBits = stopbits;
		this->dcbSerialParams.Parity = parity;
		
		//Done the configuration by apply it to the serial port
		if (!SetCommState(this->hSerial, &(this->dcbSerialParams))) {
			std::cerr << "Error : unable to configure serial communication :(" << std::endl;
		}
		
		//Put a reading timeout
		COMMTIMEOUTS timeouts = {0};
		timeouts.ReadIntervalTimeout = 1; // Delay between two bytes
		timeouts.ReadTotalTimeoutConstant = 50; // Reading Timeout 
		timeouts.ReadTotalTimeoutMultiplier = 10; // ???
		SetCommTimeouts(this->hSerial, &timeouts);
		
		//Clear the serial port
		PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
		
	#endif
	
}
char Serial::uread(){
	
	char buffer[1];
	
	#if defined(__linux__)
		int num_bytes = read(this->serial_port, buffer, sizeof(buffer));
        if (num_bytes > 0) {
            return buffer[0];
        }
	#elif defined(_WIN32)
		//Read a Character and return it
		if (ReadFile(this->hSerial, buffer, sizeof(buffer), &(this->bytesRead), NULL)) {
			return buffer[0];
		} else {
			std::cerr << "Error : Bad reading :(" << std::endl;
		}
	#endif
	return '\0';
}
//Now do linux idiot !
void Serial::uwrite(char byte){
	#if defined(__linux__)
		//Check the port availability
		if (this->serial_port == -1) {
            std::cerr << "Error : invalid port" << std::endl;
            return;
        }
		//Write a character
        if (write(this->serial_port, &byte, sizeof(byte)) < 0) {
            std::cerr << "Error : Bad writting :(" << std::endl;
        }
	#elif defined(_WIN32)
		//Check the port availability
		if (this->hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error : invalid port" << std::endl;
            return;
        }
		//Write a character
		if (!WriteFile(this->hSerial, &byte, sizeof(byte), &(this->bytes_written), NULL)) {
            std::cerr << "Error : Bad writting :(" << std::endl;
        }
	#endif
}
Serial::~Serial(){
	
	#if defined(__linux__)
		close(serial_port);
	#elif defined(_WIN32)
		CloseHandle(this->hSerial);
	#endif
}