#include "serial.hpp"


Serial::Serial(const char *port, unsigned int speed, char byte_size, char stopbits, char parity){
	
	this->is_available = true;
	
	#if defined(__linux__)
		this->serial_port = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (this->serial_port < 0) {
			this->is_available = false;
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		//Get the current configuration
		if (tcgetattr(this->serial_port, &(this->tty)) != 0) {
			this->is_available = false;
			std::cerr << "Error : unable to get serial communication settings" << std::endl;
		}
		
		//Set input / output speed
		cfsetispeed(&(this->tty), speed); 
		cfsetospeed(&(this->tty), speed); 
		
		//Serial port configuration
		this->tty.c_cflag = (this->tty.c_cflag & ~CSIZE) | CS8; 
		this->tty.c_cflag |= (CLOCAL | CREAD);
		this->tty.c_cflag &= ~CRTSCTS;                   // Pas de contrôle matériel
		
		this->tty.c_iflag &= ~IGNBRK;    // Disable ignore break
        this->tty.c_iflag &= ~(IXON | IXOFF | IXANY); // No software flux control

        this->tty.c_lflag = 0; // No echo
        this->tty.c_oflag = 0; // No output processing

        // Timeout configuration
        this->tty.c_cc[VMIN] = 1;  // Minimum 1 char
        this->tty.c_cc[VTIME] = 5; // 0.5s Timeout 
		
		//Done the configuration by apply it to the serial port
		tcsetattr(this->serial_port, TCSANOW, &(this->tty));
	#elif defined(_WIN32)
	
		this->hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (this->hSerial == INVALID_HANDLE_VALUE) {
			this->is_available = false;
			std::cerr << "Error : can't open serial port" << std::endl;
		}
		
		this->dcbSerialParams = {0};
		this->dcbSerialParams.DCBlength = sizeof(this->dcbSerialParams);
		
		if (!GetCommState(this->hSerial, &(this->dcbSerialParams))) {
			this->is_available = false;
			std::cerr << "Error : unable to get serial communication settings" << std::endl;
		}
	
		//Serial configuration
		this->dcbSerialParams.BaudRate = speed;  
		this->dcbSerialParams.ByteSize = byte_size;      
		this->dcbSerialParams.StopBits = stopbits;
		this->dcbSerialParams.Parity = parity;
		
		//Done the configuration by apply it to the serial port
		if (!SetCommState(this->hSerial, &(this->dcbSerialParams))) {
			this->is_available = false;
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
		usleep(1000); //Sleep delay 
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
		usleep(1000); //Sleep delay 
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
bool Serial::get_availability(){
	return this->is_available;
}
//To do on linux
void getAvailableSerialPort(std::vector<std::string>& port_array){
	
	char port[10];
	port_array.clear();	
	#if defined(__linux__)
	#elif defined(_WIN32)
	for (int i = 1; i <= 256; i++) {  // Tester COM1 à COM256
        snprintf(port, sizeof(port), "COM%d", i);
        HANDLE hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hSerial != INVALID_HANDLE_VALUE) {
			port_array.push_back(port);
            CloseHandle(hSerial);
        }
    }
	#endif
	

}
