# Anvil C - DSA Editor Core

This is a C-language translation of the core Data Structures and Algorithms (DSA) from the Python-based "Anvil" terminal text editor repository. 

## Project Structure
- \`include/\`: Header files defining the data structures.
- \`src/\`: Implementation of the DSA modules and a demo \`main.c\`.
- \`Makefile\`: Build configuration for easy compilation.

## Data Structures Included
1. **Text Statistics** (\`stats.h/c\`): O(N) word, character, and line counting.
2. **Bounded Queue** (\`queue.h/c\`): O(1) FIFO clipboard history using a circular buffer.
3. **Prefix Trie** (\`trie.h/c\`): O(L) code completion and keyword storage.
4. **Bounded Stack & Undo/Redo** (\`stack.h/c\`): Snapshot-based undo/redo with intelligent edit coalescing and strict memory bounds.

## How to Build and Run
Ensure you have \`gcc\` and \`make\` installed.

\`\`\`bash
# Compile the project
make

# Run the demonstration
make run

# Clean build artifacts
make clean
\`\`\`

## Memory Safety
All dynamic allocations (\`malloc\`/\`calloc\`) are paired with corresponding \`free\` calls. The demo explicitly destroys all structures to guarantee zero memory leaks.
