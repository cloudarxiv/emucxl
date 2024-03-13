#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#include "../src/emucxl_lib.h"
#define LOCAL_MEMORY 0
#define REMOTE_MEMORY 1

struct node
{
    int data;
    struct node *next;
};

struct Queue
{
    int count;
    int policy;
    struct node *front;
    struct node *rear;
};

struct node* createNode(struct Queue *que, int data)
{
    struct node *newnode;
    newnode = (struct node*)emucxl_alloc(sizeof(struct node), que->policy);
    if(newnode == NULL)
    {
        printf("alloc failed\n");
        return NULL;
    }
    else
    {
        newnode->data = data;
        newnode->next = NULL;
        return newnode;
    }
}


struct Queue* createQueue(int flag) // flag = 0 for local memory, flag = 1 for remote memory
{
    struct Queue *que = (struct Queue*)emucxl_alloc(sizeof(struct Queue), flag ? REMOTE_MEMORY : LOCAL_MEMORY);
    if(!que)
        return NULL;
    que->front = que->rear = NULL;
    que->count = 0;
    que->policy = flag ? REMOTE_MEMORY : LOCAL_MEMORY;
    return que;
}
 
int queueFrontElement(struct Queue *que)
{
    if((que->front != NULL) && (que->rear != NULL))
        return que->front->data;
    else
        return 0;
}
void queueEmpty(struct Queue *que)
{
     if((que->front == NULL) && (que->rear == NULL))
        printf("Queue is empty\n");
    else
       printf("Queue not empty\n");
}

void queueSize(struct Queue *que)
{
    printf("Queue size is %d\n", que->count);
}
void enqueue(struct Queue *que, int data)
{
    struct node *newnode = createNode(que, data);
    if((que->front == NULL) && (que->rear == NULL))
    {
        que->front = que->rear = newnode;
        que->count++;
        return;
    }
    que->rear->next = newnode;
    que->rear = newnode;
    que->count++;
    return;
}
void dequeue(struct Queue *que)
{
    struct node *temp;
    if((que->front == NULL) && (que->rear == NULL))
    {
        printf("Queue is empty\n");
        return;
    }
    temp = que->front;
    que->front = que->front->next;
    if(que->front == NULL)
        que->rear = NULL;
    emucxl_free(temp, sizeof(struct node));
    que->count--;
    return;
}
void queueDisplay(struct Queue *que)
{
    struct node *temp;
    temp = que->front;
    if((que->front == NULL) && (que->rear == NULL))
    {
        printf("Queue is empty\n");
        return;
    }
    while(temp != NULL)
    {
        printf("%d ",temp->data);
        temp = temp->next;
    }
    printf("\n");
}

void queueDestroy(struct Queue *que)
{
    struct node *temp;
    while(que->front != NULL)
    {
        temp = que->front;
        que->front = que->front->next;
        emucxl_free(temp, sizeof(struct node));
    }
    que->count = 0;
    que->front = que->rear = NULL;
    emucxl_free(que, sizeof(struct Queue));
#ifdef DEBUG
    printf("Queue destroyed\n");
#endif
}

void testCase(struct Queue *que, int num_enq, int num_deque, int num_enq_next, const char *test_name)
{
#ifdef DEBUG
    printf("DEBUG: Queue created\n");
    printf("Check Queue is empty or not: "); queueEmpty(que);
    printf("Queue state: "); queueDisplay(que);
    printf("Queue: Enqueue 20 elements\n");
#endif
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    for(int i = 0; i < num_enq; i++) {
        enqueue(que, i*2);
    }
#ifdef DEBUG
    printf("%s_Time_Enqueue: %f\n", test_name, cpu_time_used);
    printf("Queue state: "); queueDisplay(que);
    printf("Queue size: %d\n", que->count);
    printf("Queue front: %d\n", queueFrontElement(que));
    printf("Queue: Dequeue 5 elements\n");
#endif
    start = clock();
    for(int i = 0; i < num_deque; i++) {
        dequeue(que);
    }
#ifdef DEBUG
    printf("%s_Time_Dequeue: %f\n", test_name, cpu_time_used);
    printf("Queue: front: %d\n", queueFrontElement(que));
    printf("Queue: "); queueDisplay(que);
    printf("Check Queue is empty or not: "); queueEmpty(que);
    printf("Queue: Enqueue 10 elements\n");
#endif
    for (int i = 0; i < num_enq_next; i++) {
        enqueue(que, i*2 + num_enq*2);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s_Time: %f\n", test_name, cpu_time_used);
#ifdef DEBUG
    printf("%s_Time_Next_Enqueue: %f\n", test_name, cpu_time_used);
    printf("Queue size: %d\n", que->count);
    printf("Queue: "); queueSize(que);
    printf("Queue state: "); queueDisplay(que);
#endif
}

void testCase2(struct Queue *que, int num, const char *test_name)
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    for(int i = 0; i < num; i++) {
        enqueue(que, i*2);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s_Enque_Time: %.2f\n", test_name, cpu_time_used * 1000.0);
    start = clock();
    for(int i = 0; i < num; i++) {
        dequeue(que);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%s_Deque_Time: %.2f\n", test_name, cpu_time_used * 1000.0);
}


int main(int argc , char *argv[])
{
    emucxl_init();
#ifdef DEBUG
    printf("DEBUG: LOCAL OBJECT\n");
#endif
    struct Queue *que = createQueue(LOCAL_MEMORY);
    if(que == NULL)
    {   
        #ifdef DEBUG
        printf("DEBUG: Queue creation failed\n");
        #endif
        emucxl_exit();
        return 0;
    }
    int num = 15000;
    printf("Number of elements: %d enqueue followed by dequeue\n", num);
    testCase2(que, num, "Local");
#ifdef DEBUG
    printf("DEBUG: REMOTE OBJECT\n");
#endif
    queueDestroy(que);
    struct Queue *que_remote = createQueue(REMOTE_MEMORY);
    if (que_remote == NULL)
    {   
        #ifdef DEBUG
        printf("DEBUG: Queue creation failed\n");
        #endif
        queueDestroy(que);
        emucxl_exit();
        return 0;
    }
    testCase2(que_remote, num, "Remote");
    queueDestroy(que_remote);

    emucxl_exit();
#ifdef DEBUG
    printf("DEBUG: Queue cleanup and deleted and exited from emucxl\n");
#endif
    return 0;
}