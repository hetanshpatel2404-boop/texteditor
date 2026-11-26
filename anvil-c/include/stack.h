#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <time.h>

#define MAX_DIFF_LEN 400
#define MAX_LABEL_LEN 32

typedef struct {
    char* text;
    int cursor;
    char label[MAX_LABEL_LEN];
    char kind[MAX_LABEL_LEN];
    time_t timestamp;
    int position;
    char inserted[MAX_DIFF_LEN + 1];
    char deleted[MAX_DIFF_LEN + 1];
    int edit_count;
    time_t last_timestamp;
} Snapshot;

typedef struct {
    Snapshot** items;
    int capacity;
    int size;
} BoundedStack;

typedef struct {
    int capacity;
    double group_window;
    int max_total_chars;
    BoundedStack* undo_stack;
    BoundedStack* redo_stack;
    int undo_chars;
    int redo_chars;
    char* group_key;
    double group_time;
} UndoRedoManager;

BoundedStack* stack_create(int capacity);
void stack_destroy(BoundedStack* stack);
Snapshot* stack_push(BoundedStack* stack, Snapshot* item);
Snapshot* stack_pop(BoundedStack* stack);
Snapshot* stack_peek(BoundedStack* stack);
void stack_clear(BoundedStack* stack);

UndoRedoManager* undo_redo_create(int capacity, double group_window, int max_total_chars);
void undo_redo_destroy(UndoRedoManager* manager);
bool undo_redo_record(UndoRedoManager* manager, const char* old_text, int old_cursor, const char* new_text, int new_cursor, const char* label);
Snapshot* undo_redo_undo(UndoRedoManager* manager, const char* current_text, int current_cursor);
Snapshot* undo_redo_redo(UndoRedoManager* manager, const char* current_text, int current_cursor);
void undo_redo_reset(UndoRedoManager* manager);

#endif
