struct graph;
struct node;
struct edge;

struct graph
{
	struct node *nodes;
};

int 	graph_create(struct graph *);
void 	graph_delete(struct graph *);
int	graph_init(struct graph *);
void	graph_fini(struct graph *);

struct node
{
	struct edge *edges;
	size_t data_size;
};

int 	node_create(struct node *);
void	node_delete(struct node *);
int	node_push_front(struct node *, struct node);
int	node_pop_front(struct node *);
int	node_push_back(struct node *, struct node);
int	node_pop_back(struct node *);
void	node_front(struct node *, struct node *);
int 	node_init(struct node *);
void	node_fini(struct node *);

struct edge
{
	int weight;
	struct node *from, *to;
	struct edge *next;
};

int	edge_create(struct edge *);
void	edge_delete(struct edge *);
int 	edge_init(struct edge *);
void	edge_fini(struct edge *);

int edge_create(struct edge *edge)
{
	edge = malloc(sizeof(*edge);
	edge_init(edge);
}

void edge_delete(struct edge *edge)
{
	edge_fini(edge);
	free(edge);
}

int edge_init(struct edge *edge)
{
	memset(edge, 0, sizeof(*edge);
}

void edge_fini(struct edge *edge)
{
}


struct node *node_create(void)
{
	struct node *node = malloc(sizeof(*node));
	memset(node, 0, sizeof(*node));
	return node;
}

int node_init(struct node *node)
{
	struct edge *edge = malloc(sizeof(*edge));
	memset(edge, 0, sizeof(*edge));
	node->edges = edge;
	return 0;
}

void node_free(struct node *node)
{
	struct node *next;
	while(node) {
		next = node->next;
		free(node);
		node = next;
	}
	node = NULL;
}

void node_destroy(struct node *node)
{
	free(node);
	node = NULL;
}
