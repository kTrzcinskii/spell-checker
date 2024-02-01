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
    {
        free(root);
        ERR("malloc", GENERAL_ERROR);
    }
    dict->root = root;
    return dict;
}

rb_node *dictionary_search(rb_tree *dictionary, char *word, rb_node **prev)
{
    rb_node *p = dictionary->root;
    if (prev)
        *prev = NULL;
    while (p && p->word)
    {
        int comparison = strcmp(word, p->word);
        if (comparison == 0)
            break;
        if (prev)
            *prev = p;
        if (comparison < 0)
            p = p->left;
        else
            p = p->right;
    }
    if (p && p->word)
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
        {
            dictionary_destroy(dictionary);
            ERR("malloc", GENERAL_ERROR);
        }
        strcpy(p->word, word);
        return;
    }

    // it's elment outside the root
    // create new element
    rb_node *new_element = malloc(sizeof(rb_node));
    if (!new_element)
    {
        dictionary_destroy(dictionary);
        ERR("malloc", GENERAL_ERROR);
    }
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

    // fix rb tree structure
    dictionary_fix_structure(dictionary, new_element);
}

void dictionary_load_from_file(rb_tree *dictionary, char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        dictionary_destroy(dictionary);
        ERR("Couldn't open file", FILE_ERROR);
    }

    char *line = NULL;
    size_t line_length = 0;
    ssize_t read = 0;

    while ((read = getline(&line, &line_length, file)) != -1)
    {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        dictionary_insert(dictionary, line);
    }

    free(line);

    if (fclose(file))
    {
        dictionary_destroy(dictionary);
        ERR("Couldn't close file", FILE_ERROR);
    }
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

void dictionary_fix_structure(rb_tree *dictionary, rb_node *node)
{
    rb_node *parent = NULL;
    rb_node *grandparent = NULL;
    rb_node *uncle = NULL;
    rb_node *root = dictionary->root;

    while ((node != root) && IS_RED(node) && IS_RED(node->parent))
    {
        parent = node->parent;
        grandparent = parent->parent;

        // Parent of node is left child of grandparent
        if (parent == grandparent->left)
        {
            uncle = grandparent->right;
            // Uncle is red
            if (IS_RED(uncle))
            {
                parent->color = BLACK;
                grandparent->right->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            }
            // Uncle is black or doesnt exist
            else
            {
                // node is left child of parent
                if (parent->right == node)
                {
                    left_rotate(dictionary, parent);
                    node = parent;
                    parent = node->parent;
                }
                // node is right child of parent (we dont put it in else, as it's also next step of above case)
                right_rotate(dictionary, grandparent);
                swap_colors((int *)&parent->color, (int *)&grandparent->color);
                node = parent;
            }
        }
        // Parent of node is right child of grandparent
        else
        {
            uncle = grandparent->left;
            // Uncle is red
            if (IS_RED(uncle))
            {
                parent->color = BLACK;
                grandparent->right->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            }
            // Uncle is black or doesnt exist
            else
            {
                // node is right child of parent
                if (parent->left == node)
                {
                    right_rotate(dictionary, parent);
                    node = parent;
                    parent = node->parent;
                }
                // node is left child of parent (we dont put it in else, as it's also next step of above case)
                left_rotate(dictionary, grandparent);
                swap_colors((int *)&parent->color, (int *)&grandparent->color);
                node = grandparent;
            }
        }
    }

    if (IS_RED(dictionary->root))
        dictionary->root->color = BLACK;
}

void left_rotate(rb_tree *dictionary, rb_node *node)
{
    rb_node *right = node->right;
    node->right = right->left;
    if (node->right)
        node->right->parent = node;
    right->parent = node->parent;
    if (!right->parent)
        dictionary->root = right;
    else if (node == node->parent->left)
        node->parent->left = right;
    else
        node->parent->right = right;
    right->left = node;
    node->parent = right;
}

void right_rotate(rb_tree *dictionary, rb_node *node)
{
    rb_node *left = node->left;
    node->left = left->right;
    if (node->left)
        node->left->parent = node;
    left->parent = node->parent;
    if (!left->parent)
        dictionary->root = left;
    else if (node == node->parent->left)
        node->parent->left = left;
    else
        node->parent->right = left;
    left->right = node;
    node->parent = left;
}

void swap_colors(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
