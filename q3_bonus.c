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
typedef struct
{
    int low;
    int up;
} data;
int num[200009];
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
		// printf("%d--",min);
		// for(int j=start;j<=end;j++)
		// 	printf("%d--",arr[j]);
		// printf("\n");
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
    // for(int i=start;i<=end;i++)
    // 	printf("%d--",arr[i]);
    // printf("\n");
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
void* threadsort(void* arg)
{
	data *d=(data *)arg;
	int end=d->up;
	int start=d->low;
	int mid=(end-start)/2+start;
	if(end-start+1<6)
	{
		selection(num,start,end);
		return NULL;
	}
 	data left;
    left.up=mid;
    left.low=start;

    pthread_t thread[2];
    pthread_create(&thread[0], NULL, threadsort, &left);

    data right;
    right.up=end;
    right.low=mid+1;

    pthread_create(&thread[1], NULL, threadsort, &right);

    for(int i=0;i<2;i++)
	    pthread_join(thread[i], NULL); 

    merge(num,start,mid,end); 
    return NULL;
}
int main(){
	long long n;
	scanf("%lld",&n);
	int nums[n];
	for(int i=0;i<n;i++)
	{
		num[i]=rand()%10000+1;
		nums[i]=num[i];
	}
	clock_t th1,th2;
	th1=clock();
		data d;
		d.up=n-1;
		d.low=0;
		pthread_t sort;
		pthread_create(&sort,NULL,threadsort,&d);
		pthread_join(sort,NULL);
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
	{
		printf("SORTING UNSUCCESSFUL\n");
	}
	printf("TIME TAKEN BY THREADS - %f\n",perf/(double)CLOCKS_PER_SEC);
	th1=clock();
	mergesort(nums,0,n-1);
	th2=clock();
	printf("TIME TAKEN BY Recurssion - %f\n",(th2-th1)/(double)CLOCKS_PER_SEC);
	return 0;
}