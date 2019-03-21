#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE (4096)

typedef union _value_t value_t;
typedef struct _object_t object_t;
typedef enum _operator_t operator_t;
typedef struct _instruction_t instruction_t;

union _value_t {
	void *nil;
	int integer;
	object_t *object;
	instruction_t *function;
	value_t *pointer;
};

struct _object_t {
	int size;
	value_t *values;
};

enum _operator_t {
	LABEL,
	OP_DEC,
	OP_PUSH,
	OP_LOADA,
	OP_NEW,
	OP_FREE,
	OP_JNZ,
	OP_CALL,
	OP_RET,
	OP_EXIT,
};

struct _instruction_t {
	operator_t operator;
	value_t operand;
};

static int num_alloc = 0;
static int num_free = 0;

object_t *alloc_object(int size) {
	num_alloc++;

	value_t *values = (value_t*) malloc(sizeof(value_t) * size);
	object_t *obj = (object_t*) malloc(sizeof(object_t));

	obj->size = size;
	obj->values = values;

	return obj;
}

void free_object(object_t *obj) {
	for (int i = 0; i < obj->size; i++) {
		object_t *child = obj->values[i].object;

		if (child) {
			free_object(obj->values[i].object);
		}
	}

	num_free++;

	free(obj->values);
	free(obj);
}

void debug_dump(instruction_t *inst, value_t *stack, value_t *sp) {
	char *label;

	switch (inst->operator) {
		case LABEL:    label = "LABEL   "; break;
		case OP_DEC:   label = "OP_DEC  "; break;
		case OP_PUSH:  label = "OP_PUSH "; break;
		case OP_LOADA: label = "OP_LOADA"; break;
		case OP_NEW:   label = "OP_NEW  "; break;
		case OP_FREE:  label = "OP_FREE "; break;
		case OP_JNZ:   label = "OP_JNZ  "; break;
		case OP_CALL:  label = "OP_CALL "; break;
		case OP_RET:   label = "OP_RET  "; break;
		case OP_EXIT:  label = "OP_EXIT "; break;
	}

	printf("%s %x\n", label, inst->operand.integer);

	for (sp--; sp >= stack; sp--) {
		printf("  %x\n", sp->integer);
	}

}

typedef struct _label_entry_t label_entry_t;
struct _label_entry_t {
	int label_number;
	instruction_t *position;
	label_entry_t *next;
};

instruction_t *find_label(label_entry_t *list, int label_number) {
	label_entry_t *p = list;

	while (p != NULL) {
		if (p->label_number == label_number) {
			return p->position;
		}
		p = p->next;
	}

	return NULL;
}

label_entry_t* remove_labels(instruction_t *begin, instruction_t *end) {
	label_entry_t *label_list = NULL;

	instruction_t *p = begin;
	instruction_t *q = p;
	label_entry_t *label = NULL;

	while (p < end) {
		if (p->operator == LABEL) {
			label_entry_t *e = (label_entry_t*) malloc(sizeof(label_entry_t));
			e->label_number = p->operand.integer;
			e->position = q;
			e->next = NULL;

			if (label != NULL) {
				label->next = e;
				label = e;
			}
			else {
				label = e;
				label_list = e;
			}
			p++;
		}
		else {
			if (p != q) {
				*q = *p;
			}
			p++;
			q++;
		}
	}

	return label_list;
}

void free_label_list(label_entry_t *p) {
	while (p != NULL) {
		label_entry_t *next = p->next;
		free(p);
		p = next;
	}
}

void resolve_label(instruction_t *begin, instruction_t *end) {
	label_entry_t *label_list = remove_labels(begin, end);

	instruction_t *p = begin;
	while (p < end) {
		if (p->operator == OP_JNZ || p->operator == OP_CALL) {
			p->operand.function = find_label(label_list, p->operand.integer);
		}
		p++;
	}

	free_label_list(label_list);
}

void run_stack_machine(int depth) {
	instruction_t program[] = {
		{ OP_PUSH,  { .integer = depth } },  // make_tree(depth)
		{ OP_CALL,  { .integer = 0x2000 } },
		{ OP_FREE,  { NULL } },
		{ OP_EXIT,  { NULL } },

		/* node_t *make_tree(int depth) */
		{ LABEL,    { .integer = 0x2000 } },
		{ OP_LOADA, { .integer = 0 } },  // if depth == 0
		{ OP_JNZ,   { .integer = 0x2001 } },
		{ OP_PUSH,  { .object = NULL } },
		{ OP_RET,   { NULL } },

		{ LABEL,    { .integer = 0x2001 } },
		{ OP_LOADA, { .integer = 0 } },  // make_tree(depth - 1)
		{ OP_DEC,   { NULL } },
		{ OP_CALL,  { .integer = 0x2000 } },
		
		{ OP_LOADA, { .integer = 0 } },  // make_tree(depth - 1)
		{ OP_DEC,   { NULL } },
		{ OP_CALL,  { .integer = 0x2000 } },

		{ OP_NEW,   { .integer = 2 } },  // new
		{ OP_RET,   { NULL } },
	};

	resolve_label(program, program + sizeof(program) / sizeof(instruction_t));
	value_t *stack = (value_t*) malloc(sizeof(value_t) * STACK_SIZE);
	
	value_t *sp = stack;
	value_t *fp = stack;
	instruction_t *pc = program;

	while (1) {
		// debug_dump(pc, stack, sp);

		// if (sp >= stack + STACK_SIZE - 2) {
		// 	printf("stack over flow\n");
		// 	exit(1);
		// }

		switch (pc->operator) {
			case LABEL:
			{
				pc++;
				break;
			}

			case OP_DEC:
			{
				value_t *p = sp - 1;
				p->integer -= 1;
				pc++;
				break;
			}

			case OP_PUSH:
			{
				*(sp++) = pc->operand;
				pc++;
				break;
			}

			case OP_LOADA:
			{
				*(sp++) = fp[ pc->operand.integer ];
				pc++;
				break;
			}

			case OP_NEW:
			{
				int size = pc->operand.integer;

				object_t *obj = alloc_object(size);
				memcpy(obj->values, sp - size, sizeof(value_t) * size);

				sp = sp - size;
				(sp++)->object= obj;

				pc++;
				break;
			}

			case OP_FREE:
			{
				free_object( (--sp)->object );
				pc++;
				break;
			}

			case OP_JNZ:
			{
				if ((--sp)->integer != 0) {
					pc = pc->operand.function;
				}
				else {
					pc++;
				}
				break;
			}

			case OP_CALL:
			{
				int num_args = 1;

				(sp++)->function = pc;
				(sp++)->pointer = fp;

				pc = pc->operand.function;
				fp = sp - 2 - num_args;
				break;
			}

			case OP_RET:
			{
				value_t retval = *(--sp);
				value_t *next_sp = fp;

				fp = (--sp)->pointer;
				pc = (--sp)->function + 1;

				sp = next_sp;
				*(sp++) = retval;
				break;
			}

			case OP_EXIT:
			{
				goto VM_EXIT;
			}
		}
	}

VM_EXIT:
	free(stack);
}

int main(int argc, char **argv) {
	int depth;
	int times;

	if (scanf("%d", &depth) != 1) {
		return 1;
	}
	if (scanf("%d", &times) != 1) {
		return 1;
	}

	for (int i = 0; i < times; i++) {
		for (int d = 1; d <= depth; d++) {
			run_stack_machine(d);
		}
	}

	printf("%d\n", num_alloc);
	printf("%d\n", num_free);

	return 0;
}
