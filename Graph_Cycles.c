/**
 * Author	: Anastasios Gkikas 
 * AM		: 7110132200217
 * Programm	: 
 * 		Creates a sparse matrix corresponding to a graph with 
 * 		insert/delete/find edge functionality and prints path 
 * 		cycles using Breadth First Search algorithm
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



/* TYPEDEFS & STRUCTS DEFINITIONS */

typedef int NodeID;

typedef struct Edge* edgePtr;
/* Element of the Sparse Matrix */
struct Edge
{ 
	NodeID i, j;
	double data;  // abstract data member
	edgePtr nextH, prevH;
	edgePtr nextV, prevV;
	bool valid;  // true if edge belongs to the BFS tree, else false
};

typedef struct EdgeList* edgeListPtr;
/* Manages Edge objects */
struct EdgeList
{
	edgePtr head, tail;
};

#define MAX_NODES 30
typedef struct Graph* grPtr;
struct Graph
{
	struct EdgeList H[MAX_NODES], V[MAX_NODES];
	bool permitting_loops;  // set to false in init_graph()
	NodeID max;  // max is used in print_graph()
	NodeID parent[MAX_NODES];  // parent[i] = i.parent
	int d[MAX_NODES];
};

typedef struct ListNode* listNodePtr;
struct ListNode
{
	edgePtr edge;
	listNodePtr next;
};

typedef struct List* listPtr;
/**
 * Manages ListNode objects.
 * Can be implemented as:
 * - queue, using enqueue() and dequeue()
 * - stack, using push() and pop()
*/
struct List
{
	listNodePtr head, tail;
};



/* FUNCTIONS DECLARATIONS */

void print_menu();

void init_graph(grPtr graph);

NodeID input_node(char* name);
NodeID* input_edge(grPtr graph);

edgePtr create_edge(NodeID* nodes, double data);
void print_edge(edgePtr edge);

void insert_main(grPtr graph);
bool insert_undirectional(grPtr graph, NodeID* nodes);
bool insert_edge(grPtr graph, NodeID i, NodeID j);
void insert_H(edgeListPtr list, edgePtr edge);
void insert_V(edgeListPtr list, edgePtr edge);

void delete_main(grPtr graph);
bool delete_undirectional(grPtr graph, NodeID* nodes);
bool delete_edge(grPtr graph, NodeID i, NodeID j);
void delete_H(edgeListPtr list, edgePtr edge);
void delete_V(edgeListPtr list, edgePtr edge);
void clear(grPtr graph);

void update_max(grPtr graph);

void find_main(grPtr graph);
edgePtr find_edge(grPtr graph, NodeID* nodes);
edgePtr find_edge_in_list(edgePtr aux, NodeID j);

void print_graph(grPtr graph);
void print_list_H(edgePtr aux);

void enqueue(listPtr queue, edgePtr edge);
edgePtr dequeue(listPtr queue);

void push(listPtr stack, edgePtr edge);
edgePtr pop(listPtr stack);
void reverse_stack(listPtr stack);
void print_stack(listPtr stack);

void BFS_main(grPtr graph);
bool BFS_driver(grPtr graph);
void BFS_reset(grPtr graph);
void BFS(grPtr graph, NodeID source, listPtr tempQ);
void enqueue_adjacent_edges(grPtr graph, NodeID source, listPtr queue);
void BFS_print(grPtr graph);
void BFS_print_list_H(edgePtr aux);

void cycles_main(grPtr graph);
void cycles_driver(grPtr graph);
void print_cycle(edgePtr edge, grPtr graph);
struct List path_to_source(NodeID node, grPtr graph);



/* FUNCTIONS DEFINITIONS */

