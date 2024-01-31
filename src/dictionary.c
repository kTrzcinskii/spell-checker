#include <string.h>

#include "dictionary.h"
#include "error_handler.h"

rb_tree *dictionary_create()
{
    rb_node *root = malloc(sizeof(rb_node));
    if (!root)
        ERR("malloc", GENERAL_ERROR);
    // At first root does not have value inserted
    root->word = NULL;
    root->color = BLACK;
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
    rb_tree *dict = malloc(sizeof(rb_tree));
    if (!dict)
        ERR("malloc", GENERAL_ERROR);
    dict->root = root;
    return dict;
}

rb_node *dictionary_search(rb_tree *dictionary, char *word, rb_node **prev)
{
    rb_node *p = dictionary->root;
    *prev = NULL;
    while (p && p->word)
    {
        int comparison = strcmp(word, p->word);
        if (comparison == 0)
            break;
        *prev = p;
        if (comparison < 0)
            p = p->left;
        else
            p = p->right;
    }
    if (p->word)
        return p;
    return NULL;
}

void dictionary_insert(rb_tree *dictionary, char *word)
{
    rb_node *parent = NULL;
    // element is already in the dictionary
    if (dictionary_search(dictionary, word, &parent))
        return;

    rb_node *p = dictionary->root;
    // it's first element to be insterted - so it will be in the root
    if (!p->word)
    {
        p->word = malloc(sizeof(char) * (strlen(word) + 1));
        if (!p->word)
            ERR("malloc", GENERAL_ERROR);
        strcpy(p->word, word);
        return;
    }

    // it's elment outside the root
    // create new element
    rb_node *new_element = malloc(sizeof(rb_node));
    if (!new_element)
        ERR("malloc", GENERAL_ERROR);
    new_element->word = malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(new_element->word, word);
    new_element->left = NULL;
    new_element->right = NULL;
    new_element->color = RED;
    new_element->parent = parent;
    if (strcmp(new_element->word, parent->word) < 0)
        parent->left = new_element;
    else
        parent->right = new_element;

    // fix structure of dictionary
    do
    {
        ERR("TODO", NOT_IMPLEMENTED);
    } while (1);
}

rb_node *dictionary_delete(rb_tree *dictionary, char *word)
{
    ERR("TODO", NOT_IMPLEMENTED);
    return NULL;
}

void rb_node_destroy(rb_node *node)
{
    if (!node)
        return;
    rb_node_destroy(node->left);
    rb_node_destroy(node->right);
    free(node->word);
    free(node);
}

void dictionary_destroy(rb_tree *dictionary)
{
    rb_node_destroy(dictionary->root);
    free(dictionary);
}