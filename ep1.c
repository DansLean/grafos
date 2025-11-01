/**
 * @brief   Prim's Algorithm using Min Heap
 * @details The program receives the name of a file containing the description of a graph via the command line.
 *          The file provided to the program must have the following layout:
 *          n m
 *          u1 v1 c1
 *          u2 v2 c2
 *          ...
 *          um vm cm
 *          where n is the number of vertices, m is the number of edges,
 *          ui and vi are the ends of edge i and ci is the cost of edge i.
 *          Using Prim's Algorithm, the program calculates and displays the edges
 *          that constitute a minimum-cost spanning tree of the graph.
 * @author  Luca Uicaa Oliveira Paiva
 * @author  Matheus Batista Chavante
 * @author  Yuri Mateus Santiago
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief a node of the adjacency list that represents the graph
 */
typedef struct node
{
    int32_t vertex;    /*!< the vertex that makes an edge with the vertex of its list index number */
    int32_t cost;      /*!< the cost of the edge */
    struct node *next; /*!< pointer to the next node of the list */
} node_t;

/**
 * @brief an edge of the graph
 */
typedef struct edge
{
    int32_t vertex_u; /*!< the first vertex of the edge */
    int32_t vertex_v; /*!< the second vertex of the edge */
    int32_t cost;     /*!< the cost of the edge */
} edge_t;

/**
 * @brief a heap of edges structure
 */
typedef struct heap
{
    int32_t size;     /*!< the size of the heap */
    int32_t capacity; /*!< the capacity of the heap */
    edge_t *edges;    /*!< the edges array */
} heap_t;

/**
 * @brief  reads the file with the graph, saves the number of vertices and edges
 *         in its pointers, saves the pointer to the graph representation and
 *         inserts all the vertices into its adjacency list
 * @param  filename the name of the file to be read
 * @param  graph returns the pointer of the graph representation
 * @param  vertices returns the number of vertices of the graph
 * @param  edges returns the number of edges of the graph
 * @return 0 on success, 1 otherwise
 */
uint8_t create_graph(const char *filename, node_t **graph, int32_t *vertices, int32_t *edges);
/**
 * @brief  searches for a null node in the list, updates the values ​​of this node
 *         and inserts a new null node at the end of the list
 * @param  list pointer to the list to insert values
 * @param  vertex the vertex to be inserted on list
 * @param  cost the edge cost to be inserted on list
 * @return 0 on success, 1 otherwise
 */
uint8_t insert_on_list(node_t *list, int32_t vertex, int32_t cost);

/**
 * @brief  Prim's algorithm for minimum spanning tree
 * @param  graph the graph to be analyzed
 * @param  vertices the number of vertices of the graph
 * @param  edges the number of edges of the graph
 * @return 0 on success, 1 otherwise
 */
uint8_t prim(node_t **graph, int32_t vertices, int32_t edges);
/**
 * @brief  searches for a value in an array
 * @param  value the value to be search
 * @param  array the pointer to array
 * @param  array_size the array size
 * @return 1 if the value is in the array, 0 otherwise
 */
uint8_t search_on_array(int32_t value, const int32_t *array, int32_t array_size);

/**
 * @brief  creates a empty heap of edges
 * @param  capacity the heap capacity
 * @return the pointer to the heap on success, NULL otherwise
 */
heap_t *create_heap(int32_t capacity);
/**
 * @brief inserts an edge into the heap
 * @param heap the pointer to the heap
 * @param edge the edge to be inserted
 */
void insert_on_heap(heap_t *heap, edge_t edge);
/**
 * @brief arranges an edge in the correct place in the heap
 * @param heap the pointer to the heap
 * @param index the index of the edge to be arranged
 */
void arrange_heap(heap_t *heap, int32_t index);
/**
 * @brief  removes the minimum edge from the heap and all edges that become internal edges
 * @param  heap the pointer to the heap
 * @return the minimum edge removed
 */
edge_t remove_min_from_heap(heap_t *heap);
/**
 * @brief  heapifies the heap to the min heap properties
 * @param  heap the pointer to the heap
 * @param  index the edge index to heapify
 */
