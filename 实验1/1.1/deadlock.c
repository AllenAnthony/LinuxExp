#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutexNorthF = PTHREAD_MUTEX_INITIALIZER;//provide the first position in north with a mutexlock
pthread_mutex_t mutexEastF = PTHREAD_MUTEX_INITIALIZER;//provide the first position in east with a mutexlock
pthread_mutex_t mutexSouthF = PTHREAD_MUTEX_INITIALIZER;//provide the first position in south with a mutexlock
pthread_mutex_t mutexWestF = PTHREAD_MUTEX_INITIALIZER;//provide the first position in west with a mutexlock

pthread_mutex_t mutexNorthQ = PTHREAD_MUTEX_INITIALIZER;//provide northern release signal with a mutexlock
pthread_mutex_t mutexEastQ = PTHREAD_MUTEX_INITIALIZER;//provide eastern release signal with a mutexlock
pthread_mutex_t mutexSouthQ = PTHREAD_MUTEX_INITIALIZER;//provide southern release signal with a mutexlock
pthread_mutex_t mutexWestQ = PTHREAD_MUTEX_INITIALIZER;//provide western release signal with a mutexlock

pthread_mutex_t mutexWait = PTHREAD_MUTEX_INITIALIZER;//provide the total number of waiting cars with a mutexlock(one at most in every intersection)
pthread_mutex_t mutexPass = PTHREAD_MUTEX_INITIALIZER;//provide the total number of passed cars with a mutexlock

int sumN, sumE, sumS, sumW, sum;//record the total number of cars in all direction 
int nextN, nextE, nextS, nextW; //indicate the next car in four intersections
int pass, wait;//the number of cars have passed and waiting in four intersections

pthread_cond_t firstNorth = PTHREAD_COND_INITIALIZER;//the first position in north
pthread_cond_t firstEast = PTHREAD_COND_INITIALIZER;//the first position in east
pthread_cond_t firstSouth = PTHREAD_COND_INITIALIZER;//the first position in south
pthread_cond_t firstWest = PTHREAD_COND_INITIALIZER;//the first position in west

pthread_cond_t queueNorth = PTHREAD_COND_INITIALIZER;//northern release signal
pthread_cond_t queueEast = PTHREAD_COND_INITIALIZER;//eastern release signal
pthread_cond_t queueSouth = PTHREAD_COND_INITIALIZER;//southern release signal
pthread_cond_t queueWest = PTHREAD_COND_INITIALIZER;//western release signal

