#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ORDER 3
#define ELEMENTS_PER_NODE ORDER - 1
typedef struct Node {
  int keys[ELEMENTS_PER_NODE];
  int element_count;
  struct Node *child[ORDER];
  struct Node *parent;
} tree_node;

tree_node *create_root(int val);
tree_node *create_node();
tree_node *find_insert_loc(tree_node *root, int val);
void insert(tree_node **root, int val);
bool needs_balance(tree_node *node);
tree_node *balance(tree_node **node);
void util_add_key(tree_node *node, int key);
bool util_node_has_children(tree_node *node);
void util_print_node_keys(tree_node *node);
void traverse_tree(tree_node *root);
void assign_parent(tree_node *parent, tree_node *child);

int main() {
  int insert_list[] = {1, 2, 3, 4, 5, 6, 7};
  tree_node *root = NULL;
  for (int i = 0; i < 7; i++) {
    printf("inserting %d\n", insert_list[i]);
    insert(&root, insert_list[i]);
  }
  traverse_tree(root);
  return 0;
}

void insert(tree_node **root, int val) {
  if (*root == NULL) {
    *root = create_root(val);
    return;
  }
  tree_node *insert_loc = find_insert_loc(*root, val);
  if (needs_balance(insert_loc)) {
    tree_node *new_root = balance(&insert_loc);
    if (new_root != NULL)
      *root = new_root;
  }
  util_add_key(insert_loc, val);
}

bool needs_balance(tree_node *node) {
  return node->element_count == ELEMENTS_PER_NODE;
}

tree_node *balance(tree_node **node_ptr) {
  /*
   * find median
   * if parent is null(root node)
          create a new node and move median to this new node
          move elements less than median to child[element_count-1] of new root
          copy the child[0] to child[median] to less_than_median of root and
  child[median + 1] to remaining to more_than_median return
  child[element_count] as insert_loc to the previous balance call

   * if parent can hold more keys
          move the median to parent
          move elements to left of median to child[element_count - 1] and
  elements to the right to child[element_count + 1] return
  child[element_count] as new insert_loc

  * if parent cannot hold more keys
          call balance on parent
          move elements to left of median to child[element_count - 1] and
  elements to the right to child[element_count + 1] return
  child[element_count] as new insert_loc
  */
  tree_node *node = *node_ptr;
  assert(node->element_count == ELEMENTS_PER_NODE);

  int median = ceil(node->element_count / 2.0);
  if (node->parent == NULL) {
    tree_node *new_root = create_root(node->keys[median]);
    tree_node *less_than_median = node->child[node->element_count - 1];
    // TODO: Maybe use the current node itself as less_than_median node and
    // skip malloc and freeing of current node memory
    less_than_median = create_node();
    assign_parent(new_root, less_than_median);
    for (int i = 0; i < median; i++) {
      util_add_key(less_than_median, node->keys[i]);
    }
    tree_node *more_than_median = node->child[node->element_count + 1];
    more_than_median = create_node();
    assign_parent(new_root, more_than_median);
    for (int i = median + 1; i < node->element_count; i++) {
      util_add_key(more_than_median, node->keys[i]);
    }
    for (int i = 0; i <= median + 1; i++) {
      less_than_median->child[i] = node->child[i];
    }
    for (int j = 0, i = median + 2;
         i <= node->element_count && j < more_than_median->element_count + 1;
         i++) {
      more_than_median->child[j] = node->child[i];
    }
    new_root->child[new_root->element_count - 1] = less_than_median;
    new_root->child[new_root->element_count] = more_than_median;
    free(node);
    *node_ptr = more_than_median;
    return new_root;
  }

  tree_node *parent = node->parent;
  tree_node *new_root = NULL;
  if (!needs_balance(node->parent)) {
    parent = node->parent;
    util_add_key(parent, node->keys[median]);
    // TODO: Maybe use the current node itself as less_than_median node and skip
    // malloc and freeing of current node memory
    tree_node *less_than_median = create_node();
    assign_parent(parent, less_than_median);
    for (int i = 0; i < median; i++) {
      util_add_key(less_than_median, node->keys[i]);
    }
    tree_node *greater_than_median = create_node();
    assign_parent(parent, greater_than_median);
    for (int i = median + 1; i < node->element_count; i++) {
      util_add_key(greater_than_median, node->keys[i]);
    }
    parent->child[parent->element_count - 1] = less_than_median;
    parent->child[parent->element_count] = greater_than_median;
    free(node);
    *node_ptr = greater_than_median;
    return NULL;
  } else {
    tree_node *new_root = balance(&parent);
    // TODO: Maybe use the current node itself as less_than_median node and skip
    // malloc and freeing of current node memory
    util_add_key(parent, node->keys[median]);
    tree_node *less_than_median = create_node();
    assign_parent(parent, less_than_median);
    for (int i = 0; i < median; i++) {
      util_add_key(less_than_median, node->keys[i]);
    }
    tree_node *greater_than_median = create_node();
    assign_parent(parent, greater_than_median);
    for (int i = median + 1; i < node->element_count; i++) {
      util_add_key(greater_than_median, node->keys[i]);
    }
    parent->child[parent->element_count - 1] = less_than_median;
    parent->child[parent->element_count] = greater_than_median;
    free(node);
    *node_ptr = greater_than_median;
    return new_root;
  }
}

tree_node *find_insert_loc(tree_node *root, int val) {
  int key_idx = 0;
  if (!util_node_has_children(root)) {
    return root;
  }
  while (key_idx < (root->element_count) && val >= (root->keys[key_idx]) &&
         (root->child[key_idx] != NULL)) {
    key_idx++;
  }
  return find_insert_loc(root->child[key_idx], val);
}

tree_node *create_node() {
  tree_node *node = (tree_node *)malloc(sizeof(tree_node));
  for (int i = 0; i < ORDER; i++) {
    node->child[i] = NULL;
  }
  for (int i = 0; i < ELEMENTS_PER_NODE; i++) {
    node->keys[i] = INT_MAX;
  }
  node->element_count = 0;
  node->parent = NULL;
  return node;
}
tree_node *create_root(int val) {
  tree_node *node = create_node();
  util_add_key(node, val);
  return node;
}

void traverse_tree(tree_node *root) {
  if (root == NULL)
    return;

  util_print_node_keys(root);
  for (int i = 0; i <= root->element_count; i++) {
    traverse_tree(root->child[i]);
  }
}

// add key to node and increment element count
void util_add_key(tree_node *node, int key) {
  assert(node->element_count < ELEMENTS_PER_NODE);
  int idx = node->element_count;
  node->keys[idx] = key;
  node->element_count++;
}

bool util_node_has_children(tree_node *node) {
  for (int i = 0; i < ORDER; i++) {
    if (node->child[i] != NULL)
      return true;
  }
  return false;
}

void assign_parent(tree_node *parent, tree_node *child) {
  child->parent = parent;
}

void util_print_node_keys(tree_node *node) {
  for (int i = 0; i < node->element_count; i++)
    printf("%d,", node->keys[i]);
}
