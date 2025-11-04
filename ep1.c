/**
 * @brief   Algoritmo de Prim utilizando Heap Mínimo
 * @details O programa recebe o nome de um arquivo .txt que contém a descrição do grafo como primeiro parâmetro via linha de comando.
 *          O arquivo .txt deve ser o seguinte formato:
 *
 *          n m
 *          u1 v1 c1
 *          u2 v2 c2
 *          ...
 *          um vm cm
 *		
 *	    Sendo n o número de vértices, m o número de arestas, ui e vi as extremidades da aresta i e ci é o custo da aresta i.
 *          Usando o Algoritmo de Prim, o programa calcula e retorna as arestas que compõem a árvore de custo mínimo desse grafo.
 * @author  Daniel Leandro Campos Silva
 * @author  João Guilherme Santos de Sousa
 * @author  Lucas Silva do Nascimento
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/** 
 * @brief um nó do grafo representado pela lista de adjacências
 */
typedef struct node
{
    int32_t vertex;    /*!< o vértice que forma uma aresta com o vértice de seu índice na lista */
    int32_t cost;      /*!< o custo da aresta */
    struct node *next; /*!< ponteiro para o próximo nó da lista */
} node_t;

/**
 * @brief uma aresta do grafo
 */
typedef struct edge
{
    int32_t vertex_u; /*!< o primeiro vértice da aresta */
    int32_t vertex_v; /*!< o segundo vértice da aresta */
    int32_t cost;     /*!< o custo da aresta */
} edge_t;

/**
 * @brief um heap de arestas
 */
typedef struct heap
{
    int32_t size;     /*!< o tamanho do heap*/
    int32_t capacity; /*!< a capacidade do heap */
    edge_t *edges;    /*!< a lista de arestas */
} heap_t;

/** 
 * @brief  lê o arquivo com o gráfico, salva o número de vértices e arestas em seus ponteiros, salva o ponteiro para a representação do grafo e insere todos os vértices na lista de adjacências
 * @param  filename o nome do arquivo a ser lido
 * @param  graph retorna o ponteiro da representação do gráfico
 * @param  vertices retorna o número de vértices do grafo
 * @param  edges retorna o número de arestas do grafo
 * @return 0 se rodar com sucesso, 1 se não
 */
uint8_t create_graph(const char *filename, node_t **graph, int32_t *vertices, int32_t *edges);
/**
 * @brief  procura por um nó vazio na lista, atualiza o valor desse nó e insere um novo nó vazio no fim da lista
 * @param  list ponteiro que aponta para a lista para inserir valores
 * @param  vertex o vértice a ser inserido na lista
 * @param  cost o custo da aresta a ser inserido na lista
 * @return 0 se rodar com sucesso, 1 se não
 */
uint8_t insert_on_list(node_t *list, int32_t vertex, int32_t cost);

/**
 * @brief  Algoritmo de Prim para a Árvore Geradora Mínima
 * @param  graph o grafo em questão a ser analizado
 * @param  vertices o número de vértices do grafo
 * @param  edges o números de arestas do grafo
 * @return 0 se rodar com sucesso, 1 se não
 */
uint8_t prim(node_t **graph, int32_t vertices, int32_t edges);
/**
 * @brief  procura por um valor na lista
 * @param  value o valor a ser procurado na lista
 * @param  array o ponteiro para a lista
 * @param  array_size o tamanho da lista
 * @return 1 se o valor está na lista, 0 se não
 */
uint8_t search_on_array(int32_t value, const int32_t *array, int32_t array_size);

/**
 * @brief  cria um heap vazio de arestas
 * @param  capacity a capacidade do heap
 * @retorna o ponteiro para o head se rodar com sucesso, NULL se não
 */
heap_t *create_heap(int32_t capacity);
/**
 * @brief insere uma aresta no heap
 * @param heap o ponteiro para o heap
 * @param edge aresta a ser inserida
 */
void insert_on_heap(heap_t *heap, edge_t edge);
/**
 * @brief posiciona uma aresta no lugar correto na pilha
 * @param heap o ponteiro para a pilha
 * @param index o índice da aresta a ser posicionada
 */
void arrange_heap(heap_t *heap, int32_t index);
/**
 * @brief  remove a aresta mínima da pilha e todas as arestas que se tornam arestas internas
 * @param  heap o ponteiro para o heap
 * @retorna a aresta mínima removida
 */
