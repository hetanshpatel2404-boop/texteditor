#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <time.h>

typedef struct {
    int id;
    char* text;
    time_t timestamp;
} ClipEntry;

typedef struct {
    ClipEntry* items;
    int capacity;
    int size;
    int head;
    int tail;
    int next_id;
} ClipboardHistory;

ClipboardHistory* clipboard_create(int capacity);
void clipboard_destroy(ClipboardHistory* history);
bool clipboard_enqueue(ClipboardHistory* history, const char* text);
ClipEntry* clipboard_dequeue(ClipboardHistory* history);
ClipEntry* clipboard_latest(ClipboardHistory* history);
ClipEntry* clipboard_get(ClipboardHistory* history, int index_from_newest);
void clipboard_clear(ClipboardHistory* history);

#endif
