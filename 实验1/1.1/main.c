#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutexNorth = PTHREAD_MUTEX_INITIALIZER;//Ϊ���ߵ�һ�����ź����ṩ������
pthread_mutex_t mutexEast = PTHREAD_MUTEX_INITIALIZER;//Ϊ���ߵ�һ�����ź����ṩ������
pthread_mutex_t mutexSouth = PTHREAD_MUTEX_INITIALIZER;//Ϊ�ϱߵ�һ�����ź����ṩ������
pthread_mutex_t mutexWest = PTHREAD_MUTEX_INITIALIZER;//Ϊ���ߵ�һ�����ź����ṩ������

pthread_mutex_t mutexWait = PTHREAD_MUTEX_INITIALIZER;//Ϊ·�ڵȴ��������ı����ṩ����
pthread_mutex_t mutexPassed = PTHREAD_MUTEX_INITIALIZER;//Ϊ�Ѿ����ĳ����������ṩ����

pthread_mutex_t mutexNorthQ = PTHREAD_MUTEX_INITIALIZER;//Ϊ���߷��е��ź����ṩ������
pthread_mutex_t mutexEastQ = PTHREAD_MUTEX_INITIALIZER;//Ϊ���߷��е��ź����ṩ������
pthread_mutex_t mutexSouthQ = PTHREAD_MUTEX_INITIALIZER;//Ϊ�ϱ߷��е��ź����ṩ������
pthread_mutex_t mutexWestQ = PTHREAD_MUTEX_INITIALIZER;//Ϊ���߷��е��ź����ṩ������

pthread_cond_t firstNorth = PTHREAD_COND_INITIALIZER;//���ߵ�һ�����ź�
pthread_cond_t firstEast = PTHREAD_COND_INITIALIZER;//���ߵ�һ�����ź�
pthread_cond_t firstSouth = PTHREAD_COND_INITIALIZER;//�ϱߵ�һ�����ź�
pthread_cond_t firstWest = PTHREAD_COND_INITIALIZER;//���ߵ�һ�����ź�

pthread_cond_t queueNorth = PTHREAD_COND_INITIALIZER;//���߷��е��ź�
pthread_cond_t queueEast = PTHREAD_COND_INITIALIZER;//���߷��е��ź�
pthread_cond_t queueSouth = PTHREAD_COND_INITIALIZER;//�ϱ߷��е��ź�
pthread_cond_t queueWest = PTHREAD_COND_INITIALIZER;//���߷��е��ź�

int sumN, sumE, sumS, sumW, sum;//���ڼ�¼���������ܹ�������
int readyN, readyE, readyS, readyW, passed, nwait;//�����һ��·�ڵĳ���·�ڵȴ��������;���������

typedef struct info Info;
struct info {
	int t;
	int id;
};//����ÿ�������ܳ�����id�͵�ǰ�����id

void* threadDeadlock(void* junk) {
	pthread_mutex_lock(&mutexPassed);//pthread_cond_wait(&a,&mutex);����������ʹ�����Ǻ�һ�������������һ��
	while (passed < sum) 
	{//�ж��Ƿ����г��Ѿ�����
		pthread_mutex_unlock(&mutexPassed);
		pthread_mutex_lock(&mutexWait);
		if (nwait == 4) {//��������
			printf("DEADLOCK detected, north go first\n");
			pthread_cond_signal(&queueNorth);//���б���
		}
		pthread_mutex_unlock(&mutexWait);
		usleep(1000);
		pthread_mutex_lock(&mutexPassed);
	}
	pthread_exit(NULL);
}//�ú������ڶ���·�������ļ��

void *threadNorth(void* car_id) {
	Info* id = (Info*)(car_id);//������ڸ÷����id
	char *dir = "North";
	while (1) 
	{
		pthread_mutex_lock(&mutexNorth);
		if (id->id != 1) //����Ƿ�Ϊ�÷����һ��
		{
			pthread_cond_wait(&firstNorth, &mutexNorth);//���Ǿ͵ȴ���һ�����ź�
		}

		if (id->id == readyN + 1) //����Ƿ�Ϊ��һ��ǰ��·��
		{
			pthread_mutex_unlock(&mutexNorth);
			printf("Car %d from %s is arriving, the %d of %s\n", id->t, dir, id->id, dir);
			fflush(stdout);
			pthread_mutex_lock(&mutexWait);
			nwait++;//·�ڵȴ��ĳ�����һ
			pthread_mutex_unlock(&mutexWait);
			usleep(600);//ģ��ȴ���״̬
		}
		else //���ǵ�ǰ���������ź��׳�
		{
			pthread_mutex_unlock(&mutexNorth);
			pthread_cond_signal(&firstNorth);
			usleep(20);//��ֹ�ź����ش���
			continue;
		}
		//���ڶ�����ǰ��ĳ����������ѵȴ�
		pthread_mutex_lock(&mutexWest);
		if (sumW != readyW) 
		{//����ұ��Ƿ��г�
			pthread_mutex_unlock(&mutexWest);
			pthread_mutex_lock(&mutexNorthQ);
			pthread_cond_wait(&queueNorth, &mutexNorthQ);//�г���ȴ������ź�
			pthread_mutex_unlock(&mutexNorthQ);		
		}
		//������ֱ��г��͵ȴ�ͨ�е��ź�
		else 
		{//û�����ж�
			pthread_mutex_unlock(&mutexWest);
		}
		//û������
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);

		pthread_mutex_lock(&mutexNorth);
		readyN++;
		pthread_mutex_unlock(&mutexNorth);
		//����һ���ж���id��һ
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//·�ڵȴ�������һ
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//ͨ���ܳ�������һ
		usleep(100);
		pthread_cond_signal(&queueEast);
		//֪ͨ���ֱߵķ���һ��
		usleep(100);
		pthread_cond_signal(&firstNorth);
		//�ø÷�����һ��������׼��
		break;
	}
	pthread_exit(NULL);
}

