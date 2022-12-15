/**********************************************************/
/**********************************************************/
/*     Final Project  : Concurrent Programming            */
/*     Author         : Kiran Jojare                      */
/*     This is the main function for this project.        */
/**********************************************************/
/**********************************************************/

/* Includes */
#include"main.hpp"

/* Global Data */

/* Main Function */
int main(int argc, char *argv[])
{
	
	/* Wrong Data Parsing */
	if(argc != 2 && argc !=6){
		printf("Incorrect Input Provided From Command Line\nCorrect Usage: ./finalproject [--name] [-t NUM_THREADS] [-i NUM_ITERATIONS] [--DS=<treiber,MandS_Queue,SGL_Stack,SGL_Queue>]\n\n");
		return 1;
	}
	
    /* Parsing Command Line arguments */
    int c                   = 0;
	int option_index        = 0;
	int thread_cnt_recieved = 0;
	
	size_t thread_count;
	int iterations;
	string concurrent_DS_Selected;  /* String to recieve concurrent 
	data structure to be parsed from command line arg */
	
	/* Long Option Structure */
	static struct option long_options[] ={
	{"name"       ,0,0,'n'},        // long option 1 for name
	{"DS"         ,1,0,'c'},        // long option 2 for data structure selected
	{"thread"     ,1,0,'t'},        // long option 3 for thread count
	{"iterations" ,1,0,'i'},        // long option 4 for no of iterations
	{0,0,0,0},
	};
    
	/* Fetch Command Line Argument Using Switch Case */
	while((c = getopt_long(argc,argv, "nc:t:i:", long_options,&option_index)) != -1)
	{
		switch (c)
		{
		case 'n':
			printf("Student name : Kiran Narendra Jojare\n");
			exit(1); //print name and exit code
			break;
		case 'c':
			concurrent_DS_Selected = optarg;
			//printf("Concurrent data structure selected : %s\n",concurrent_DS_Selected);
			cout << "Concurrent data structure Selected : "<< concurrent_DS_Selected << "\n";
			break;
		case 'i':
			iterations = atoi(optarg);
			printf("No of iterations selected : %d\n",iterations);
			if(iterations>10000){iterations=10000;}

			break;
		case 't':
			thread_count = atoi(optarg);
			printf("No of threads selected : %ld\n",thread_count);
			thread_cnt_recieved = 1;
			break;
		default :
			printf("Default \n");
			break;	
		}
	}
	
	/* Configure default thread counts */
    if (thread_cnt_recieved != 1 || thread_count > 150)
    {
		printf("Incorrect thread count selected.\nMaximum Thread Count Allowed is 20.\nUsing Default Thread Count of 4\n");
        thread_count = 4;
		exit(-1);
    }	
	/* Creating an object of class concurrent_DS_Tester called mytest
	 * Using default constructor */
	
	Concurrent_DS_Tester mytest(thread_count);
	mytest.test(iterations, thread_count, concurrent_DS_Selected);
	
return 0;
}