int main()
{
	struct Graph G;
	grPtr g = &G;
	init_graph(g);

	char choice;
	bool first_run = true;

	print_menu();
	
	while(true)
	{
		printf("\n\n Give your choice ('m' for main menu): ");
		fflush(stdin);
		choice=getchar(); getchar();
		
		if (choice=='m') print_menu();
		if (choice=='q') 
		{
			printf("\n QUIT \n");
			clear(g);
			print_graph(g);
			break;
		}
		if (choice=='i')
		{
			printf("\n INSERT \n");
			
			if (first_run)
			{
				// template input
				NodeID nodes[][2] = {
					{6, 3}, {3, 7}, {3, 4}, {4, 8}, {4, 7}, 
					{7, 8}, {2, 6}, {2, 1}, {1, 5}, {6, 7}
				};
				for (int k = 0; k < 10; k++)
					insert_undirectional(g, nodes[k]);

				first_run = false;
				printf("\n Template graph ready");
			}
			else insert_main(g); 
		}
		if (choice=='d')
		{
			printf("\n DELETE \n");
			delete_main(g); 
		}
		if (choice=='p')
		{	
			printf("\n PRINT \n");
			print_graph(g);
		}
		if (choice=='f')
		{	
			printf("\n FIND \n");
			find_main(g);
		}
		if (choice=='b')
		{	
			printf("\n BFS \n");
			BFS_main(g);
		}
		if (choice=='k')
		{
			printf("\n CYCLES \n");
			cycles_main(g);
		}
		if (choice=='c')
		{
			printf("\n CLEAR \n");
			clear(g);
		}
	}
	return 0;
}

void print_menu()
{
	printf(
		"\n MAIN MENU"
		"\n insert: i"
		"\n delete: d"
		"\n print : p"
		"\n find  : f"
		"\n bfs   : b"
		"\n cycles: k"
		"\n clear : c"
		"\n quit  : q"
	);
}

void init_graph(grPtr graph)
{
	graph->permitting_loops = false;
	graph->max = 0;
	for (NodeID k = 1; k < MAX_NODES; k++)
	{
		graph->H[k].head = NULL;
		graph->H[k].tail = NULL;
		
		graph->V[k].head = NULL;
		graph->V[k].tail = NULL;

		graph->parent[k] = -1;
		graph->d[k] = 0;
	}
}

edgePtr create_edge(NodeID* nodes, double data)
{
	edgePtr edge = malloc(sizeof *edge);

	edge->i = nodes[0];
	edge->j = nodes[1];
	edge->data = data;
	edge->nextH = NULL;
	edge->nextV = NULL;
	edge->prevH = NULL;
	edge->prevV = NULL;
	edge->valid = false;
	
	return edge;
}

void print_edge(edgePtr edge)
{
	printf("(%d, %d)", edge->i, edge->j);
}

/* gets node id from the user */
NodeID input_node(char* name)
{
	NodeID node;
	while (true)
	{
		// prompt
		printf("\n (to cancel enter 0)");
		printf("\n Give %s: ", name);

		// read from user
		fflush(stdin);
		scanf("%d", &node);

		// ckeck for cancel option
		if (node == 0) return 0;

		// return node id if valid
		if (0 < node && node < MAX_NODES) 
			return node;
		
		// else clarify valid range to the user
		printf("\n Valid node IDs: %d-%d", 1, MAX_NODES-1);
	}
}

/* gets edge nodes from the user*/
NodeID* input_edge(grPtr graph)
{
	NodeID i = input_node("source");
	if (i == 0) return NULL;
	NodeID j = input_node("destination");
	if (j == 0) return NULL;

	if (i == j && !graph->permitting_loops) {
		printf("\n Loops are not allowed!");
		return NULL; 
	}
	
	NodeID* nodes = malloc(sizeof *nodes * 2);
	nodes[0] = i;
	nodes[1] = j;
	return nodes;
}

/* interface for insertion */
void insert_main(grPtr graph)
{
	NodeID* nodes = input_edge(graph);
	if (nodes == NULL) return;

	bool success = insert_undirectional(graph, nodes);

	printf("\n Edge (%d, %d)", nodes[0], nodes[1]);
	if (success) printf(" inserted");
	else printf(" already exists");

	free(nodes);
}

/* inserts both edge and its symmetrical edge into the sparse matrix*/
bool insert_undirectional(grPtr graph, NodeID *nodes)
{
	NodeID i = nodes[0], j = nodes[1];

	bool success = insert_edge(graph, i, j);
	if (i != j) success = success && insert_edge(graph, j, i);

	return success;
}

/* inserts edge into the sparse matrix */
bool insert_edge(grPtr graph, NodeID i, NodeID j)
{
	NodeID nodes[2] = {i, j};

	edgePtr edge = find_edge(graph, nodes);
	if (edge != NULL) return false; // guard clause

	edge = create_edge(nodes, 1);
	edgeListPtr V = &graph->V[i], H = &graph->H[j];
	
	insert_H(V, edge);
	insert_V(H, edge);

	/* command below is the same as calling update_max() 
	but would require a few more computations */
	if (i > graph->max) graph->max = i;

	return true;
}

