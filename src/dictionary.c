#include <string.h>

#include "dictionary.h"
#include "error_handler.h"

node *dictionary_create()
{
    node *dict = malloc(sizeof(node));
    if (!dict)
        ERR("malloc");
    // At first root does not have value inserted
    dict->word = NULL;
    dict->color = BLACK;
    dict->left = NULL;
    dict->right = NULL;
    dict->parent = NULL;
    return dict;
}

node *dictionary_search(node *dictionary, char *word)
{
    while (dictionary && dictionary->word)
    {
        int comparison = strcmp(word, dictionary->word);
        if (comparison == 0)
            break;
        if (comparison < 0)
            dictionary = dictionary->left;
        else
            dictionary = dictionary->right;
    }
    if (dictionary->word)
        return dictionary;
    return NULL;
}

void dictionary_insert(node *dictionary, char *word)
{
    // element is already in the dictionary
    if (dictionary_search(dictionary, word))
        return;
    // it's first element to be insterted - so it will be in the root
    if (!dictionary->word)
    {
        dictionary->word = malloc(sizeof(char) * (strlen(word) + 1));
        if (!dictionary->word)
            ERR("malloc");
        strcpy(dictionary->word, word);
        return;
    }
    // it's elment outside the root
    node *new_element = malloc(sizeof(node));
    if (!new_element)
        ERR("malloc");
    new_element->word = malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(new_element->word, word);
    new_element->left = NULL;
    new_element->right = NULL;
    new_element->color = RED;

    node *prev = NULL;
    int comparison = 0;
    while (dictionary)
    {
        if (is_four_node(dictionary))
            split_four_node(dictionary);
        prev = dictionary;
        comparison = strcmp(word, dictionary->word);
        if (comparison < 0)
            dictionary = dictionary->left;
        else
            dictionary = dictionary->right;
    }

    if (comparison < 0)
    {
        prev->left = dictionary;
    }
    else
    {
        prev->right = dictionary;
    }
    dictionary->parent = prev;
}

node *dictionary_delete(node *dictionary, char *word)
{
    ERR("TODO");
    return NULL;
}

void dictionary_destroy(node *dictionary)
{
    dictionary_destroy(dictionary->left);
    dictionary_destroy(dictionary->right);
    free(dictionary->word);
    free(dictionary);
}

int is_four_node(node *p)
{
    return p && p->color == BLACK && p->left && p->left->color == RED && p->right && p->right->color == RED;
}

void split_four_node(node *p)
{
    ERR("TODO");
}