void *threadNorth(void* dir_number)
{
	int dir_num = *((int*)dir_number);//the car number in the northern queue
	char* direction = "North";

	while (1)
	{
		pthread_mutex_lock(&mutexNorthF);
		if (dir_num != 1)//if the car is the first arriver from this direction, then occupy the first position directly, otherwise wait
		{
			pthread_cond_wait(&firstNorth, &mutexNorthF);
		}

		if (dir_num == (nextN+1))//if the car is the frontest car waiting in the direction
		{
			pthread_mutex_unlock(&mutexNorthF);
			printf("the car %d from %s arrives at intersection\n", dir_num, direction);//wait here
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			wait++;
			pthread_mutex_unlock(&mutexWait);
			//usleep(600);
		}
		else
		{
			pthread_mutex_unlock(&mutexNorthF);//if the car is not the frontest car, then give the position to another car
			pthread_cond_signal(&firstNorth);
			usleep(30);//to prevent the signal back and forth
			continue;
		}

		pthread_mutex_lock(&mutexWestF);
		if (nextW != sumW)// if there is no car left from the right hand side
		{
			pthread_mutex_unlock(&mutexWestF);
			pthread_mutex_lock(&mutexNorthQ);
			pthread_cond_wait(&queueNorth, &mutexNorthQ);//wait until the release signal arrive
			pthread_mutex_unlock(&mutexNorthQ);
		}
		else
			pthread_mutex_unlock(&mutexWestF);

		//let the car leave
		printf("the car %d from %s have left, which is the %d car\n", dir_num, direction,pass+1);
		fflush(stdout);

		pthread_mutex_lock(&mutexNorthF);
		nextN++;//indicate the next car
		pthread_mutex_unlock(&mutexNorthF);


		pthread_mutex_lock(&mutexWait);
		wait--;//the number of waiting car minus one
		pthread_mutex_unlock(&mutexWait);

		pthread_mutex_lock(&mutexPass);
		pass++;//the number of passed car plus one
		pthread_mutex_unlock(&mutexPass);

		//usleep(100);
		pthread_cond_signal(&queueEast);//release the car at left hand side
		usleep(100);//prevent starvation
		pthread_cond_signal(&firstNorth);//release the next car from the direction
		break;
	}
	pthread_exit(NULL);
}
void *threadEast(void* dir_number)
{
	int dir_num = *((int*)dir_number);//the car number in the Eastern queue
	char* direction = "East";

	while (1)
	{
		pthread_mutex_lock(&mutexEastF);
		if (dir_num != 1)//if the car is the first arriver from this direction, then occupy the first position directly, otherwise wait
		{
			pthread_cond_wait(&firstEast, &mutexEastF);
		}

		if (dir_num == (nextE+1))//if the car is the frontest car waiting in the direction
		{
			pthread_mutex_unlock(&mutexEastF);
			printf("the car %d from %s arrives at intersection\n", dir_num, direction);//wait here
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			wait++;
			pthread_mutex_unlock(&mutexWait);
			//usleep(600);
		}
		else
		{
			pthread_mutex_unlock(&mutexEastF);//if the car is not the frontest car, then give the position to another car
			pthread_cond_signal(&firstEast);
			usleep(30);//to prevent the signal back and forth
			continue;
		}

		pthread_mutex_lock(&mutexNorthF);
		if (nextN != sumN)// if there is no car left from the right hand side
		{
			pthread_mutex_unlock(&mutexNorthF);
			pthread_mutex_lock(&mutexEastQ);
			pthread_cond_wait(&queueEast, &mutexEastQ);//wait until the release signal arrive
			pthread_mutex_unlock(&mutexEastQ);
		}
		else
			pthread_mutex_unlock(&mutexNorthF);

		//let the car leave
		printf("the car %d from %s have left, which is the %d car\n", dir_num, direction,pass+1);
		fflush(stdout);

		pthread_mutex_lock(&mutexEastF);
		nextE++;//indicate the next car
		pthread_mutex_unlock(&mutexEastF);


		pthread_mutex_lock(&mutexWait);
		wait--;//the number of waiting car minus one
		pthread_mutex_unlock(&mutexWait);

		pthread_mutex_lock(&mutexPass);
		pass++;//the number of passed car plus one
		pthread_mutex_unlock(&mutexPass);

		//usleep(100);
		pthread_cond_signal(&queueSouth);//release the car at left hand side
		usleep(100);//prevent starvation
		pthread_cond_signal(&firstEast);//release the next car from the direction
		break;
	}
	pthread_exit(NULL);
}

void *threadSouth(void* dir_number)
{
	int dir_num = *((int*)dir_number);//the car number in the Southern queue
	char* direction = "South";

	while (1)
	{
		pthread_mutex_lock(&mutexSouthF);
		if (dir_num != 1)//if the car is the first arriver from this direction, then occupy the first position directly, otherwise wait
		{
			pthread_cond_wait(&firstSouth, &mutexSouthF);
		}

		if (dir_num == (nextS+1))//if the car is the frontest car waiting in the direction
		{
			pthread_mutex_unlock(&mutexSouthF);
			printf("the car %d from %s arrives at intersection\n", dir_num, direction);//wait here
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			wait++;
			pthread_mutex_unlock(&mutexWait);
			//usleep(600);
		}
		else
		{
			pthread_mutex_unlock(&mutexSouthF);//if the car is not the frontest car, then give the position to another car
			pthread_cond_signal(&firstSouth);
			usleep(30);//to prevent the signal back and forth
			continue;
		}

		pthread_mutex_lock(&mutexEastF);
		if (nextE != sumE)// if there is no car left from the right hand side
		{
			pthread_mutex_unlock(&mutexEastF);
			pthread_mutex_lock(&mutexSouthQ);
			pthread_cond_wait(&queueSouth, &mutexSouthQ);//wait until the release signal arrive
			pthread_mutex_unlock(&mutexSouthQ);
		}
		else
			pthread_mutex_unlock(&mutexEastF);

		//let the car leave
		printf("the car %d from %s have left, which is the %d car\n", dir_num, direction,pass+1);
		fflush(stdout);

		pthread_mutex_lock(&mutexSouthF);
		nextS++;//indicate the next car
		pthread_mutex_unlock(&mutexSouthF);


		pthread_mutex_lock(&mutexWait);
		wait--;//the number of waiting car minus one
		pthread_mutex_unlock(&mutexWait);

		pthread_mutex_lock(&mutexPass);
		pass++;//the number of passed car plus one
		pthread_mutex_unlock(&mutexPass);

		//usleep(100);
		pthread_cond_signal(&queueWest);//release the car at left hand side
		usleep(100);//prevent starvation
		pthread_cond_signal(&firstSouth);//release the next car from the direction
		break;
	}
	pthread_exit(NULL);
}

