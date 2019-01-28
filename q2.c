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

const int waiting=0;
const int assign=1;
const int complete=2;

typedef struct Booth Booth;
typedef struct EVM EVM;
typedef struct Voter Voter;

struct EVM
{
	int index;
	int flag;
	int nslots;
	pthread_t evm_id;
	Booth * booth;
};

struct Voter
{
	int index;
	int status;
	pthread_t voter_id;
	Booth * booth;
	EVM * evm;
};

struct Booth
{
	int index;
	int voter;
	int dvoters;
	int evm;
	pthread_t booth_id;
	Voter ** voters;
	EVM ** evms;
	pthread_mutex_t mutex;
	pthread_cond_t con1;
	pthread_cond_t con2;
};

void polling_ready_evm(Booth* booth,EVM* evm,int count)
{
	printf(MAGENTA"Evm %d at Booth Number %d free with slots = %d\n"RESET, evm->index+1 ,booth->index+1, count);
	return;
}

EVM* evm_init(int index, Booth * booth)
{
	EVM* evm;
	evm = (EVM*)malloc(sizeof(EVM));
	evm->nslots = 0;
	evm->flag = 0;
	evm->index = index;
	evm->booth = booth;
	return evm;
}

void voter_in_slot(Booth* booth,EVM* evm,int voter_index)
{
	printf(GREEN"Voter %d at Booth %d got allocated EVM %d\n"RESET, voter_index,booth->index+1, evm->index+1);
	return;
}

Voter* voter_init(int index, Booth * booth)
{
	Voter* voter;
	voter = (Voter*)malloc(sizeof(Voter));
	voter->index = index;
	voter->booth = booth;
	voter->evm = NULL;
	voter->status = -1;
	return voter;
}

Booth* booth_init( Booth * booth, int index, int evm, int voter)
{
	booth = (Booth*)malloc(sizeof(Booth));
	booth->index = index;
	booth->voter = voter;
	booth->dvoters = 0;
	pthread_mutex_init(&(booth->mutex), NULL);
	booth->evm = evm;
	pthread_cond_init(&(booth->con1), NULL);
	pthread_cond_init(&(booth->con2), NULL);
	booth->voters = (Voter**)malloc(sizeof(Voter*)*voter);
	booth->evms = (EVM**)malloc(sizeof(EVM*)*evm);
	return booth;
}

void * voter_wait_for_evm(void * args)
{
	Voter * voter = (Voter*)args;
	pthread_cond_t * cv_1_ptr = &(voter->booth->con1);
	pthread_cond_t * cv_2_ptr = &(voter->booth->con2);
	pthread_mutex_t * mutex_lock = &(voter->booth->mutex);
	pthread_mutex_lock(mutex_lock);
	voter->status = waiting;
	while(voter->status == waiting) 
		pthread_cond_wait(cv_1_ptr, mutex_lock); /* voter_wait_for_evm */
	pthread_mutex_unlock(mutex_lock);

	EVM * evm = voter->evm;
	pthread_mutex_lock(mutex_lock);
	while(evm->flag == 0)
		pthread_cond_wait(cv_1_ptr, mutex_lock);
	
	evm->nslots--;
	printf("Voter %d casted its vote at Booth %d of EVM %d\n", voter->index+1,evm->booth->index+1, evm->index+1);
	pthread_cond_broadcast(cv_2_ptr);
	pthread_mutex_unlock(mutex_lock);

	return NULL;
}

