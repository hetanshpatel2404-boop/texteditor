#include "../include/stats.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

TextStats compute_text_stats(const char* text) {
    TextStats stats = {0, 0, 0, 0};
    if (!text) return stats;

    stats.chars = strlen(text);
    stats.lines = 1;
    bool in_word = false;

    for (int i = 0; text[i] != '\0'; i++) {
        if (!isspace((unsigned char)text[i])) {
            stats.chars_no_whitespace++;
            if (!in_word) {
                stats.words++;
                in_word = true;
            }
        } else {
            in_word = false;
            if (text[i] == '\n') stats.lines++;
        }
    }
    return stats;
}

void get_cursor_line_col(const char* text, int index, int* line, int* col) {
    if (!text || !line || !col) return;
    int len = strlen(text);
    if (index < 0) index = 0;
    if (index > len) index = len;

    *line = 1;
    *col = 1;
    for (int i = 0; i < index; i++) {
        if (text[i] == '\n') {
            (*line)++;
            *col = 1;
        } else {
            (*col)++;
        }
    }
}