void heapify(heap_t *heap, int32_t index);

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Insira o nome do arquivo ao executar o programa.\n");
        printf("Ex: $ ./ep1 grafo.txt\n");
        return 1;
    }

    int32_t vertices;
    int32_t edges;
    node_t *graph = NULL;
    if (create_graph(argv[1], &graph, &vertices, &edges))
    {
        return 1;
    }

    if (prim(&graph, vertices, edges))
    {
        return 1;
    }

    return 0;
}

uint8_t create_graph(const char *filename, node_t **graph, int32_t *vertices, int32_t *edges)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }

    char line[32];
    if (fgets(line, sizeof(line), file) == NULL)
    {
        printf("Erro ao ler arquivo\n");
        return 1;
    }

    // reads the first n and m from file
    if (sscanf(line, "%d %d", vertices, edges) != 2)
    {
        printf("Erro no layout do arquivo\n");
        return 1;
    }

    // allocates memory for vertices + 1, due to vertex number begins with 1
    *graph = (node_t *)malloc((*vertices + 1) * sizeof(node_t));
    if (*graph == NULL)
    {
        printf("Erro ao alocar espaço para o grafo\n");
        return 1;
    }
    // initializes all nodes of the list array with NULL in their pointers
    for (int32_t i = 1; i <= *vertices; i++)
    {
        (*graph)[i].next = NULL;
    }

    for (int32_t i = 0; i < *edges; i++)
    {
        if (fgets(line, sizeof(line), file) == NULL)
        {
            printf("Erro ao ler arquivo\n");
            return 1;
        }

        int32_t vertex_u;
        int32_t vertex_v;
        int32_t cost;
        // reads each edge i
        if (sscanf(line, "%d %d %d", &vertex_u, &vertex_v, &cost) != 3)
        {
            printf("Erro no layout do arquivo\n");
            return 1;
        }
        // checks for edges with ends at the same vertex
        if (vertex_u == vertex_v)
        {
            printf("Erro, o grafo fornecido possui uma aresta conectando o mesmo vertice\n");
            return 1;
        }

        // inserts vertex v in vertex u list
        if (insert_on_list(&(*graph)[vertex_u], vertex_v, cost))
        {
            return 1;
        }
        // inserts vertex u in vertex v list
        if (insert_on_list(&(*graph)[vertex_v], vertex_u, cost))
        {
            return 1;
        }
    }

    fclose(file);
    return 0;
}

uint8_t insert_on_list(node_t *list, int32_t vertex, int32_t cost)
{
    node_t *null_node = (node_t *)malloc(sizeof(node_t));
    if (null_node == NULL)
    {
        printf("Erro ao alocar espaço para o grafo\n");
        return 1;
    }
    null_node->next = NULL;

    // searches for a null node on the list
    while (list->next != NULL)
    {
        list = list->next;
    }

    // updates the values of the null node
    list->vertex = vertex;
    list->cost = cost;
    // inserts a null node at the end of the list
    list->next = null_node;

    return 0;
}

uint8_t prim(node_t **graph, int32_t vertices, int32_t edges)
{
    // creates a array with the vertices inserted on the minimum spanning tree
    int32_t tree_vertices_size = 0;
    int32_t *tree_vertices = (int32_t *)malloc(vertices * sizeof(int32_t));
    if (tree_vertices == NULL)
    {
        printf("Erro ao alocar espaço para os vertices da árvore\n");
        return 1;
    }

    // creates a array with the edges inserted on the minimum spanning tree
    edge_t *tree_edges = (edge_t *)malloc((vertices - 1) * sizeof(edge_t));
    if (tree_edges == NULL)
    {
        printf("Erro ao alocar espaço para as arestas da árvore\n");
        return 1;
    }

    // creates a heap that will store the border edges of the tree
    heap_t *heap = create_heap(edges);
    if (heap == NULL)
    {
        printf("Erro ao alocar espaço para a heap\n");
        return 1;
    }

    // inserts the first vertex on the tree
    tree_vertices[tree_vertices_size] = 1;
    tree_vertices_size++;

    // inserts the first vertex edges to the heap of border edges
    node_t *node = &(*graph)[1];
    while (node->next != NULL)
    {
        edge_t edge = {
            .vertex_u = 1,
            .vertex_v = node->vertex,
            .cost = node->cost,
        };
        insert_on_heap(heap, edge);

        node = node->next;
    }

    while (tree_vertices_size < vertices)
    {
        if (heap->size == 0)
        {
            printf("O grafo não possui arvore geradora minima\n");
            return 1;
        }

        edge_t min = remove_min_from_heap(heap);

        // inserts the new vertex edges to the heap of border edges
        node = &(*graph)[min.vertex_v];
        while (node->next != NULL)
        {
            // checks if the edge to be inserted is not an
            // internal edge to insert it into the heap
            if (!search_on_array(node->vertex, tree_vertices, tree_vertices_size))
            {
                edge_t edge = {
                    .vertex_u = min.vertex_v,
                    .vertex_v = node->vertex,
                    .cost = node->cost,
                };
                insert_on_heap(heap, edge);
            }

            node = node->next;
        }

        // inserts the new vertex on the tree
        // and the minimum edge on the tree
        tree_edges[tree_vertices_size - 1] = min;
        tree_vertices[tree_vertices_size] = min.vertex_v;
        tree_vertices_size++;
    }

    // prints the minimum spanning tree edges and cost
    int32_t tree_value = 0;
    printf("Arvore Geradora de Custo Minimo:");
    for (int32_t i = 0; i < vertices - 1; i++)
    {
        tree_value += tree_edges[i].cost;
        printf(" (%d, %d)", tree_edges[i].vertex_u, tree_edges[i].vertex_v);
    }
    printf("\n");

    printf("Custo: %d\n", tree_value);
    return 0;
}

