// MMList.c ... implementation of List-of-Mail-Messages ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MailMessage.h"

// data structures representing MMList

typedef struct MMListNode *Link;

typedef struct MMListNode {
	MailMessage data; // message associated with this list item
	Link next;		  // pointer to node containing next element
} MMListNode;

typedef struct MMListRep {
	Link first; // node containing first value
	Link last;  // node containing last value
	Link curr;  // current node (for iteration)
} MMListRep;

static Link newMMListNode (MailMessage mesg);

// create a new empty MMList
MMList newMMList (void)
{
	MMListRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList");
	*new = (MMListRep) { };
	return new;
}

// free up memory associated with list
// note: does not remove Mail Messages
void dropMMList (MMList L)
{
	assert (L != NULL);
	Link curr = L->first;
	while (curr != NULL) {
		Link next = curr->next;
		free (curr);
		curr = next;
	}
	free (L);
}

// display list as one integer per line on stdout
void showMMList (MMList L)
{
	assert (L != NULL);
	for (Link curr = L->first; curr != NULL; curr = curr->next)
		showMailMessage (curr->data, 0);
}

// insert mail message in order
// ordering based on MailMessageDateTime
void MMListInsert (MMList L, MailMessage mesg)
{
	assert (L != NULL);
	assert (MMListIsOrdered (L));
	assert (mesg != NULL);
    
    //FLAG
    int inserted = 0;
    
    //LIST NODE FOR NEW MSG.
    Link newNode = newMMListNode(mesg);

   
    if (L->first == NULL) {
        
        //GIVEN AN EMPTY LIST
        
        L->first = newNode;
        L->curr = L->first;
        L->last = L->first;
        
   
    } else if (L->first->next == NULL){
        
        //GIVEN LIST WITH ONLY ONE ELEMENT IN IT.
        /*
         If an item with a duplicate time stamp comes up it falls in the else catagory where it will be inserted before.
         */
        
        if(DateTimeAfter(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->data))){
            
            //AFTER ONLY ELEMENT
            
            L->curr->next = newNode;
            L->last = newNode;
            
        } else {
            
            //BEFOR ONLY ELEMENT
            newNode->next = L->first;
            L->first = newNode;
        }
        
    } else {
        
        //GIVEN LIST WITH LIST SIZE >= 2


        //MAKING SURE L-.CURR IS AT BEGGINING ON LIST
        L->curr = L->first;

        //LOOP THROUGH GIVEN LIST CHECKING DATES AGAINST DATE FOR MESG.
        while(L->curr->next != NULL && inserted == 0){
            
            if(DateTimeAfter(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->data)) == 0 &&
               DateTimeBefore(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->data)) == 0) {
                
                //IF NEW NODE HAS IDENTIAL TIMESTAMP AS..
                
                if(L->curr == L->first){
                    
                    //..FIRST, THEN ADD IT BEFORE FIRST.
                    
                    newNode->next = L->first;
                    L->first = newNode;
                    
                    
                } else {
                    
                    
                    //..A RANDOM ELEMENT IN LIST, THEN ADD IT BEFORE THAT ELEMENT IN THE LIST.
                    newNode->next = L->curr->next;
                    L->curr->next = newNode;
                }
            
                inserted = 1;
                
            } else {
            
                    if(DateTimeBefore(MailMessageDateTime(newNode->data),MailMessageDateTime(L->first->data))){
                        
                        //BEFORE FIRST ELEMENT
                        
                        newNode->next = L->first;
                        L->first = newNode;
                        inserted = 1;
                        
                    }else if(DateTimeAfter(MailMessageDateTime(newNode->data),MailMessageDateTime(L->last->data))) {
                        
                        //AFTER LAST ELEMENT
                        
                        L->last->next = newNode;
                        L->last = newNode;
                        inserted = 1;
                        
                    }else if (L->curr->next == L->last && DateTimeAfter(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->next->data)) == 0 &&
                              DateTimeBefore(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->next->data)) == 0) {
                        
                        //IF NEW NODE HAS IDENTICAL TIME STAMP AS LAST ELEMENT ADD IT BEFORE LAST ELEMENT.
                        
                        newNode->next = L->last;
                        L->curr->next = newNode;
                        inserted = 1;
                        
                        
                    }else{
                        
                        if(DateTimeAfter(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->data)) &&
                           DateTimeBefore(MailMessageDateTime(newNode->data),MailMessageDateTime(L->curr->next->data))){

                            //INBETWEEEN TWO ELEMENTS IN THE LIST.
                            
                            newNode->next = L->curr->next;
                            L->curr->next = newNode;
                            inserted = 1;
                        }
                    }
            }
           
            L->curr = L->curr->next;

        }
        
    }
}

// create a new MMListNode for mail message
// (this function is used only within the ADT)
static Link newMMListNode (MailMessage mesg)
{
	Link new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate MMList node");
	*new = (MMListNode) { .data = mesg };
	return new;
}

// check whether a list is ordered (by MailMessageDate)
bool MMListIsOrdered (MMList L)
{
	DateTime prevDate = NULL;
	for (Link n = L->first; n != NULL; n = n->next) {
		DateTime currDate = MailMessageDateTime (n->data);
		if (prevDate != NULL && DateTimeAfter (prevDate, currDate))
			return false;
		prevDate = currDate;
	}
	return true;
}

// start scan of an MMList
void MMListStart (MMList L)
{
	assert (L != NULL);
    
    //printf("L-curr before it points to first is equal to %s ---------- MIN\n",MailMessageID (L->curr->data));
    //printf("L-first before L->curr points to it is equal to %s ---------- MIN\n",MailMessageID (L->first->data));
    
	L->curr = L->first;
    
    //printf("L-curr After it points to first is equal to %s ---------- MIN\n",MailMessageID (L->curr->data));

}

// get next item during scan of an MMList
MailMessage MMListNext (MMList L)
{
	assert (L != NULL);
    if (L->curr == NULL) {
		// this is probably an error
		return NULL;
    }

	MailMessage mesg = L->curr->data;
	L->curr = L->curr->next;
	return mesg;
}

// check whether MMList scan is complete
bool MMListEnd (MMList L)
{
	assert (L != NULL);
	return (L->curr == NULL);
}
