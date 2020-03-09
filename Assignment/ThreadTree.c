// ThreadTree.c ... implementation of Tree-of-Mail-Threads ADT
// Written by John Shepherd, Feb 2019

#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

#include "MMList.h"
#include "MMTree.h"
#include "MailMessage.h"
#include "ThreadTree.h"

// Representation of ThreadTree's

typedef struct ThreadTreeNode *Link;

static void fillLeftHandSide(MMList mesgs, MMTree msgids, ThreadTree newThread);
static Link insertIntoThreadTree(Link treeRoot, Link node);
static Link newThreadTreeNode(MailMessage mesg);
static bool inTree(Link treeRoot, MailMessage mesg);

void dropThreadTreeNode(Link node);

typedef struct ThreadTreeNode {
	MailMessage mesg;
	Link next, replies;
} ThreadTreeNode;

typedef struct ThreadTreeRep {
	Link messages;
} ThreadTreeRep;

// Auxiliary data structures and functions

// Add any new data structures and functions here ...

static void doDropThreadTree (Link t);
static void doShowThreadTree (Link t, int level);

// END auxiliary data structures and functions

// create a new empty ThreadTree
ThreadTree newThreadTree (void)
{
	ThreadTreeRep *new = malloc (sizeof *new);
	if (new == NULL) err (EX_OSERR, "couldn't allocate ThreadTree");
	*new = (ThreadTreeRep) { };
	return new;
}

void dropThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doDropThreadTree (tt->messages);
}

// free up memory associated with list
static void doDropThreadTree (Link t)
{
	if (t == NULL)
		return;

	for (Link curr = t, next; curr != NULL; curr = next) {
		next = curr->next;
		doDropThreadTree (curr->replies);
		// don't drop curr->mesg, in case referenced elsehwere
		free (curr);
	}
}

void showThreadTree (ThreadTree tt)
{
	assert (tt != NULL);
	doShowThreadTree (tt->messages, 0);
}

// display thread tree as hiearchical list
static void doShowThreadTree (Link t, int level)
{
	if (t == NULL)
		return;
	for (Link curr = t; curr != NULL; curr = curr->next) {
		showMailMessage (curr->mesg, level);
		doShowThreadTree (curr->replies, level + 1);
	}
}

// insert mail message into ThreadTree
// if a reply, insert in appropriate replies list
// whichever list inserted, must be in timestamp-order
ThreadTree ThreadTreeBuild (MMList mesgs, MMTree msgids)
{

    if (mesgs == NULL && msgids == NULL) return NULL;
    
    ThreadTree newThread = newThreadTree();
    
    fillLeftHandSide(mesgs,msgids,newThread);
    
    Link node = newThreadTreeNode(NULL);
    
    MMListStart(mesgs);
    
    while(MMListEnd(mesgs) != 1){
        
        MailMessage mail = MMListNext(mesgs);
        
        node->mesg = mail;
        
        if(inTree(newThread->messages,mail) == 0){
            
            //free(node);
            
            printf("%s Not in tree\n",MailMessageID(mail));
            
        }
    }
    
	return newThread;
}


static void fillLeftHandSide(MMList mesgs, MMTree msgids, ThreadTree newThread){
    
    Link lastLeft = NULL;
    
    MMListStart(mesgs);
    
    while(MMListEnd(mesgs) != 1){
        
        MailMessage newMail = MMListNext(mesgs);
        
        if(MailMessageRepliesTo(newMail) == NULL || MMTreeFind(msgids,MailMessageRepliesTo(newMail)) == NULL){
  
            Link newNode = newThreadTreeNode(newMail);
            
            if(newThread->messages == NULL){
                
                newThread->messages = newNode;
                lastLeft = newThread->messages;
                
            } else {
                
                lastLeft->next = newNode;
                lastLeft = lastLeft->next;
            }
        }
    }
}

static bool inTree(Link treeRoot, MailMessage mesg){
    
    if(strcmp(MailMessageID(treeRoot->mesg),MailMessageID(mesg)) == 0) {
        
        return 1;
        
    } else {
        
        if(treeRoot->next != NULL){
            
            return inTree(treeRoot->next,mesg);
        }
        
        if(treeRoot->replies != NULL) {
            
            return inTree(treeRoot->replies,mesg);
        }
    }
    return 0;
}

static Link newThreadTreeNode(MailMessage mesg) {

    Link new = malloc (sizeof (ThreadTreeNode));
    assert (new != NULL);
    new->mesg = mesg;
    new->next = NULL;
    new->replies = NULL;
    return new;
}

void dropThreadTreeNode(Link node) {
    if (node == NULL) return;
    free(node);
}















//static Link insertIntoThreadTree(ThreadTreeNode *treeRoot,ThreadTreeNode *node){
//
//    if (treeRoot == NULL || node == NULL) return treeRoot;
//
//    if(strcmp(MailMessageRepliesTo(node->mesg),MailMessageID(treeRoot->mesg)) != 0){
//
//
//        if(treeRoot->next != NULL) insertIntoThreadTree(treeRoot->next,node);
//        if(treeRoot->replies != NULL) insertIntoThreadTree(treeRoot->replies,node);
//
//
//    } else {
//
//        node->next = treeRoot->replies;
//        treeRoot->replies = node->next;
//        return node;
//    }
//       return NULL;
//}
