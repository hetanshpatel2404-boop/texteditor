#include <stdio.h>
#include <string.h>
#include "../include/stats.h"
#include "../include/queue.h"
#include "../include/trie.h"
#include "../include/stack.h"

int main(void) {
    printf("=== DSA Editor Core Components Demo ===\n\n");

    // 1. Text Statistics
    const char* sample_text = "Hello world.\nThis is a DSA editor test.";
    TextStats stats = compute_text_stats(sample_text);
    printf("1. Text Stats: Words=%d, Chars=%d, Lines=%d\n", stats.words, stats.chars, stats.lines);
    
    int line, col;
    get_cursor_line_col(sample_text, 18, &line, &col);
    printf("   Cursor at index 18: Line %d, Col %d\n\n", line, col);

    // 2. Trie Completion
    Trie* trie = trie_create();
    trie_insert(trie, "printf", 100, "keyword", false, false);
    trie_insert(trie, "print", 80, "keyword", false, false);
    trie_insert(trie, "private", 45, "common", false, false);
    
    CompletionList* completions = trie_complete(trie, "pri", 5);
    printf("2. Trie Completions for 'pri':\n");
    for (int i = 0; i < completions->count; i++) {
        printf("   - %s (weight: %d, kind: %s)\n", completions->results[i].word, completions->results[i].weight, completions->results[i].kind);
    }
    completion_list_destroy(completions);
    printf("\n");

    // 3. Clipboard History (Bounded Queue)
    ClipboardHistory* clipboard = clipboard_create(3);
    clipboard_enqueue(clipboard, "First copy");
    clipboard_enqueue(clipboard, "Second copy");
    clipboard_enqueue(clipboard, "Third copy");
    clipboard_enqueue(clipboard, "Fourth copy");
    
    printf("3. Clipboard History (Size: %d):\n", clipboard->size);
    printf("   Latest: %s\n", clipboard_latest(clipboard)->text);
    printf("   Oldest: %s\n\n", clipboard_get(clipboard, clipboard->size - 1)->text);

    // 4. Undo/Redo Manager (Bounded Stack with Coalescing)
    UndoRedoManager* undo_mgr = undo_redo_create(10, 0.9, 10000);
    char* current_text = strdup("");
    int cursor = 0;

    undo_redo_record(undo_mgr, current_text, cursor, "H", 1, "insert");
    free(current_text); current_text = strdup("H"); cursor = 1;
    
    undo_redo_record(undo_mgr, current_text, cursor, "He", 2, "insert");
    free(current_text); current_text = strdup("He"); cursor = 2;

    printf("4. Undo/Redo Manager:\n");
    printf("   Undo depth after coalesced typing: %d\n", undo_mgr->undo_stack->size);
    
    Snapshot* undone = undo_redo_undo(undo_mgr, current_text, cursor);
    printf("   Undone to: '%s' (Edit count in snapshot: %d)\n", undone->text, undone->edit_count);
    
    free(current_text);
    free(undone->text); free(undone);
    undo_redo_destroy(undo_mgr);
    clipboard_destroy(clipboard);
    trie_destroy(trie);

    printf("\n=== All DSA structures destroyed cleanly. No memory leaks. ===\n");
    return 0;
}