void * evmThread(void * args){

	EVM * evm = (EVM*)args;
	Booth * booth = evm->booth;

	pthread_mutex_t * mutex_lock = &(booth->mutex);
	pthread_cond_t * cv_2_ptr = &(booth->con2);
	pthread_cond_t * cv_1_ptr = &(booth->con1);

	int loop=1;

	while(loop){
		pthread_mutex_lock(mutex_lock);
		if(booth->dvoters == booth->voter)
		{
			pthread_mutex_unlock(mutex_lock);
			break;
		}
		pthread_mutex_unlock(mutex_lock);

		int j = 0;
		int slots_in_evm = rand()%10 + 1;
		pthread_mutex_lock(mutex_lock);
		evm->flag = 0;
		evm->nslots = slots_in_evm;
		pthread_mutex_unlock(mutex_lock);
		polling_ready_evm(booth,evm,slots_in_evm);

		while(loop)
		{
			if(j - slots_in_evm == 0)
				break;
			int i = rand() % booth->voter;
			pthread_mutex_lock(mutex_lock);
			if(booth->voters[i]->status == waiting)
			{
				booth->voters[i]->evm = evm;
				booth->voters[i]->status = assign;
				booth->dvoters++;
				voter_in_slot(booth,evm,i+1);
				j++;
			}
			if(booth->dvoters - booth->voter == 0)
			{
				pthread_mutex_unlock(mutex_lock);
				loop=0;
			}
			pthread_mutex_unlock(mutex_lock);
		}

		/* evm executing voting phase. */
		printf(CYAN  "Evm %d of Booth %d moved to voting phase.\n" RESET,  evm->index+1,booth->index+1);

		pthread_mutex_lock(mutex_lock);
		evm->flag = 1;
		evm->nslots = j;
		pthread_cond_broadcast(cv_1_ptr);
		while(evm->nslots)
			pthread_cond_wait(cv_2_ptr, mutex_lock);
		pthread_mutex_unlock(mutex_lock);
		sleep(1);
		printf("Evm %d of Booth %d has finished Voting phase.\n",  evm->index+1,booth->index+1);
	}
	return NULL;
}
void * booth_voting(void* args)
{
	int i=0;
	Booth * booth = (Booth*)args;
	/* evms and voters initialised */
	do
	{
		booth->voters[i] = voter_init(i, booth);
		i++;
	}while(booth->voter-i);
	
	i=0;
	do
	{
		booth->evms[i] = evm_init(i, booth);
		i++;
	}while(booth->evm-i);

	/* voters and evms threads start */
	i=0;
	do
	{
		int xx=pthread_create(&(booth->voters[i]->voter_id),NULL, voter_wait_for_evm, booth->voters[i]);
		if(xx!=0)
				{
					printf("THREAD CAN'T BE CREATED\n");
					exit(-1);
				}
		i++;
	}while(booth->voter-i);
	
	i=0;
	do
	{
		int xx=pthread_create(&(booth->evms[i]->evm_id),NULL, evmThread, booth->evms[i]);
		if(xx!=0)
				{
					printf("THREAD CAN'T BE CREATED\n");
					exit(-1);
				}
		i++;
	}while(booth->evm-i);

	/* evms and voters threads joined */
	i=0;
	do
	{
		pthread_join(booth->voters[i]->voter_id, 0);
		i++;
	}while(booth->voter-i);
	
	i=0;
	do
	{
		pthread_join(booth->evms[i]->evm_id, 0);
		i++;
	}while(booth->evm-i);

	printf(YELLOW "Voters at Booth Number %d are done with voting.\n" RESET, booth->index+1);

	/* freeing memory */
	i=0;
	do
	{
		free(booth->voters[i]);
		i++;
	}while(booth->voter-i);
	
	i=0;
	do
	{
		free(booth->evms[i]);
		i++;
	}while(booth->evm-i);
	free(booth->evms);
	free(booth->voters);

	return NULL;
}



int main()
{
	printf(GREEN"ENTER NUMBER OF BOOTHS->"RESET);
	int boothnum;
	scanf("%d", &boothnum);

	Booth ** booths = (Booth**)malloc(sizeof(Booth*)*boothnum);
	int evm[boothnum];
	int voter[boothnum];

	printf(GREEN"ENTER NUMBER OF EVMS AND VOTERS FOR EACH BOOTH\n"RESET);
	for(int i=0; i<boothnum; i++)
		scanf("%d%d",&evm[i],&voter[i]);
	
	printf(RED"ELECTION PROCESS STARTED.\n"RESET);

	/* booth initialisation */
	int i=0;
	do{
		booths[i] = booth_init( booths[i], i, evm[i], voter[i]);
		i++; 
	}while(boothnum-i);

	/* booth thread start */
	i=0;
	do{
		int xx=pthread_create(&(booths[i]->booth_id), NULL, booth_voting, booths[i]);
		if(xx!=0)
				{
					printf("THREAD CAN'T BE CREATED\n");
					exit(-1);
				}
		i++; 
	}while(boothnum-i);

	/* booth thread joined */
	i=0;
	do{
		pthread_join(booths[i]->booth_id, 0);
		i++; 
	}while(boothnum-i);

	printf(RED"ELECTION PROCESS COMPLETED\n"RESET);

	/* freeing alloted memory */
	i=0;
	do{
		free(booths[i]);
		i++; 
	}while(boothnum-i);

	free(booths);

	return 0;
}