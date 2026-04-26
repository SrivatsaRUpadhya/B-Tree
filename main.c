#include <assert.h>
#include <math.h>
#include<stdio.h>
#include<stdbool.h>
#include <stdlib.h>

#define ORDER 3
#define ELEMENTS_PER_NODE ORDER - 1
typedef struct Node{
	int keys[ELEMENTS_PER_NODE];
	int element_count;
	struct Node *child[ORDER];
	struct Node* parent;
}tree_node;

tree_node* create_root(int val);
tree_node *create_node();
tree_node* find_insert_loc(tree_node *root);
void insert(tree_node *root, int val);
bool needs_balance(tree_node *node);
tree_node *balance(tree_node *node);
void util_add_key(tree_node *node, int key);

int main(){
	int insert_list[] = {1,2,3,4,5,6,7};
	tree_node *root = NULL;
	for(int i = 0; i < 7; i++){
		insert(root, insert_list[i]);
	}
	return 0;
}

void insert(tree_node *root, int val){
	if(root == NULL){
		create_root(val);
		return;
	}
	tree_node *insert_loc = find_insert_loc(root);
	if(needs_balance(insert_loc)){
		balance(insert_loc);
	}
}

bool needs_balance(tree_node *node){
	return node->element_count == ELEMENTS_PER_NODE;
}

tree_node *balance(tree_node *node){
	/* 
	 * find median
	 * if parent is null(root node)
	 	create a new node and move median to this new node
		move elements less than median to child[element_count-1] of new root
		copy the child[0] to child[median] to less_than_median of root and child[median + 1] to remaining to more_than_median 
		return child[element_count] as insert_loc to the previous balance call

	 * if parent can hold more keys 
		move the median to parent
		move elements to left of median to child[element_count - 1] and elements to the right to child[element_count + 1]
		return child[element_count] as new insert_loc
	
	* if parent cannot hold more keys
		call balance on parent
		move elements to left of median to child[element_count - 1] and elements to the right to child[element_count + 1]
		return child[element_count] as new insert_loc
	*/
	assert(node->element_count == ELEMENTS_PER_NODE);

	int median = ceil(node->element_count / 2.0) - 1;
	if(node->parent == NULL){
		create_root(node->keys[median]);
		tree_node *less_than_median = node->child[node->element_count - 1] ;
		// TODO: Maybe use the current node itself as less_than_median node and skip malloc and freeing of current node memory
		less_than_median = create_node();
		for(int i = 0; i < median; i++){
			util_add_key(less_than_median, node->keys[i]);
		}
		tree_node *more_than_median = node->child[node->element_count + 1] ;
		more_than_median = create_node();
		for(int i = median + 1; i < node->element_count; i++){
			util_add_key(more_than_median, node->keys[i]);
		}
		for(int i = 0; i <= median + 1; i++){
			less_than_median->child[i] = node->child[i];
		}
		for(int j = 0,i = median + 2; i <= node->element_count && j < more_than_median->element_count + 1; i++){
			more_than_median->child[j] = node->child[i];
		}
		free(node);
		return more_than_median;
	}

	tree_node *parent;
	if(!needs_balance(node->parent)){
		parent = node->parent;
	}
	else{
		tree_node *parent= balance(node->parent);
	}

	util_add_key(parent,node->keys[median]);
	tree_node *less_than_median = node->child[node->element_count - 1] ;
	// TODO: Maybe use the current node itself as less_than_median node and skip malloc and freeing of current node memory
	less_than_median = create_node();
	for(int i = 0; i < median; i++){
		util_add_key(less_than_median, node->keys[i]);
	}
	tree_node *more_than_median = node->child[node->element_count] ;
	more_than_median = create_node();
	for(int i = median + 1; i < node->element_count; i++){
		util_add_key(more_than_median, node->keys[i]);
	}
	parent->child[parent->element_count - 1] = less_than_median;
	parent->child[parent->element_count - 1] = more_than_median;
	free(node);
	return more_than_median;
}

// add key to node and increment element count
void util_add_key(tree_node *node, int key){
	assert(node->element_count < ELEMENTS_PER_NODE);
	int idx = node->element_count;
	node->keys[idx] = key;
	node->element_count++;
}

tree_node *create_node(){
	return (tree_node*) malloc(sizeof(tree_node));
}
