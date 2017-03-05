#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutexNorth = PTHREAD_MUTEX_INITIALIZER;//为北边第一辆车信号量提供互斥锁
pthread_mutex_t mutexEast = PTHREAD_MUTEX_INITIALIZER;//为东边第一辆车信号量提供互斥锁
pthread_mutex_t mutexSouth = PTHREAD_MUTEX_INITIALIZER;//为南边第一辆车信号量提供互斥锁
pthread_mutex_t mutexWest = PTHREAD_MUTEX_INITIALIZER;//为西边第一辆车信号量提供互斥锁

pthread_mutex_t mutexWait = PTHREAD_MUTEX_INITIALIZER;//为路口等待车辆数的变量提供保护
pthread_mutex_t mutexPassed = PTHREAD_MUTEX_INITIALIZER;//为已经过的车辆数变量提供保护

pthread_mutex_t mutexNorthQ = PTHREAD_MUTEX_INITIALIZER;//为北边放行的信号量提供互斥锁
pthread_mutex_t mutexEastQ = PTHREAD_MUTEX_INITIALIZER;//为东边放行的信号量提供互斥锁
pthread_mutex_t mutexSouthQ = PTHREAD_MUTEX_INITIALIZER;//为南边放行的信号量提供互斥锁
pthread_mutex_t mutexWestQ = PTHREAD_MUTEX_INITIALIZER;//为西边放行的信号量提供互斥锁

pthread_cond_t firstNorth = PTHREAD_COND_INITIALIZER;//北边第一辆车信号
pthread_cond_t firstEast = PTHREAD_COND_INITIALIZER;//东边第一辆车信号
pthread_cond_t firstSouth = PTHREAD_COND_INITIALIZER;//南边第一辆车信号
pthread_cond_t firstWest = PTHREAD_COND_INITIALIZER;//西边第一辆车信号

pthread_cond_t queueNorth = PTHREAD_COND_INITIALIZER;//北边放行的信号
pthread_cond_t queueEast = PTHREAD_COND_INITIALIZER;//东边放行的信号
pthread_cond_t queueSouth = PTHREAD_COND_INITIALIZER;//南边放行的信号
pthread_cond_t queueWest = PTHREAD_COND_INITIALIZER;//西边放行的信号

int sumN, sumE, sumS, sumW, sum;//用于记录各个方向总共的数量
int readyN, readyE, readyS, readyW, passed, nwait;//标记下一辆路口的车，路口等待的数量和经过的数量

typedef struct info Info;
struct info {
	int t;
	int id;
};//建立每辆车的总车数的id和当前方向的id

void* threadDeadlock(void* junk) {
	pthread_mutex_lock(&mutexPassed);//pthread_cond_wait(&a,&mutex);条件变量的使用总是和一个互斥锁结合在一起
	while (passed < sum) 
	{//判断是否所有车已经经过
		pthread_mutex_unlock(&mutexPassed);
		pthread_mutex_lock(&mutexWait);
		if (nwait == 4) {//发现死锁
			printf("DEADLOCK detected, north go first\n");
			pthread_cond_signal(&queueNorth);//放行北边
		}
		pthread_mutex_unlock(&mutexWait);
		usleep(1000);
		pthread_mutex_lock(&mutexPassed);
	}
	pthread_exit(NULL);
}//该函数用于对于路口死锁的检测

