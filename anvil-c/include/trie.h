#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define MAX_WORD_LENGTH 64
#define ALPHABET_SIZE 256

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int weight;
    char* kind;
    bool terminal;
} TrieNode;

typedef struct {
    TrieNode* root;
    int node_count;
    int word_count;
} Trie;

typedef struct {
    char word[MAX_WORD_LENGTH + 1];
    int weight;
    char kind[32];
} CompletionResult;

typedef struct {
    CompletionResult* results;
    int count;
    int capacity;
} CompletionList;

Trie* trie_create(void);
void trie_destroy(Trie* trie);
bool trie_insert(Trie* trie, const char* word, int weight, const char* kind, bool only_if_new, bool bump);
bool trie_contains(Trie* trie, const char* word);
CompletionList* trie_complete(Trie* trie, const char* prefix, int limit);
void completion_list_destroy(CompletionList* list);

#endif