/* inserts edge into the horizontal list*/
void insert_H(edgeListPtr list, edgePtr edge)
{
	// next starts from head
	edgePtr next = list->head; 
	
	// next will be edge->next
	while(next != NULL && next->j < edge->j)
		next = next->nextH;

	// linking edge with next->prev
	if (next != NULL) {  // edge before tail
		edge->prevH = next->prevH;
		if (next->prevH != NULL)  // next != head
			next->prevH->nextH = edge;
		else  // next == head
			list->head = edge;
	} else {  // edge after tail
		edge->prevH = list->tail;  // edge.prev = tail
		if (list->tail != NULL)  // if tail != NULL
			list->tail->nextH = edge;  // tail.next = edge
		else  // head == NULL then head = edge
			list->head = edge;
	}

	// linking edge with next
	edge->nextH = next;
	if (next != NULL) next->prevH = edge;
	else list->tail = edge;
}

/* inserts edge into the vertical list*/
void insert_V(edgeListPtr list, edgePtr edge)
{
	edgePtr next = list->head; 
	while(next != NULL && next->j < edge->j)
		next = next->nextV;

	if (next != NULL) {
		edge->prevV = next->prevV;
		if (next->prevV != NULL)
			next->prevV->nextV = edge;
		else list->head = edge;
	} else {
		edge->prevV = list->tail;
		if (list->tail != NULL)
			list->tail->nextV = edge;
		else list->head = edge;
	}

	edge->nextV = next;
	if (next != NULL) next->prevV = edge;
	else list->tail = edge;
}

/* interface for deletion */
void delete_main(grPtr graph)
{
	NodeID* nodes = input_edge(graph);
	if (nodes == NULL) return;
	
	bool success = delete_undirectional(graph, nodes);

	printf("\n Edge (%d, %d)", nodes[0], nodes[1]);
	if (success) printf(" deleted");
	else printf(" doesn't exist");

	free(nodes);
}

/* deletes both edge and its symmetrical edge from the sparse matrix*/
bool delete_undirectional(grPtr graph, NodeID *nodes)
{
	NodeID i = nodes[0], j = nodes[1];

	bool success = delete_edge(graph, i, j);
	if (i != j) success = success && delete_edge(graph, j, i);
	
	return success;
}

/* deletes edge from the sparse matrix */
bool delete_edge(grPtr graph, NodeID i, NodeID j)
{
	NodeID nodes[2] = {i, j};
	edgePtr edge = find_edge(graph, nodes);
	if (edge == NULL) return false; // guard clause
	
	edgeListPtr V = &graph->V[i], H = &graph->H[j];

	delete_H(V, edge);
	delete_V(H, edge);
	
	free(edge);

	if (i == graph->max) update_max(graph);

	return true;
}

/* deletes edge from the horizontal list*/
void delete_H(edgeListPtr list, edgePtr edge)
{	
	// unlink from prev
	if (edge == list->head)
		list->head = edge->nextH;
	else edge->prevH->nextH = edge->nextH;

	// unlink from next
	if (edge == list->tail)
		list->tail = edge->prevH;
	else edge->nextH->prevH = edge->prevH;
}

/* deletes edge from the vertical list*/
void delete_V(edgeListPtr list, edgePtr edge)
{	
	if (edge == list->head)
		list->head = edge->nextV;
	else edge->prevV->nextV = edge->nextV;

	if (edge == list->tail)
		list->tail = edge->prevV;
	else edge->nextV->prevV = edge->prevV;
}

/* deletes all edges from the sparse matrix*/
void clear(grPtr graph)
{
	for (NodeID i = 1; i <= graph->max; i++)
		for (NodeID j = 1; j <= graph->max; j++)
			delete_edge(graph, i, j);
}

/* updates max value of graph according to the present edges */
void update_max(grPtr graph)
{
	graph->max = 0;
	for (NodeID i = 0; i < MAX_NODES; i++)
		if (graph->V[i].head != NULL)
			graph->max = i;
}