uint8_t search_on_array(int32_t value, const int32_t *array, int32_t array_size)
{
    for (int32_t i = 0; i < array_size; i++)
    {
        if (array[i] == value)
        {
            return 1;
        }
    }
    return 0;
}

heap_t *create_heap(int32_t capacity)
{
    heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
    if (heap == NULL)
    {
        return NULL;
    }

    heap->size = 0;
    heap->capacity = capacity;
    heap->edges = (edge_t *)malloc(capacity * sizeof(edge_t));
    if (heap->edges == NULL)
    {
        return NULL;
    }

    return heap;
}

void insert_on_heap(heap_t *heap, edge_t edge)
{
    if (heap->size < heap->capacity)
    {
        // inserts the edge at the end of the heap
        heap->edges[heap->size] = edge;
        // arranges the edge to the correct place
        arrange_heap(heap, heap->size);
        heap->size++;
    }
}

void arrange_heap(heap_t *heap, int32_t index)
{
    if (index <= 0)
    {
        return;
    }

    int32_t parent_index = (index - 1) / 2;
    edge_t *parent = &heap->edges[parent_index];
    edge_t *current = &heap->edges[index];

    if (parent->cost > current->cost)
    {
        edge_t temp = *parent;
        *parent = *current;
        *current = temp;
        // recursive call with the new position of the edge
        arrange_heap(heap, parent_index);
    }
}

edge_t remove_min_from_heap(heap_t *heap)
{
    edge_t removed = heap->edges[0];

    // removes all internal edges from the heap
    int32_t i = 0;
    while (i < heap->size)
    {
        edge_t *current = &heap->edges[i];

        // the removed vertex v will be an internal vertex,
        // so all edges with this vertex at v will be internal edges
        if (current->vertex_v == removed.vertex_v)
        {
            // swaps the last edge with the current and checks this index again
            *current = heap->edges[heap->size - 1];
            heap->size--;
        }
        else
        {
            i++;
        }
    }

    if (heap->size > 1)
    {
        // heapify the remaining edges
        i = (heap->size - 2) / 2;
        while (i >= 0)
        {
            heapify(heap, i);
            i--;
        }
    }

    return removed;
}

void heapify(heap_t *heap, int32_t index)
{
    int64_t left = index * 2 + 1;
    int64_t right = index * 2 + 2;
    int64_t min = index;

    if (left >= heap->size || left < 0)
    {
        left = -1;
    }
    if (right >= heap->size || right < 0)
    {
        right = -1;
    }

    if (left != -1 && heap->edges[left].cost < heap->edges[index].cost)
    {
        min = left;
    }
    if (right != -1 && heap->edges[right].cost < heap->edges[min].cost)
    {
        min = right;
    }

    if (min != index)
    {
        edge_t *minimum = &heap->edges[min];
        edge_t *current = &heap->edges[index];

        edge_t temp = *minimum;
        *minimum = *current;
        *current = temp;
        heapify(heap, min);
    }
}