void *threadWest(void* dir_number)
{
	int dir_num = *((int*)dir_number);//the car number in the Western queue
	char* direction = "West";

	while (1)
	{
		pthread_mutex_lock(&mutexWestF);
		if (dir_num != 1)//if the car is the first arriver from this direction, then occupy the first position directly, otherwise wait
		{
			pthread_cond_wait(&firstWest, &mutexWestF);
		}

		if (dir_num == (nextW+1))//if the car is the frontest car waiting in the direction
		{
			pthread_mutex_unlock(&mutexWestF);
			printf("the car %d from %s arrives at intersection\n", dir_num, direction);//wait here
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			wait++;
			pthread_mutex_unlock(&mutexWait);
			//usleep(600);
		}
		else
		{
			pthread_mutex_unlock(&mutexWestF);//if the car is not the frontest car, then give the position to another car
			pthread_cond_signal(&firstWest);
			usleep(30);//to prevent the signal back and forth
			continue;
		}

		pthread_mutex_lock(&mutexSouthF);
		if (nextS != sumS)// if there is no car left from the right hand side
		{
			pthread_mutex_unlock(&mutexSouthF);
			pthread_mutex_lock(&mutexWestQ);
			pthread_cond_wait(&queueWest, &mutexWestQ);//wait until the release signal arrive
			pthread_mutex_unlock(&mutexWestQ);
		}
		else
			pthread_mutex_unlock(&mutexSouthF);

		//let the car leave
		printf("the car %d from %s have left, which is the %d car\n", dir_num, direction,pass+1);
		fflush(stdout);

		pthread_mutex_lock(&mutexWestF);
		nextW++;//indicate the next car
		pthread_mutex_unlock(&mutexWestF);


		pthread_mutex_lock(&mutexWait);
		wait--;//the number of waiting car minus one
		pthread_mutex_unlock(&mutexWait);

		pthread_mutex_lock(&mutexPass);
		pass++;//the number of passed car plus one
		pthread_mutex_unlock(&mutexPass);

		//usleep(100);
		pthread_cond_signal(&queueNorth);//release the car at left hand side
		usleep(100);//prevent starvation
		pthread_cond_signal(&firstWest);//release the next car from the direction
		break;
	}
	pthread_exit(NULL);
}

void* threadDeadlock(void* junk)
{
	pthread_mutex_lock(&mutexPass);
	while (pass<sum)//to determine whether there is still a car to go through 
	{
		pthread_mutex_unlock(&mutexPass);
		pthread_mutex_lock(&mutexWait);
		if (wait == 4)//deadlock happen
		{
			printf("DEADLOCK FOUND,the car from north go first\n");
			pthread_cond_signal(&queueNorth);//the northern car go first
		}
		pthread_mutex_unlock(&mutexWait);
		usleep(1000);//wait 0.1s
		pthread_mutex_lock(&mutexPass);

	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	char* input;//the input
	pthread_t *threadArray;//the array of thread id
	pthread_t DL;//the deadlock judgement thread

	int* dir_num;//the serial number of the car in it's direction


	input = (char*)malloc(31 * sizeof(char));
	scanf("%30s", input);
	sum = strlen(input);

	dir_num = (int*)malloc(sum*sizeof(int));
	threadArray = (pthread_t*)malloc(sum*sizeof(pthread_t));

	pass = wait = nextW = nextS = nextE = nextN = 0;
	sumN = sumE = sumS = sumW = 0;
	for (int cou = 0; cou<sum; cou++)
	{
		switch (input[cou])
		{
		case 'n':case 'N':
			sumN++;
			dir_num[cou] = sumN;
			break;                                                                                                                                                                                                                                                                                                                     
		case 'e':case 'E':
			sumE++;
			dir_num[cou] = sumE;
			break;
		case 's':case 'S':
			sumS++;
			dir_num[cou] = sumS;
			break;
		case 'w':case 'W':
			sumW++;
			dir_num[cou] = sumW;
			break;
		}
	}

	pthread_create(&DL, NULL, threadDeadlock, NULL);

	for (int i = 0; i < sum; i++)
	{
		switch (input[i])
		{
		case 'n':case 'N':
			pthread_create(&threadArray[i], NULL, threadNorth, (void*)&dir_num[i]);
			break;
		case 'e':case 'E':
			pthread_create(&threadArray[i], NULL, threadEast, (void*)&dir_num[i]);
			break;
		case 's':case 'S':
			pthread_create(&threadArray[i], NULL, threadSouth, (void*)&dir_num[i]);
			break;
		case 'w':case 'W':
			pthread_create(&threadArray[i], NULL, threadWest, (void*)&dir_num[i]);
			break;
		}
	}

	pthread_join(DL, NULL);
	return 0;
}