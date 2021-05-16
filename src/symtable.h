typedef struct {
	char* name;
	int value;
} symtable_entry_t;

int symtable_build(FILE*, symtable_entry_t**, size_t);
