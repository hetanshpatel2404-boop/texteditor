#ifndef STATS_H
#define STATS_H

typedef struct {
    int words;
    int chars;
    int chars_no_whitespace;
    int lines;
} TextStats;

TextStats compute_text_stats(const char* text);
void get_cursor_line_col(const char* text, int index, int* line, int* col);

#endif
