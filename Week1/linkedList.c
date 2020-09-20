#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "helper.h"

node *makeNewNode(elementtype element)
{
	node *new = (node *)malloc(sizeof(node));
	new->element = element;
	new->next = NULL;
	return new;
}

void displayNode(node *p)
{
	elementtype tmp = p->element;
	printf("%s\t%s\t%d\n", tmp.username, tmp.password, tmp.status);
}

void insertAtHead(elementtype element)
{
	node *new = makeNewNode(element);
	new->next = root;
	root = new;
	cur = root;
}

void insertAfter(elementtype element)
{
	node *new = makeNewNode(element);
	if (cur == NULL)
		return;
	if (root == NULL)
	{
		root = new;
		cur = root;
	}
	else
	{
		new->next = cur->next;
		cur->next = new;
		// prev = cur;
		cur = cur->next;
	}
}

void insertBefore(elementtype element)
{
	node *new = makeNewNode(element);
	node *tmp = root;
	while (tmp != NULL && tmp->next != cur && cur != NULL)
	{
		tmp = tmp->next;
	}
	prev = tmp;
	if (root == NULL)
	{
		root = new;
		cur = root;
		prev = NULL;
	}
	else
	{
		new->next = cur;
		if (cur == root)
		{
			root = new;
		}
		else
			prev->next = new;
		cur = new;
	}
}

void deleteFirst()
{
	node *del = root;
	if (del == NULL)
		return;
	root = del->next;
	free(del);
	cur = root;
}

void deleteNode()
{
	node *tmp = root;
	while (tmp != NULL && tmp->next != cur && cur != NULL)
	{
		tmp = tmp->next;
	}
	prev = tmp;
	if (cur == NULL)
		return;
	if (cur == root)
		deleteFirst();
	else
	{
		prev->next = cur->next;
		free(cur);
		cur = prev->next;
	}
}

node *find(char *username)
{
	node *p;
	for (p = root; p != NULL; p = p->next)
	{
		if (strcmp(p->element.username, username) == 0)
		{
			return p;
		}
	}
	return NULL;
}

node *reverseList(node *root)
{
	node *cur, *prev;
	cur = prev = NULL;
	while (root != NULL)
	{
		cur = root;
		root = root->next;
		cur->next = prev;
		prev = cur;
	}
	return prev;
}

void freeList()
{
	node *toFree;
	while (toFree != NULL)
	{
		root = root->next;
		free(toFree);
		toFree = root;
	}
}