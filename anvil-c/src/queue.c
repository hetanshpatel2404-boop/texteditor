#define _POSIX_C_SOURCE 200809L
#include "../include/queue.h"
#include <stdlib.h>
#include <string.h>

ClipboardHistory* clipboard_create(int capacity) {
    if (capacity < 1) capacity = 50;
    ClipboardHistory* history = calloc(1, sizeof(ClipboardHistory));
    history->items = calloc(capacity, sizeof(ClipEntry));
    history->capacity = capacity;
    history->next_id = 1;
    return history;
}

void clipboard_destroy(ClipboardHistory* history) {
    if (!history) return;
    for (int i = 0; i < history->capacity; i++) free(history->items[i].text);
    free(history->items);
    free(history);
}

bool clipboard_enqueue(ClipboardHistory* history, const char* text) {
    if (!history || !text || strlen(text) == 0) return false;

    if (history->size > 0) {
        int last_idx = (history->tail - 1 + history->capacity) % history->capacity;
        if (strcmp(history->items[last_idx].text, text) == 0) {
            history->items[last_idx].timestamp = time(NULL);
            return true;
        }
    }

    if (history->size == history->capacity) {
        free(history->items[history->head].text);
        history->head = (history->head + 1) % history->capacity;
    } else {
        history->size++;
    }

    ClipEntry* entry = &history->items[history->tail];
    entry->id = history->next_id++;
    entry->text = strdup(text);
    entry->timestamp = time(NULL);
    history->tail = (history->tail + 1) % history->capacity;
    return true;
}

ClipEntry* clipboard_dequeue(ClipboardHistory* history) {
    if (!history || history->size == 0) return NULL;
    ClipEntry* entry = &history->items[history->head];
    history->head = (history->head + 1) % history->capacity;
    history->size--;
    return entry;
}

ClipEntry* clipboard_latest(ClipboardHistory* history) {
    if (!history || history->size == 0) return NULL;
    int idx = (history->tail - 1 + history->capacity) % history->capacity;
    return &history->items[idx];
}

ClipEntry* clipboard_get(ClipboardHistory* history, int index_from_newest) {
    if (!history || index_from_newest < 0 || index_from_newest >= history->size) return NULL;
    int idx = (history->tail - 1 - index_from_newest + history->capacity) % history->capacity;
    return &history->items[idx];
}

void clipboard_clear(ClipboardHistory* history) {
    if (!history) return;
    for (int i = 0; i < history->capacity; i++) {
        free(history->items[i].text);
        history->items[i].text = NULL;
    }
    history->size = 0;
    history->head = 0;
    history->tail = 0;
    history->next_id = 1;
}
