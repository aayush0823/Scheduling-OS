#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
void selection(int arr[],int start,int end)
{
	// for(int i=start;i<=end;i++)
	// 	printf("%d--",arr[i]);
	// printf("\n");
	if(start>=end)return;
	for(int i=start;i<=end;i++)
	{
		int min=i;
		for(int j=i+1;j<=end;j++)
		{
			if(arr[j]<arr[min])
				min=j;
		}
		int temp=arr[i];
		arr[i]=arr[min];
		arr[min]=temp;
		//printf("-->%d  %d   ",i,min);
	}
	// for(int i=start;i<=end;i++)
	// 	printf("%d--",arr[i]);
	// printf("\n");
	return;
}
void merge(int arr[],int start,int mid,int end)
{
    int left[mid-start+1];
    int right[end-mid];
    for(int i=0;i<mid-start+1;i++)
    {
    	left[i]=arr[start+i];
    }
    for(int i=0;i<end-mid;i++)
    {
    	right[i]=arr[mid+1+i];
    }
    int l=0,r=0,m=start;
    while(l<mid-start+1 && r<end-mid)
    {
        if(left[l]<=right[r])
        {
			arr[m++]=left[l++];
        }
        else
        {
        	arr[m++]=right[r++];
        }
    }
    while(l<mid-start+1)
	{
		arr[m++]=left[l++];
	}
    while(r<end-mid)
    {
    	arr[m++]=right[r++];
    }
    return;
}
void mergesort(int arr[],int start,int end)
{
    if(start<end)
    {
 	   int mid=(end-start)/2+start;
  	  mergesort(arr,start,mid);
  	  mergesort(arr,mid+1,end);
	    merge(arr,start,mid,end);
	}	
    return;
}
void mergesortf(int arr[],int start,int end)
{
	if(end-start+1<6)
	{
		selection(arr,start,end);
		return;
	}
 	  int mid=(end-start)/2+start;
 	  pid_t left,right;
 	  left=fork();
 	  if(left<0)
 	  {
 	  	perror("Left Child Not Created");
 	  	exit(-1);
 	  }
 	  else if(left==0)
 	  {
	  	  mergesort(arr,start,mid);
	  	  exit(0);	
 	  }
 	  else
 	  {
 	  	right=fork();
 	  	if(right<0)
 	  	{
 	  		perror("Right Child Not Created");
 	  		exit(-1);
 	  	}
 	  	else if(right==0)
 	  	{
  	 		 mergesort(arr,mid+1,end);
  	 		 exit(0);
 	  	}
 	  }
 	  int status;
 	  waitpid(left,&status,0);
 	  waitpid(right,&status,0);
	  merge(arr,start,mid,end);	
    return;
}
int main(){
	long long n;
	scanf("%lld",&n);
	int *num;
	int nums[n];
	int sid;
	clock_t th1,th2;
	th1=clock();
		sid=shmget(IPC_PRIVATE,sizeof(int)*n,IPC_CREAT | 0666);
		if(sid<0)
		{
			perror("shmget");
			return -1;
		}
		num=shmat(sid,NULL,0);
		for(int i=0;i<n;i++)
		{
			num[i]=rand();
			nums[i]=num[i];
		}
		mergesortf(num,0,n-1);
	th2=clock();
	clock_t perf=th2-th1;
	int yes=1;
	printf("%d  ",num[0]);
	for(int i=1;i<n;i++)
	{
		printf("%d ",num[i]);
		if(num[i]<num[i-1])
		{
			yes=0;
			break;
		}
	}
	printf("\n");
	if(yes)
		printf("SORTING SUCCESSFUL\n");
	else
		printf("SORTING UNSUCCESSFUL\n");

	printf("TIME TAKEN BY PROCESSES - %f\n",perf/(double)CLOCKS_PER_SEC);
	th1=clock();
	mergesort(nums,0,n-1);
	th2=clock();
	printf("TIME TAKEN BY Recurssion - %f\n",(th2-th1)/(double)CLOCKS_PER_SEC);
	return 0;
}