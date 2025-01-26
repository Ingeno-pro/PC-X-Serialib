#include "serial.hpp"

int main(){
		
	std::cout << "Hello world";
	Serial s("COM6", 9600, 8, 1, 0);
	
	while(1){
		std::cout << s.uread();
		s.uwrite('c');
	}
}