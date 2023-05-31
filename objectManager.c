
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ObjectManager.h"

typedef struct Node node;
static void compact();

//node struct
struct Node
{
  Ref id;
  int size;
  int count;
  int start;
  node *next;
};

// top/free pointer/number of nodes in existence
node *top = NULL;
static int freePtr;


//two buffers
unsigned char * buffer_one;
unsigned char * buffer_two;

unsigned char *bufferInUse = NULL;


//initialies the pool
void initPool()
{
  buffer_one = (unsigned char *) malloc (sizeof(MEMORY_SIZE));
  buffer_two = (unsigned char *) malloc (sizeof(MEMORY_SIZE));
  freePtr = 0;
}

//destroys all nodes and two bufers
void destroyPool()
{
  assert(top != NULL);
  node *currNode = top;
  node *next;
  
  while ( currNode != NULL )
  {
    next = currNode->next;
    free( currNode );
    currNode = next;  
  }   
  free (buffer_one);   
  free (buffer_two);   
}

//inserts new objects into buffer.. checks for space first
Ref insertObject( int size )
{
  node *newNode;
  newNode = (node *)malloc(sizeof(node));
  Ref sizeRef = NULL_REF;
  
  assert(size > 0);
  assert(newNode!= NULL);

  //if list is empty
  if(top ==  NULL)
  {
    newNode->id = 1;
    newNode->size = size;
    newNode->start = 0;
    newNode->count = 1;

    newNode->next = NULL;
    top = newNode;

    freePtr += size; 
    sizeRef = newNode->id;
  }

  //if nodes already exist in list
  else
  {
    node *currNode = top;
    node *prevNode = NULL;

    while (currNode != NULL)
    {
      prevNode = currNode;
      currNode = currNode->next;
    }

    //if the previous nodes size plus the size we're trying to add is less than our buffer size
    if (size + (prevNode->start + prevNode->size) <= MEMORY_SIZE)
    {      
      newNode->id = prevNode->id + 1;
      newNode->size = size;
      newNode->count = 1;
      newNode->start = prevNode->start + prevNode->size;

      newNode->next = NULL;
      prevNode->next = newNode;

      freePtr += size;
      sizeRef = newNode->id;
    }
    
    else
    {
      //running garbage collecter
      compact();

      if (size + (prevNode->start + prevNode->size) <= MEMORY_SIZE)
      {
        newNode->id = prevNode->id + 1;
        newNode->size = size;
        newNode->count = 1;
        newNode->start = prevNode->start + prevNode->size;

        newNode->next = NULL;
        prevNode->next = newNode;

        freePtr += size;
        sizeRef = newNode->id;
      }

      else
      {
        printf("\nBUFFER IS FULL\n");
      }
    }
 
  }
  //assert(sizeRef != 0);
  return sizeRef;

}

//retrieves an object from our buffer
void *retrieveObject(Ref ref )
{
  assert(ref > 0);

  node *currNode = top;
  unsigned char *Objectptr = NULL;

  while (currNode != NULL)
  {
    if(currNode->id == ref)
    {
		  Objectptr = buffer_one + currNode->start; 
    }
    currNode = currNode->next;
  }

  assert(Objectptr != NULL);

  return Objectptr ;
}


void addReference( const Ref ref )
{
  assert(ref > 0);
  assert(ref != 0);

  node *currNode = top;

  while (currNode != NULL)
  {
    if(currNode->id == ref)
    {
      currNode->count++;
    }
    currNode = currNode->next;
  }
  
}

void dropReference( const Ref ref )
{
  assert(ref >= 0);

  node *currNode = top;

  while (currNode != NULL)
  {
    if(currNode->id == ref)
    {
      currNode->count--;
    }
    currNode = currNode->next;
  }
  
}


static void compact()
{
  node *currNode = top;
  node *prevNode = NULL;

  int buffer_two_start = 0;
  
  while (currNode != NULL)
  {
    if(currNode->count > 0)
    {
      //memcpy((void*)buffer_one, (void*)buffer_two, freePtr);
      
      currNode->start = buffer_two_start;      
      buffer_two_start = buffer_two_start + currNode->size; 

      prevNode = currNode;
      currNode = currNode->next;
    }

    else if(currNode->count <= 0 && prevNode != NULL)
    {
      prevNode->next = currNode->next;
      //bytesCollected += currNode->size;

      free(currNode);

      currNode = prevNode->next;
    }
  }
}

void dumpPool()
{
  assert(top !=  NULL);
  node *currNode = top;
  
  while (currNode != NULL)
  {
    printf("id: %lu\nsize: %d count: %d starts at: %d\n\n", currNode->id, currNode->size, currNode->count, currNode->start);
    currNode = currNode->next;
  }
}