#include <stdio.h>
#include <stdlib.h>

static int num_alloc = 0;
static int num_free = 0;

typedef struct _node_t node_t;
struct _node_t {
	node_t *l;
	node_t *r;
};

node_t *make_node(node_t *l, node_t *r) {
	num_alloc += 1;

	node_t *node = (node_t*) malloc(sizeof(node_t));
	node->l = l;
	node->r = r;
	return node;
}

node_t *make_tree(int depth) {
	if (depth <= 0) {
		return NULL;
	}
	else {
		return make_node( make_tree(depth - 1), make_tree(depth - 1) );
	}
}

void free_tree(node_t *tree) {
	if (tree->l) {
		free_tree(tree->l);
	}
	if (tree->r) {
		free_tree(tree->r);
	}

	free(tree);
	num_free += 1;
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
		node_t *last_tree = NULL;
		for (int d = 1; d <= depth; d++) {
			node_t *tree = make_tree(d);

			if (last_tree != NULL) {
				free_tree(last_tree);
			}

			last_tree = tree;
		}
		free_tree(last_tree);
	}

	printf("%d\n", num_alloc);
	printf("%d\n", num_free);

	return 0;
}
