// Abstract data structure dictionary implemented using Red-Black Tree
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>

// Dictionary structures
enum rb_node_color
{
    RED,
    BLACK
};
typedef struct __rb_node
{
    char *word;
    struct __rb_node *left, *right, *parent;
    enum rb_node_color color;
} rb_node;

typedef struct __rb_tree
{
    rb_node *root;
} rb_tree;

// Dictionary interface
rb_tree *dictionary_create();
rb_node *dictionary_search(rb_tree *dictionary, char *word, rb_node **prev);
void dictionary_insert(rb_tree *dictionary, char *word);
rb_node *dictionary_delete(rb_tree *dictionary, char *word);
void rb_node_destroy(rb_node *node);
void dictionary_destroy(rb_tree *dictionary);

#endif // DICTIONARY_H