void *threadNorth(void* car_id) {
	Info* id = (Info*)(car_id);//获得其在该方向的id
	char *dir = "North";
	while (1) 
	{
		pthread_mutex_lock(&mutexNorth);
		if (id->id != 1) //检测是否为该方向第一辆
		{
			pthread_cond_wait(&firstNorth, &mutexNorth);//不是就等待下一辆的信号
		}

		if (id->id == readyN + 1) //检测是否为下一辆前往路口
		{
			pthread_mutex_unlock(&mutexNorth);
			printf("Car %d from %s is arriving, the %d of %s\n", id->t, dir, id->id, dir);
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			nwait++;//路口等待的车数加一
			pthread_mutex_unlock(&mutexWait);
			usleep(600);//模拟等待的状态
		}
		else //不是当前这辆车则将信号抛出
		{
			pthread_mutex_unlock(&mutexNorth);
			pthread_cond_signal(&firstNorth);
			usleep(20);//防止信号来回传递
			continue;
		}
		//排在队列最前面的车辆将被唤醒等待
		pthread_mutex_lock(&mutexWest);
		if (sumW != readyW) 
		{//检测右边是否有车
			pthread_mutex_unlock(&mutexWest);
			pthread_mutex_lock(&mutexNorthQ);
			pthread_cond_wait(&queueNorth, &mutexNorthQ);//有车则等待放行信号
			pthread_mutex_unlock(&mutexNorthQ);		
		}
		//如果右手边有车就等待通行的信号
		else 
		{//没有则行动
			pthread_mutex_unlock(&mutexWest);
		}
		//没车就走
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);

		pthread_mutex_lock(&mutexNorth);
		readyN++;
		pthread_mutex_unlock(&mutexNorth);
		//将下一辆行动的id加一
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//路口等待车辆减一
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//通过总车辆数加一
		usleep(100);
		pthread_cond_signal(&queueEast);
		//通知左手边的放行一辆
		usleep(100);
		pthread_cond_signal(&firstNorth);
		//让该方向下一辆车做好准备
		break;
	}
	pthread_exit(NULL);
}

void *threadEast(void* car_id) {//功能与前者一样
	Info* id = (Info*)(car_id);
	char *dir = "East";
	while (1) {
		pthread_mutex_lock(&mutexEast);
		if (id->id != 1) {
			pthread_cond_wait(&firstEast, &mutexEast);
		}
		if (id->id == readyE + 1) {
			pthread_mutex_unlock(&mutexEast);
			printf("Car %d from %s is arriving, the %d of %s\n", id->t, dir, id->id, dir);
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			nwait++;
			pthread_mutex_unlock(&mutexWait);
			usleep(600);
		}
		else {
			pthread_mutex_unlock(&mutexEast);
			pthread_cond_signal(&firstEast);
			usleep(20);
			continue;
		}
		//排在队列最前面的车辆将被唤醒等待
		pthread_mutex_lock(&mutexNorth);
		if (sumN != readyN) {
			pthread_mutex_unlock(&mutexNorth);
			pthread_mutex_lock(&mutexEastQ);
			pthread_cond_wait(&queueEast, &mutexEastQ);
			pthread_mutex_unlock(&mutexEastQ);
		}
		//如果右手边有车就等待通行的信号
		else {
			pthread_mutex_unlock(&mutexNorth);
		}
		//没车就走
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexEast);
		readyE++;
		pthread_mutex_unlock(&mutexEast);
		//将下一辆行动的id加一
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//路口等待车辆减一
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//通过总车辆数加一
		usleep(100);
		pthread_cond_signal(&queueSouth);
		//通知左手边的放行一辆
		usleep(100);
		pthread_cond_signal(&firstEast);
		//让该方向下一辆车做好准备
		break;
	}
	pthread_exit(NULL);
}

void *threadSouth(void* car_id) {//功能与前者一样
	Info* id = (Info*)(car_id);
	char *dir = "South";
	while (1) {
		pthread_mutex_lock(&mutexSouth);
		if (id->id != 1) {
			pthread_cond_wait(&firstSouth, &mutexSouth);
		}
		if (id->id == readyS + 1) {
			pthread_mutex_unlock(&mutexSouth);
			printf("Car %d from %s is arriving, the %d of %s\n", id->t, dir, id->id, dir);
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			nwait++;
			pthread_mutex_unlock(&mutexWait);
			usleep(600);
		}
		else {
			pthread_mutex_unlock(&mutexSouth);
			pthread_cond_signal(&firstSouth);
			usleep(20);
			continue;
		}
		//排在队列最前面的车辆将被唤醒等待
		pthread_mutex_lock(&mutexEast);
		if (sumE != readyE) {
			pthread_mutex_unlock(&mutexEast);
			pthread_mutex_lock(&mutexSouthQ);
			pthread_cond_wait(&queueSouth, &mutexSouthQ);
			pthread_mutex_unlock(&mutexSouthQ);
		}
		//如果右手边有车就等待通行的信号
		else {
			pthread_mutex_unlock(&mutexEast);
		}
		//没车就走
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexSouth);
		readyS++;
		pthread_mutex_unlock(&mutexSouth);
		//将下一辆行动的id加一
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//路口等待车辆减一
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//通过总车辆数加一
		usleep(100);
		pthread_cond_signal(&queueWest);
		//通知左手边的放行一辆
		usleep(100);
		pthread_cond_signal(&firstSouth);
		//让该方向下一辆车做好准备
		break;
	}
	pthread_exit(NULL);
}

