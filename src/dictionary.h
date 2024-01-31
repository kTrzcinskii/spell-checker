// Abstract data structure dictionary implemented using Red-Black Tree
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdlib.h>

// Dictionary structures
enum node_color
{
    RED,
    BLACK
};
typedef struct __node
{
    char *word;
    struct __node *left, *right;
    enum node_color color;
} node;

// Dictionary interface
node *dictionary_create();
node *dictionary_search(node *dictionary, char *word);
void dictionary_insert(node *dictionary, char *word);
node *dictionary_delete(node *dictionary, char *word);
void dictionary_destroy(node *dictionary);

// Dictionary helpers
int is_four_node(node *p);
void split_four_node(node *p, node *prev, node *pprev);

#endif // DICTIONARY_H