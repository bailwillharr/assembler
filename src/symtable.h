typedef struct {
	char* name;
	int value;
} symtable_entry_t;

// size_t is how many entries in the symbol table
symtable_entry_t* symtable_build(FILE*, size_t);
void symtable_cleanup(symtable_entry_t*);