edge_t remove_min_from_heap(heap_t *heap);
/**
 * @brief  reestrutura o heap para manter as propriedades do min-heap
 * @param  heap o ponteiro para o heap
 * @param  index o índice da aresta a ser reestruturada
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

    // lê o primeiro n e m do arquivo
    if (sscanf(line, "%d %d", vertices, edges) != 2)
    {
        printf("Erro no layout do arquivo\n");
        return 1;
    }

    // aloca memória para vertices + 1, pois o número de vertices começa em 1
    *graph = (node_t *)malloc((*vertices + 1) * sizeof(node_t));
    if (*graph == NULL)
    {
        printf("Erro ao alocar espaço para o grafo\n");
        return 1;
    }
    // inicializa todos os nós da lisa com NULL em seus ponteiros
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
        // lê cada aresta i
        if (sscanf(line, "%d %d %d", &vertex_u, &vertex_v, &cost) != 3)
        {
            printf("Erro no layout do arquivo\n");
            return 1;
        }
        // verifica por arestas que terminam no mesmo vértice
        if (vertex_u == vertex_v)
        {
            printf("Erro, o grafo fornecido possui uma aresta conectando o mesmo vertice\n");
            return 1;
        }

        // insere vertex_v na lista vertex_u
        if (insert_on_list(&(*graph)[vertex_u], vertex_v, cost))
        {
            return 1;
        }
        // insere vertex_u na lista vertex_v
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

    // procura por um nó nulo na lista
    while (list->next != NULL)
    {
        list = list->next;
    }

    // atualiza o valor do nó nulo
    list->vertex = vertex;
    list->cost = cost;
    // insere um nó nullo no fim da lista
    list->next = null_node;

    return 0;
}

uint8_t prim(node_t **graph, int32_t vertices, int32_t edges)
{
    // cria uma lista com os vértices inseridos na Árvore Geradora Mínima
    int32_t tree_vertices_size = 0;
    int32_t *tree_vertices = (int32_t *)malloc(vertices * sizeof(int32_t));
    if (tree_vertices == NULL)
    {
        printf("Erro ao alocar espaço para os vertices da árvore\n");
        return 1;
    }

    // cria uma lista com as arestas inseridos na Árvore Geradora Mínima
    edge_t *tree_edges = (edge_t *)malloc((vertices - 1) * sizeof(edge_t));
    if (tree_edges == NULL)
    {
        printf("Erro ao alocar espaço para as arestas da árvore\n");
        return 1;
    }

    // cria um heap que armazenará as arestas de fronteira da árvore
    heap_t *heap = create_heap(edges);
    if (heap == NULL)
    {
        printf("Erro ao alocar espaço para a heap\n");
        return 1;
    }

    // insere o primeiro vértice na árvore
    tree_vertices[tree_vertices_size] = 1;
    tree_vertices_size++;

    // adiciona as arestas do primeiro vértice ao heap que guarda as arestas de fronteira
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

        // insere as arestas do novo vértice no heap de arestas de fronteira
        node = &(*graph)[min.vertex_v];
        while (node->next != NULL)
        {
            // verifica se a aresta a ser inserida não é uma aresta interna para inseri-la no heap
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

        // insere o novo vértice na árvore e a aresta de menor peso na árvore
        tree_edges[tree_vertices_size - 1] = min;
        tree_vertices[tree_vertices_size] = min.vertex_v;
        tree_vertices_size++;
    }

    // imprime as arestas e o custo da árvore geradora mínima
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
        // insere a aresta no final do heap
        heap->edges[heap->size] = edge;
        // coloca a aresta na posição correta
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
        // chamada recursiva com a nova posição da aresta
        arrange_heap(heap, parent_index);
    }
}

edge_t remove_min_from_heap(heap_t *heap)
{
    edge_t removed = heap->edges[0];

    //remove todas as arestas internas do heap
    int32_t i = 0;
    while (i < heap->size)
    {
        edge_t *current = &heap->edges[i];

        // o vértice removido v será um vértice interno, portanto todas as arestas que têm esse vértice v serão arestas internas
        if (current->vertex_v == removed.vertex_v)
        {
            // troca a última aresta pela aresta atual e verifica novamente este índice
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
        // reorganiza as arestas restantes na forma de um min-heap
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
