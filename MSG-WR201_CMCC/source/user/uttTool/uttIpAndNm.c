#include <stdio.h>                                                                                                                   
#include <arpa/inet.h>                                                                                                               

int main(int argc, char **argv)                                                                                                      
{                                                                                                                                    
	if (argc < 3) {                                                                                                                  
		return -1;                                                                                                                   
	}                                                                                                                                

	struct in_addr addr;                                                                                                             
	int i, len;                                                                                                                      
	unsigned int ipint = ntohl(inet_addr(argv[1]));                                                                                  
	unsigned int nmint = ntohl(inet_addr(argv[2]));                                                                                  
	unsigned int maskint = 0u;                                                                                                       
	unsigned int bit = 0u;                                                                                                           
	len = sizeof(unsigned int)*8;                                                                                                    
	for (i = 0; i < len; i++) {                                                                                                      
		bit = 0x1u << (len - 1 - i);                                                                                                 
		if (nmint & bit) {                                                                                                           
			maskint |= bit;                                                                                                          
		} else {                                                                                                                     
			break;                                                                                                                   
		}                                                                                                                            
	}                                                                                                                                
	addr.s_addr = htonl(ipint & maskint);                                                                                            
	printf("%s/%u", inet_ntoa(addr), i);                                                                                             
	return 1;  
}
