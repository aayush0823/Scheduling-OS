#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


pthread_mutex_t lock;
int pcount=0;
int rcount=0;
int entp[20000];
int entr[10000];
void meetOrganizer(int x,int y)
{
	if(y==0)
		printf("Player %d Met Organiser\n",x);
	else
		printf(YELLOW "Refree %d Met Organiser\n" RESET,x);
	return;
}
void enterCourt(int x,int y)
{
	if(y==0)
		printf("Player %d Entered The Court\n",x);
	else
		printf(GREEN "Refree %d Entered The Court\n" RESET,x);
	return;
}
void warmUp(int a)
{
	printf(CYAN "Player %d starts warming up\n" RESET,a);
	sleep(1);
	return;
}
void adjustEquipment(int a)
{
	printf(MAGENTA "Refree %d adjusts equipments\n" RESET,a);
	sleep(0.5);
	return;
}
void startGame(int x)
{
	printf(RED "Refree %d starts game\n" RESET,x);
	return;
}
void* EnterPlayer(void * arg)
{
	int ploc;
	pcount++;
	printf("Player %d Entered The Academy\n",pcount);
	ploc=pcount;
	entp[ploc]=1;
	if(ploc%2==0)
	{
		while(entp[ploc-1]==0 || entr[ploc/2]==0);
		while(entr[ploc/2]!=2);
		meetOrganizer(ploc,0);
		enterCourt(ploc,0);
		warmUp(ploc);
		entp[ploc]=2;
	}
	else
	{
		while(entp[ploc+1]==0 || entr[ploc/2+1]==0);
		while(entr[ploc/2+1]!=2);
		meetOrganizer(ploc,0);
		enterCourt(ploc,0);
		warmUp(ploc);
		entp[ploc]=2;
	}
}
void* EnterRefree(void * arg)
{
	int rloc;
	rcount++;
	printf("Refree %d Entered The Academy\n",rcount);
	rloc=rcount;
	entr[rloc]=1;
	while(entp[2*rloc-1]==0 || entp[2*rloc]==0);
		pthread_mutex_lock(&lock);
			entr[rloc]=2;
			meetOrganizer(rloc,1);
			enterCourt(rloc,1);
			adjustEquipment(rloc);
			while(entp[2*rloc]!=2 || entp[2*rloc-1]!=2);
			startGame(rloc);
		pthread_mutex_unlock(&lock);
}

int main(){
	int n;
	scanf("%d",&n);
	pthread_t play[2*n+1];
	pthread_t ref[n+1];
	int players=0;
	int refree=0;
	int delay=0;
	// time_t rawtime;
 //  	struct tm * timeinfo;
	// time ( &rawtime );
 //  	timeinfo = localtime ( &rawtime );
	// int presec=timeinfo->tm_sec;
	while( players<2*n || refree<n )
	{
		// time ( &rawtime );
  // 		timeinfo = localtime ( &rawtime );
		// if(timeinfo->tm_sec-presec>=delay)
		// {
			sleep(delay);
			int remplayers=2*n-players;
			int remrefree=n-refree;
			int x=rand()%(remplayers+remrefree)+1;
			if(x<=remplayers)
			{
				players++;
				int xx=pthread_create(&(play[players]),NULL,EnterPlayer,(void *)NULL);
				if(xx!=0)
				{
					printf("THREAD CAN'T BE CREATED\n");
					return 1;
				}
			}
			else
			{
				refree++;
				int xx=pthread_create(&(ref[refree]),NULL,EnterRefree,(void *)NULL);
				if(xx!=0)
				{
					printf("THREAD CAN'T BE CREATED\n");
					return 1;
				}
			}
			delay=rand()%3+1;
			// presec=timeinfo->tm_sec;
		// }
	}
	for(int i=1;i<n+1;i++)
	{
		pthread_join(ref[i],NULL);
		pthread_join(play[2*i],NULL);
		pthread_join(play[2*i-1],NULL);
	}
	return 0;
}