/* interface for search */
void find_main(grPtr graph)
{
	NodeID* nodes = input_edge(graph);
	if (nodes == NULL) return;
	
	edgePtr edge = find_edge(graph, nodes);

	printf("\n Edge (%d, %d)", nodes[0], nodes[1]);
	if (edge == NULL) printf(" not");
	printf(" found");
}

/* finds appropriate horizontal list in which the edge might be */
edgePtr find_edge(grPtr graph, NodeID *nodes)
{
	NodeID i = nodes[0], j = nodes[1];
	return find_edge_in_list(graph->V[i].head, j);
}

/* searches for edge in the horizontal list */
edgePtr find_edge_in_list(edgePtr aux, NodeID j)
{
	if (aux == NULL || aux->j == j) return aux;
	return find_edge_in_list(aux->nextH, j);
}

/* intiates printing for every horizontal list*/
void print_graph(grPtr graph)
{
	if (graph->max == 0)
	{
		printf("\n The graph is empty");
		return;
	}

	printf("\n");
	// printing horizontal indices at the top
	for (NodeID i = 1; i <= graph->max; i++)
		printf("\t  %d", i);

	for (NodeID i = 1; i <= graph->max; i++)
	{
		// vertical index at the beginning of each list
		printf("\n\n\n%d", i);  
		print_list_H(graph->V[i].head);
	}
}

/* prints horizontal lists*/
void print_list_H(edgePtr aux)
{
	if (aux == NULL) return;
	
	int gap;  // used for printing the gaps between edges
	if (aux->prevH == NULL) gap = aux->j;
	else gap = aux->j - aux->prevH->j;

	// printing horizontal space
	for (int j = 0; j < gap; j++)
		printf("\t");
	
	print_edge(aux);
	
	print_list_H(aux->nextH);
}

void enqueue(listPtr queue, edgePtr edge)
{
	listNodePtr newNode = malloc(sizeof *newNode);
	newNode->edge = edge;
	newNode->next = NULL;

	if (queue->head == NULL)
		queue->head = newNode;
	else queue->tail->next = newNode;

	queue->tail = newNode;
}

edgePtr dequeue(listPtr queue)
{
	listNodePtr head = queue->head;
	if (head == NULL) return NULL;

	edgePtr edge = head->edge;

	if (queue->tail == head) queue->tail = NULL;
	queue->head = head->next;
	free(head);

	return edge;
}

void push(listPtr stack, edgePtr edge)
{
	listNodePtr newNode = malloc(sizeof *newNode);
	newNode->edge = edge;

	newNode->next = stack->tail;
	stack->tail = newNode;
	if (stack->head == NULL)
		stack->head = newNode;
}

edgePtr pop(listPtr stack)
{
	listNodePtr tail = stack->tail;
	if (tail == NULL) return NULL;

	edgePtr edge = tail->edge;

	if (stack->head == tail) stack->head = NULL;
	stack->tail = tail->next;
	free(tail);

	return edge;
}

void reverse_stack(listPtr stack)
{
	struct List Buffer;  // actually a queue
	Buffer.head = Buffer.tail = NULL;

	// temporarily store stack's nodes in a queue
	while (stack->head != NULL)
		enqueue(&Buffer, pop(stack));
		
	// push the nodes in reverse order into the stack
	while (Buffer.head != NULL)
		push(stack, dequeue(&Buffer));
}

/* prints and empties the stack */
void print_stack(listPtr stack)
{
	while (stack->tail != NULL) {
		printf(", ");
		print_edge(pop(stack));
	}
}

/* BFS entry and print */
void BFS_main(grPtr graph)
{
	BFS_driver(graph);
	BFS_print(graph);
}

/* BFS interface and algorithm */
bool BFS_driver(grPtr graph)
{
	BFS_reset(graph);

	struct List Queue;
	Queue.head = Queue.tail = NULL;

	NodeID source = input_node("source");
	if (source == 0) return false;

	BFS(graph, source, &Queue);

	return true;
}

/* resets all edge values that might have been affected by BFS */
void BFS_reset(grPtr graph)
{
	for (NodeID i = 1; i <= graph->max; i++)
	{
		graph->parent[i] = -1;
		graph->d[i] = 0;
		
		edgePtr edge = graph->V[i].head;
		while (edge != NULL) 
		{
			edge->valid = false;
			edge = edge->nextH;
		}
	}
}

