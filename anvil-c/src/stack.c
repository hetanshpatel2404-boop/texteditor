#define _POSIX_C_SOURCE 200809L
#include "../include/stack.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

BoundedStack* stack_create(int capacity) {
    if (capacity < 1) capacity = 150;
    BoundedStack* stack = malloc(sizeof(BoundedStack));
    stack->items = calloc(capacity, sizeof(Snapshot*));
    stack->capacity = capacity;
    stack->size = 0;
    return stack;
}

void stack_destroy(BoundedStack* stack) {
    if (!stack) return;
    for (int i = 0; i < stack->size; i++) {
        free(stack->items[i]->text);
        free(stack->items[i]);
    }
    free(stack->items);
    free(stack);
}

Snapshot* stack_push(BoundedStack* stack, Snapshot* item) {
    if (!stack || !item) return NULL;
    Snapshot* dropped = NULL;
    if (stack->size == stack->capacity) {
        dropped = stack->items[0];
        for (int i = 0; i < stack->capacity - 1; i++) {
            stack->items[i] = stack->items[i + 1];
        }
        stack->items[stack->capacity - 1] = item;
    } else {
        stack->items[stack->size] = item;
        stack->size++;
    }
    return dropped;
}

Snapshot* stack_pop(BoundedStack* stack) {
    if (!stack || stack->size == 0) return NULL;
    stack->size--;
    return stack->items[stack->size];
}

Snapshot* stack_peek(BoundedStack* stack) {
    if (!stack || stack->size == 0) return NULL;
    return stack->items[stack->size - 1];
}

void stack_clear(BoundedStack* stack) {
    if (!stack) return;
    for (int i = 0; i < stack->size; i++) {
        free(stack->items[i]->text);
        free(stack->items[i]);
    }
    stack->size = 0;
}

UndoRedoManager* undo_redo_create(int capacity, double group_window, int max_total_chars) {
    UndoRedoManager* manager = malloc(sizeof(UndoRedoManager));
    manager->capacity = capacity;
    manager->group_window = group_window;
    manager->max_total_chars = max_total_chars;
    manager->undo_stack = stack_create(capacity);
    manager->redo_stack = stack_create(capacity);
    manager->undo_chars = 0;
    manager->redo_chars = 0;
    manager->group_key = NULL;
    manager->group_time = 0.0;
    return manager;
}

void undo_redo_destroy(UndoRedoManager* manager) {
    if (!manager) return;
    stack_destroy(manager->undo_stack);
    stack_destroy(manager->redo_stack);
    free(manager->group_key);
    free(manager);
}

static void compute_diff(const char* old_text, const char* new_text, int* position, char* deleted, char* inserted) {
    if (strcmp(old_text, new_text) == 0) {
        *position = 0;
        deleted[0] = '\0';
        inserted[0] = '\0';
        return;
    }
    int i = 0;
    while (old_text[i] && new_text[i] && old_text[i] == new_text[i]) i++;
    *position = i;
    strncpy(inserted, new_text + i, MAX_DIFF_LEN);
    inserted[MAX_DIFF_LEN] = '\0';
    strncpy(deleted, old_text + i, MAX_DIFF_LEN);
    deleted[MAX_DIFF_LEN] = '\0';
}

