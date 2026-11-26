#define _POSIX_C_SOURCE 200809L
#include "../include/trie.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static TrieNode* trie_node_create(void) {
    return calloc(1, sizeof(TrieNode));
}

static void trie_destroy_node(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) trie_destroy_node(node->children[i]);
    }
    free(node->kind);
    free(node);
}

Trie* trie_create(void) {
    Trie* trie = malloc(sizeof(Trie));
    trie->root = trie_node_create();
    trie->node_count = 1;
    trie->word_count = 0;
    return trie;
}

void trie_destroy(Trie* trie) {
    if (!trie) return;
    trie_destroy_node(trie->root);
    free(trie);
}

bool trie_insert(Trie* trie, const char* word, int weight, const char* kind, bool only_if_new, bool bump) {
    if (!trie || !word) return false;
    int len = strlen(word);
    if (len == 0 || len > MAX_WORD_LENGTH) return false;

    TrieNode* node = trie->root;
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)word[i];
        if (!node->children[c]) {
            node->children[c] = trie_node_create();
            trie->node_count++;
        }
        node = node->children[c];
    }

    bool is_new = !node->terminal;
    if (is_new) {
        node->terminal = true;
        trie->word_count++;
    }
    if (only_if_new && !is_new) return false;

    if (bump) node->weight += (weight > 0 ? weight : 0);
    else if (weight > node->weight) node->weight = weight;

    if (kind) {
        free(node->kind);
        node->kind = strdup(kind);
    }
    return is_new;
}

bool trie_contains(Trie* trie, const char* word) {
    if (!trie || !word) return false;
    TrieNode* node = trie->root;
    for (int i = 0; word[i] != '\0'; i++) {
        unsigned char c = (unsigned char)word[i];
        if (!node->children[c]) return false;
        node = node->children[c];
    }
    return node->terminal;
}

static int compare_completions(const void* a, const void* b) {
    const CompletionResult* ca = (const CompletionResult*)a;
    const CompletionResult* cb = (const CompletionResult*)b;
    if (cb->weight != ca->weight) return cb->weight - ca->weight;
    int len_diff = strlen(ca->word) - strlen(cb->word);
    if (len_diff != 0) return len_diff;
    return strcasecmp(ca->word, cb->word);
}

static void collect_completions(TrieNode* node, char* prefix, int depth, CompletionList* list) {
    if (!node || !list || list->count >= list->capacity) return;
    if (node->terminal) {
        strncpy(list->results[list->count].word, prefix, MAX_WORD_LENGTH);
        list->results[list->count].weight = node->weight;
        strncpy(list->results[list->count].kind, node->kind ? node->kind : "word", 31);
        list->count++;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            prefix[depth] = (char)i;
            prefix[depth + 1] = '\0';
            collect_completions(node->children[i], prefix, depth + 1, list);
        }
    }
}

CompletionList* trie_complete(Trie* trie, const char* prefix, int limit) {
    if (!trie || !prefix || limit <= 0) return NULL;
    TrieNode* node = trie->root;
    int len = strlen(prefix);
    for (int i = 0; i < len; i++) {
        unsigned char c = (unsigned char)prefix[i];
        if (!node->children[c]) return NULL;
        node = node->children[c];
    }

    CompletionList* list = malloc(sizeof(CompletionList));
    list->capacity = limit;
    list->count = 0;
    list->results = calloc(limit, sizeof(CompletionResult));

    char current_prefix[MAX_WORD_LENGTH + 2];
    strncpy(current_prefix, prefix, MAX_WORD_LENGTH);
    current_prefix[MAX_WORD_LENGTH] = '\0';

    collect_completions(node, current_prefix, len, list);
    qsort(list->results, list->count, sizeof(CompletionResult), compare_completions);
    return list;
}

void completion_list_destroy(CompletionList* list) {
    if (!list) return;
    free(list->results);
    free(list);
}
