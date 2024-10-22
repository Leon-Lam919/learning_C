// creating calculator

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
	
	int string = 4;
	float answer;
	char *endptr;
	float num_1 = strtof(argv[1], &endptr);
	float num_2 = strtof(argv[2], &endptr);
	

	switch(string){
	case 1:
		answer = num_1 + num_2;
		break;
	case 2: 
		answer = num_1 - num_2;
		break;
	case 3: 
		answer = num_1 * num_2;
		break;
	case 4:
		answer = num_1 / num_2;
		break;
	}

	printf("Answer is %f \n", answer);
	return 0;
} 