/**
 *	BFS(source, queue)
 *		if first call of BFS
 *			source.parent = source
 *
 *		enqueue adjacent edges that point to non-visited nodes
 *
 *		while queue not empty
 *			call BFS for the node pointed by the dequeued edge
*/
void BFS(grPtr graph, NodeID source, listPtr queue)
{
	if (graph->parent[source] == -1)
		graph->parent[source] = source;

	enqueue_adjacent_edges(graph, source, queue);

	while (queue->head != NULL)
		BFS(graph, dequeue(queue)->j, queue);
}

/**
 *	enqueue_adjacent_edges(source, queue)
 *		for all edges adjacent to source node
 *			destination = node pointed by edge
 *			if destination not visited
 *				destination.parent = source
 *				destination.d = source.d + 1
 *				mark edge as valid
 *				enqueue edge
 *				mark symmetric edge as valid
*/
void enqueue_adjacent_edges(grPtr graph, NodeID source, listPtr queue)
{
	edgePtr adj = graph->V[source].head;
	while (adj != NULL)
	{
		NodeID destination = adj->j;

		if (graph->parent[destination] == -1)
		{
			graph->parent[destination] = source;
			graph->d[destination] = graph->d[source] + 1;

			adj->valid = true;
			enqueue(queue, adj);

			NodeID symmetric[2] = {adj->j, adj->i};
			find_edge(graph, symmetric)->valid = true;
		}
		
		adj = adj->nextH;
	}
}

/* same as print_graph() but only for valid edges */
void BFS_print(grPtr graph)
{
	if (graph->max == 0)
	{
		printf("\n The graph is empty");
		return;
	}

	printf("\n");
	for (NodeID i = 1; i <= graph->max; i++)
		printf("\t  %d", i);

	for (NodeID i = 1; i <= graph->max; i++)
	{
		printf("\n\n\n%d", i);
		BFS_print_list_H(graph->V[i].head);
	}
}

/* same as print_list_H() but only for valid edges */
void BFS_print_list_H(edgePtr aux)
{
	if (aux == NULL) return;
	
	NodeID j;
	int gap;
	if (aux->prevH == NULL) gap = aux->j;
	else gap = aux->j - aux->prevH->j;

	for (j = 0; j < gap; j++)
		printf("\t");
	
	if (!aux->valid) print_edge(aux);
	
	BFS_print_list_H(aux->nextH);
}

/* BFS interface and algorithm, then cycles aglorithm */
void cycles_main(grPtr graph)
{
	if (BFS_driver(graph) == false) return;
	cycles_driver(graph);
}

/* prints a cycle for every invalid edge */
void cycles_driver(grPtr graph)
{
	// for every horizontal list
	for (NodeID i = 1; i <= graph->max; i++) 
	{
		edgePtr edge = graph->V[i].head;
		// search for invalid edges
		while (edge != NULL) {
			if (!edge->valid) {
				print_cycle(edge, graph);
				
				/* toggle inverse edge validity 
				so as not to find same cycles */
				NodeID symmetric[2] = {edge->j, edge->i};
				find_edge(graph, symmetric)->valid = true;
			}
			edge = edge->nextH;
		}

	}
}

/* cycles algorithm for an invalid edge */
void print_cycle(edgePtr edge, grPtr graph)
{
	// stacks to store paths from edge nodes to source
	struct List iStack, jStack;
	iStack = path_to_source(edge->i, graph);
	jStack = path_to_source(edge->j, graph);

	// discard same edges from both paths
	while(iStack.tail->edge == jStack.tail->edge) {
		pop(&iStack);
		pop(&jStack);
	}

	// mainly for nice printing
	reverse_stack(&iStack);
	reverse_stack(&jStack);

	printf("\n ");
	print_edge(edge);  // edge that creates cycle
	print_stack(&iStack);  // section of circle from node i
	print_stack(&jStack);  // section of circle from node j
}

struct List path_to_source(NodeID node, grPtr graph)
{
	struct List Stack;  // stores path edges
	Stack.head = Stack.tail = NULL;

	// until node == source
	while (graph->parent[node] != node)  
	{
		// traverse to (node, node.parent) edge
		NodeID nodes[2] = {node, graph->parent[node]};
		edgePtr edge = find_edge(graph, nodes);
		push(&Stack, edge);

		// move up a node
		node = graph->parent[node];
	}

	return Stack;
}
