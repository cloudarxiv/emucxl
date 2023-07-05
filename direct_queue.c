#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "emucxl_lib.h"
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
    struct node *front;
    struct node *rear;
};

struct node* createNode(int data)
{
    struct node *newnode;
    newnode = (struct node*)emucxl_alloc(sizeof(struct node), LOCAL_MEMORY);
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


struct Queue* createQueue()
{
    struct Queue *que = (struct Queue*)emucxl_alloc(sizeof(struct Queue), LOCAL_MEMORY);
    if(!que)
        return NULL;
    que->front = que->rear = NULL;
    que->count = 0;
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
    struct node *newnode = createNode(data);
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
int dequeue(struct Queue *que)
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
    printf("Queue destroyed\n");
}


int main(int argc , char *argv[])
{
    emucxl_init();

    struct Queue *que = createQueue();
    if(que == NULL)
    {
        printf("Queue creation failed\n");
        return 0;
    }
    printf("DEBUG: Queue created\n");
    printf("Check Queue is empty or not: "); queueEmpty(que);
    printf("Queue state: "); queueDisplay(que);

    printf("Queue: Enqueue 20 elements\n");
    for(int i = 0; i < 20; i++) {
        enqueue(que, i*10);
    }

    printf("Queue state: "); queueDisplay(que);
    printf("Queue size: %d\n", que->count);

    printf("Queue front: %d\n", queueFrontElement(que));

    printf("Queue: Dequeue 5 elements\n");
    for(int i = 0; i < 5; i++) {
        dequeue(que);
    }

    printf("Queue: front: %d\n", queueFrontElement(que));

    printf("Queue: "); queueDisplay(que);

    printf("Check Queue is empty or not: "); queueEmpty(que);

    printf("Queue: Enqueue 10 elements\n");
    for (int i = 0; i < 10; i++) {
        enqueue(que, i*10 + 200);
    }

    printf("Queue: "); queueSize(que);

    printf("Queue state: "); queueDisplay(que);

    queueDestroy(que);

    emucxl_exit();
    printf("DEBUG: Queue cleanup and deleted and exited from emucxl\n");
    return 0;
}