bool undo_redo_record(UndoRedoManager* manager, const char* old_text, int old_cursor, const char* new_text, int new_cursor, const char* label) {
    if (!manager || strcmp(old_text, new_text) == 0) return false;

    time_t now = time(NULL);
    int position;
    char deleted[MAX_DIFF_LEN + 1];
    char inserted[MAX_DIFF_LEN + 1];
    compute_diff(old_text, new_text, &position, deleted, inserted);

    const char* kind = "edit";
    if (strlen(inserted) > 0 && strlen(deleted) == 0) kind = "insert";
    else if (strlen(deleted) > 0 && strlen(inserted) == 0) kind = "delete";
    else kind = "replace";

    if (!label) label = kind;

    bool can_coalesce = false;
    if (strcmp(kind, "insert") == 0 || strcmp(kind, "delete") == 0) {
        if (manager->group_key && strcmp(manager->group_key, kind) == 0) {
            if (difftime(now, manager->group_time) <= manager->group_window) {
                can_coalesce = true;
            }
        }
    }

    if (can_coalesce) {
        Snapshot* top = stack_peek(manager->undo_stack);
        if (top) {
            if (strcmp(kind, "insert") == 0) {
                strncat(top->inserted, inserted, MAX_DIFF_LEN - strlen(top->inserted));
            } else {
                char temp[MAX_DIFF_LEN + 1];
                snprintf(temp, sizeof(temp), "%s%s", deleted, top->deleted);
                strncpy(top->deleted, temp, MAX_DIFF_LEN);
            }
            top->edit_count++;
            top->last_timestamp = now;
            strncpy(top->label, label, MAX_LABEL_LEN - 1);
            strncpy(top->kind, kind, MAX_LABEL_LEN - 1);
            manager->group_time = (double)now;
            return false;
        }
    }

    Snapshot* snap = malloc(sizeof(Snapshot));
    snap->text = strdup(old_text);
    snap->cursor = old_cursor;
    strncpy(snap->label, label ? label : kind, MAX_LABEL_LEN - 1);
    strncpy(snap->kind, kind, MAX_LABEL_LEN - 1);
    snap->timestamp = now;
    snap->position = position;
    strncpy(snap->inserted, inserted, MAX_DIFF_LEN);
    strncpy(snap->deleted, deleted, MAX_DIFF_LEN);
    snap->edit_count = 1;
    snap->last_timestamp = now;

    Snapshot* dropped = stack_push(manager->undo_stack, snap);
    if (dropped) {
        manager->undo_chars -= strlen(dropped->text);
        free(dropped->text);
        free(dropped);
    }
    manager->undo_chars += strlen(snap->text);

    stack_clear(manager->redo_stack);
    manager->redo_chars = 0;

    while (manager->undo_chars > manager->max_total_chars && manager->undo_stack->size > 1) {
        Snapshot* old = manager->undo_stack->items[0];
        for (int i = 0; i < manager->undo_stack->size - 1; i++) {
            manager->undo_stack->items[i] = manager->undo_stack->items[i + 1];
        }
        manager->undo_stack->size--;
        manager->undo_chars -= strlen(old->text);
        free(old->text);
        free(old);
    }

    free(manager->group_key);
    manager->group_key = strdup(kind);
    manager->group_time = (double)now;
    return true;
}

Snapshot* undo_redo_undo(UndoRedoManager* manager, const char* current_text, int current_cursor) {
    if (!manager || manager->undo_stack->size == 0) return NULL;
    Snapshot* snap = stack_pop(manager->undo_stack);
    manager->undo_chars -= strlen(snap->text);

    Snapshot* redo_snap = malloc(sizeof(Snapshot));
    redo_snap->text = strdup(current_text);
    redo_snap->cursor = current_cursor;
    strncpy(redo_snap->label, snap->label, MAX_LABEL_LEN - 1);
    strncpy(redo_snap->kind, snap->kind, MAX_LABEL_LEN - 1);
    redo_snap->timestamp = time(NULL);
    redo_snap->position = snap->position;
    strncpy(redo_snap->inserted, snap->inserted, MAX_DIFF_LEN);
    strncpy(redo_snap->deleted, snap->deleted, MAX_DIFF_LEN);
    redo_snap->edit_count = snap->edit_count;
    redo_snap->last_timestamp = snap->last_timestamp;

    Snapshot* dropped = stack_push(manager->redo_stack, redo_snap);
    if (dropped) {
        manager->redo_chars -= strlen(dropped->text);
        free(dropped->text);
        free(dropped);
    }
    manager->redo_chars += strlen(redo_snap->text);

    free(manager->group_key);
    manager->group_key = NULL;
    manager->group_time = 0.0;
    return snap;
}

Snapshot* undo_redo_redo(UndoRedoManager* manager, const char* current_text, int current_cursor) {
    if (!manager || manager->redo_stack->size == 0) return NULL;
    Snapshot* snap = stack_pop(manager->redo_stack);
    manager->redo_chars -= strlen(snap->text);

    Snapshot* undo_snap = malloc(sizeof(Snapshot));
    undo_snap->text = strdup(current_text);
    undo_snap->cursor = current_cursor;
    strncpy(undo_snap->label, snap->label, MAX_LABEL_LEN - 1);
    strncpy(undo_snap->kind, snap->kind, MAX_LABEL_LEN - 1);
    undo_snap->timestamp = time(NULL);
    undo_snap->position = snap->position;
    strncpy(undo_snap->inserted, snap->inserted, MAX_DIFF_LEN);
    strncpy(undo_snap->deleted, snap->deleted, MAX_DIFF_LEN);
    undo_snap->edit_count = snap->edit_count;
    undo_snap->last_timestamp = snap->last_timestamp;

    Snapshot* dropped = stack_push(manager->undo_stack, undo_snap);
    if (dropped) {
        manager->undo_chars -= strlen(dropped->text);
        free(dropped->text);
        free(dropped);
    }
    manager->undo_chars += strlen(undo_snap->text);

    free(manager->group_key);
    manager->group_key = NULL;
    manager->group_time = 0.0;
    return snap;
}

void undo_redo_reset(UndoRedoManager* manager) {
    if (!manager) return;
    stack_clear(manager->undo_stack);
    stack_clear(manager->redo_stack);
    manager->undo_chars = 0;
    manager->redo_chars = 0;
    free(manager->group_key);
    manager->group_key = NULL;
    manager->group_time = 0.0;
}
