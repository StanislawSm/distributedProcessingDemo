#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tree {
	int element;
	struct tree* left;
	struct tree* right;
};

struct element {
	struct element* previous;
	int number;
	struct element* next;
};

struct tree* createTreeNode(int n) {
	struct tree* result = (struct tree*)malloc(sizeof(struct tree));
	result->element = n;
	result->left = NULL;
	result->right = NULL;
	return result;
}

void addToTree(struct tree *root, int n) {
	static int side;
	if (root->left == NULL) {
		root->left = createTreeNode(n);
	}
	else if (root->right == NULL) {
		root->right = createTreeNode(n);
	}
	else
	{
		if (side % 2 == 0) {
			addToTree(root->left, n);
			side++;
		}
		else
		{
			addToTree(root->right, n);
			side++;
		}
	}
}

void printTree(struct tree *root) {
	if (root != NULL)
	{
		printf("\ntree: %d", root->element);
		printTree(root->left);
		printTree(root->right);
	}
}

void freeTree(struct tree *root) {
	if (root != NULL)
	{
		freeTree(root->left);
		freeTree(root->right);
		free(root);
	}
}

void insert(int n, struct element* node) {
	if (node->next == NULL) {
		struct element* temp = (struct element*)malloc(sizeof(struct element));
		temp->next = NULL;
		temp->number = n;
		temp->previous = node;
		node->next = temp;
	}
	else
	{
		insert(n, node->next);
	}
}

void printList(struct element* node) {
	if (node != NULL) {
		printf("\nList: %d", node->number);
		printList(node->next);
	}
}

void freeList(struct element* node) {
	if (node != NULL) {
		freeList(node->next);
		free(node);
	}	
}

int main() {
	struct tree *root = createTreeNode(5);
	
	struct element* listRoot;
	listRoot = (struct element*)malloc(sizeof(struct element));
	listRoot->next = NULL;
	listRoot->previous = NULL;
	listRoot->number = 10;

	char inputC;
	int inputI;
	scanf("%c", &inputC);
	while (inputC != 'q') {
		scanf("%c", &inputC);
		scanf("%d", &inputI);
		switch (inputC) {
		case 't':
			addToTree(root, inputI);
			break;
		case 'l':
			insert(inputI, listRoot);
			break;
		case 'p':
			switch (inputI){
			case 1:
				printList(listRoot);
				break;
			case 2:
				printTree(root);
			}
			break;
		}
	}
	

	freeTree(root);
	freeList(listRoot);

	return 0;
}