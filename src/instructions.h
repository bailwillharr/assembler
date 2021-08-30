// Both real and pseudo instructions

#define OPCODE_NAME_MAX_LEN 4

enum pseudo_opcode { PSEUDO_NULL, PSEUDO_DB, PSEUDO_ORG };

void opcode_lookup(char *opcode_name, int *opcode, size_t *opcode_sz);
