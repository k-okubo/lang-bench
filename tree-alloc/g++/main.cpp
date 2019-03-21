#include <iostream>

static int num_alloc = 0;
static int num_free = 0;

class Node {
	private:
		Node *l;
		Node *r;

	public:
		Node() : l(NULL), r(NULL) {}
		Node(Node *l, Node *r) : l(l), r(r) {}
		virtual ~Node();

};

Node::~Node() {
	if (l) {
		delete l;
	}
	if (r) {
		delete r;
	}
	num_free += 1;
}

Node *make_tree(int depth) {
	if (depth <= 0) {
		return NULL;
	}
	else {
		num_alloc += 1;
		return new Node( make_tree(depth - 1), make_tree(depth - 1) );
	}
}

void free_tree(Node *tree) {
	delete tree;
}

int main(int argc, char **argv) {
	int depth;
	int times;

	std::cin >> depth;
	std::cin >> times;

	for (int i = 0; i < times; i++) {
		Node *last_tree = NULL;
		for (int d = 1; d <= depth; d++) {
			Node *tree = make_tree(d);

			if (last_tree != NULL) {
				free_tree(last_tree);
			}

			last_tree = tree;
		}
		free_tree(last_tree);
	}

	std::cout << num_alloc << std::endl;
	std::cout << num_free << std::endl;

	return 0;
}