void *threadEast(void* car_id) {//������ǰ��һ��
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
		//���ڶ�����ǰ��ĳ����������ѵȴ�
		pthread_mutex_lock(&mutexNorth);
		if (sumN != readyN) {
			pthread_mutex_unlock(&mutexNorth);
			pthread_mutex_lock(&mutexEastQ);
			pthread_cond_wait(&queueEast, &mutexEastQ);
			pthread_mutex_unlock(&mutexEastQ);
		}
		//������ֱ��г��͵ȴ�ͨ�е��ź�
		else {
			pthread_mutex_unlock(&mutexNorth);
		}
		//û������
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexEast);
		readyE++;
		pthread_mutex_unlock(&mutexEast);
		//����һ���ж���id��һ
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//·�ڵȴ�������һ
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//ͨ���ܳ�������һ
		usleep(100);
		pthread_cond_signal(&queueSouth);
		//֪ͨ���ֱߵķ���һ��
		usleep(100);
		pthread_cond_signal(&firstEast);
		//�ø÷�����һ��������׼��
		break;
	}
	pthread_exit(NULL);
}

void *threadSouth(void* car_id) {//������ǰ��һ��
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
		//���ڶ�����ǰ��ĳ����������ѵȴ�
		pthread_mutex_lock(&mutexEast);
		if (sumE != readyE) {
			pthread_mutex_unlock(&mutexEast);
			pthread_mutex_lock(&mutexSouthQ);
			pthread_cond_wait(&queueSouth, &mutexSouthQ);
			pthread_mutex_unlock(&mutexSouthQ);
		}
		//������ֱ��г��͵ȴ�ͨ�е��ź�
		else {
			pthread_mutex_unlock(&mutexEast);
		}
		//û������
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexSouth);
		readyS++;
		pthread_mutex_unlock(&mutexSouth);
		//����һ���ж���id��һ
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//·�ڵȴ�������һ
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//ͨ���ܳ�������һ
		usleep(100);
		pthread_cond_signal(&queueWest);
		//֪ͨ���ֱߵķ���һ��
		usleep(100);
		pthread_cond_signal(&firstSouth);
		//�ø÷�����һ��������׼��
		break;
	}
	pthread_exit(NULL);
}

void *threadWest(void* car_id) {//������ǰ��һ��
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
		//���ڶ�����ǰ��ĳ����������ѵȴ�
		pthread_mutex_lock(&mutexSouth);
		if (sumS != readyS) {
			pthread_mutex_unlock(&mutexSouth);
			pthread_mutex_lock(&mutexWestQ);
			pthread_cond_wait(&queueWest, &mutexWestQ);
			pthread_mutex_unlock(&mutexWestQ);
		}
		//������ֱ��г��͵ȴ�ͨ�е��ź�
		else {
			pthread_mutex_unlock(&mutexSouth);
		}
		//û������
		printf("Car %d from %s is leaving, the %d of %s\n", id->t, dir, id->id, dir);
		fflush(stdout);
		pthread_mutex_lock(&mutexWest);
		readyW++;
		pthread_mutex_unlock(&mutexWest);
		//����һ���ж���id��һ
		pthread_mutex_lock(&mutexWait);
		nwait--;
		pthread_mutex_unlock(&mutexWait);
		//·�ڵȴ�������һ
		pthread_mutex_lock(&mutexPassed);
		passed++;
		pthread_mutex_unlock(&mutexPassed);
		//ͨ���ܳ�������һ
		usleep(100);
		pthread_cond_signal(&queueNorth);
		//֪ͨ���ֱߵķ���һ��
		usleep(100);
		pthread_cond_signal(&firstWest);
		//�ø÷�����һ��������׼��
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
	n = e = s = w = passed = nwait = 0;//��ʼ����ز���
	input = (char*)malloc(sizeof(char) * 21);
	scanf("%20s", input);//��ֹ��������Խ��
	sum = strlen(input);//��ȡ������
	t = (pthread_t*)malloc(sizeof(pthread_t)*sum);//�߳�id
	I = (Info*)malloc(sizeof(Info)*sum);//�߳���Ϣ
	for (i = 0; i < sum; i++) {//��ȡÿ������ĳ�����
		switch (input[i]) {
		case 'n':sumN++;break;
		case 'e':sumE++;break;
		case 's':sumS++;break;
		case 'w':sumW++;break;
		default:break;
		}
	}
	pthread_create(&d, NULL, threadDeadlock, NULL);
	for (i = 0; i < sum; i++) {//����ÿһ�������̲߳��Ҹ��費ͬ��id
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
	pthread_join(d, NULL);//�ȴ���������߳̽����������г����Ѿ�ͨ��
	return 0;
}