void *threadWest(void* car_id) {//功能与前者一样
	Info* id = (Info*)(car_id);
	char *dir = "West";
	while (1) {
		pthread_mutex_lock(&mutexWest);
		if (id->id != 1) {
			pthread_cond_wait(&firstWest, &mutexWest);
		}
		if (id->id == readyW + 1) {
			pthread_mutex_unlock(&mutexWest);
			printf("Car %d from %s is arriving, the %d of %s\n", id->t, dir, id->id, dir);
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			nwait++;
			pthread_mutex_unlock(&mutexWait);
			usleep(600);
		}
		else {
			pthread_mutex_unlock(&mutexWest);
			pthread_cond_signal(&firstWest);
			usleep(20);
			continue;
		}
		//排在队列最前面的车辆将被唤醒等待
		pthread_mutex_lock(&mutexSouth);
		if (sumS != readyS) {
			pthread_mutex_unlock(&mutexSouth);
			pthread_mutex_lock(&mutexWestQ);
			pthread_cond_wait(&queueWest, &mutexWestQ);
			pthread_mutex_unlock(&mutexWestQ);
		}
		//如果右手边有车就等待通行的信号
		else {
			pthread_mutex_unlock(&mutexSouth);
		}
		//没车就走
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexWest);
		readyW++;
		pthread_mutex_unlock(&mutexWest);
		//将下一辆行动的id加一
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//路口等待车辆减一
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//通过总车辆数加一
		usleep(100);
		pthread_cond_signal(&queueNorth);
		//通知左手边的放行一辆
		usleep(100);
		pthread_cond_signal(&firstWest);
		//让该方向下一辆车做好准备
		break;
	}
	pthread_exit(NULL);
}

int main() {
	int i;
	char *input;
	pthread_t *t, d;
	Info *I;
	int n, e, s, w;
	readyN = readyE = readyS = readyW = sumN = sumE = sumS = sumW = 0;
	n = e = s = w = passed = nwait = 0;//初始化相关参数
	input = (char*)malloc(sizeof(char) * 21);
	scanf("%20s", input);//防止发生数组越界
	sum = strlen(input);//获取总数量
	t = (pthread_t*)malloc(sizeof(pthread_t)*sum);//线程id
	I = (Info*)malloc(sizeof(Info)*sum);//线程信息
	for (i = 0; i < sum; i++) {//获取每个方向的车辆数
		switch (input[i]) {
		case 'n':sumN++;break;
		case 'e':sumE++;break;
		case 's':sumS++;break;
		case 'w':sumW++;break;
		default:break;
		}
	}
	pthread_create(&d, NULL, threadDeadlock, NULL);
	for (i = 0; i < sum; i++) {//建立每一辆车的线程并且赋予不同的id
		switch (input[i]) 
		{
		case 'n':
			I[i].t = i + 1;
			I[i].id = ++n;
			pthread_create(&t[i], NULL, threadNorth, (void*)&(I[i]));
			break;
		case 'e':
			I[i].t = i + 1;
			I[i].id = ++e;
			pthread_create(&t[i], NULL, threadEast, (void*)&(I[i]));
			break;
		case 's':
			I[i].t = i + 1;
			I[i].id = ++s;
			pthread_create(&t[i], NULL, threadSouth, (void*)&(I[i]));
			break;
		case 'w':
			I[i].t = i + 1;
			I[i].id = ++w;
			pthread_create(&t[i], NULL, threadWest, (void*)&(I[i]));
			break;
		default:break;
		}
	}
	pthread_join(d, NULL);//等待死锁检测线程结束，即所有车都已经通过
	return 0;
}