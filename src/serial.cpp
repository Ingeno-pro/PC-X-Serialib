#include "serial.hpp"


Serial::Serial(const char *port, unsigned int speed, char byte_size, char stopbits, char parity){
	
	#if defined(__linux__)
		this->serial_port = open(port, O_RDWR);
		if (serial_port < 0) {
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		//Get the current configuration
		if (tcgetattr(serial_port, &(this->tty)) != 0) {
			std::cerr << "Error : unable to get serial communication settings" << std::endl;
		}
		
		//Set input / output speed
		cfsetispeed(&(this->tty), speed); 
		cfsetospeed(&(this->tty), speed); 
		
		//Serial port configuration
		this->tty.c_cflag = (this->tty.c_cflag & ~CSIZE) | CS8; 
		this->tty.c_cflag |= (CLOCAL | CREAD);
		
		//Done the configuration by apply it to the serial port
		tcsetattr(serial_port, TCSANOW, &(this->tty));
	#elif defined(_WIN32)
	
		this->hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (this->hSerial == INVALID_HANDLE_VALUE) {
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		this->dcbSerialParams = {0};
		this->dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
		
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
		
		//Clear the serial port
		PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
		
	#endif
	
}
char Serial::uread(){
	
	char buffer[1];
	
	#if defined(__linux__)
		int num_bytes = read(serial_port, buffer, sizeof(buffer));
        if (num_bytes > 0) {
            return buffer[0];
        }
	#elif defined(_WIN32)
		if (ReadFile(this->hSerial, buffer, sizeof(buffer), &(this->bytesRead), NULL)) {
			return buffer[0];
		} else {
			std::cerr << "Error : Bad reading :(" << std::endl;
		}
	#endif
	return '\0';
}
//Not tested funtion
void Serial::uwrite(char byte){
	#if defined(__linux__)
		
	#elif defined(_WIN32)
		if (this->hSerial == INVALID_HANDLE_VALUE) {
            std::cerr << "Error : invalid port" << std::endl;
            return;
        }
		
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