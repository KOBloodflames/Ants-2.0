typedef enum {CELL_DIRT , CELL_WATER, CELL_ANT , CELL_FOOD , CELL_HILL , CELL_ANT_ON_HILL }  cell_type;
typedef enum {N, E, S, W} direction;
typedef enum {GATHERER , SOLDIER , DEFENDER} ant_role;
typedef struct {cell_type type; int owner; int bfs; int bfsvisited; int move; int squad; int soldier; int defender; int vision; int danger; int found;} cell;
//typedef struct {int x; int y; ant_role role; direction lastmove; int water; int alive; int moving; int targetx; int targety;} ant;
typedef struct {int turn; int loadtime; int turntime; int rows; int cols; int turns; int viewradius2; int attackradius2; int spawnradius2; int player_seed;} game_settings; 

typedef struct {char *routelist; int x; int y;} route;

/*linked list*/
struct Node;
typedef struct Item{int id; int x; int y; ant_role role; direction lastmove; int water; int squad; int alive; int moving; int targetx; int targety; int routenr; route *routes; int fight; int fightturns; int fightready;} Item;
typedef struct BFS{int n; int visited; int x; int y;} BFS;
typedef struct Node{struct Node* next; struct Node* prev; struct Item* item; struct BFS* bfs;} Node;
typedef struct ant {int idcounter; int counter; Node* head; int squadsize[3]; int nrsquads; char squadop[3]; int squadmembers[3][9];} ant;

typedef struct {cell **map; int rows; int cols; unsigned int turn; int end; ant *ants; int antnr; int razing; int razex; int razey;} worldmap;

int mod(int a, int b);

ant *List_create();
void List_destroy(ant *list);
void List_clear(ant *list);
void List_print(ant *list);

void List_append(ant *list);
void List_insert(ant *list);

void List_remove(ant* list, Node* node);
Node* List_find(ant *list,int id);
void List_putfirst(ant *list,Node* node);
void List_sort(ant* list);

char vision_change(worldmap w, game_settings g, Node *current);
void print_vision(worldmap w);
worldmap reset_vision(worldmap w);
worldmap check_vision(worldmap w, game_settings g);

route *search_vision(worldmap w, int Y, int X, Node *current);
route *search_hill(worldmap w, int Y, int X, Node *current);

void print_danger(worldmap w);
worldmap reset_danger(worldmap w);
worldmap set_danger(worldmap w, game_settings g, int x0, int y0);

void bfs_print_visited(worldmap w);
worldmap bfs_reset(worldmap w);

worldmap assign_squads(worldmap w);

void print_bfs(worldmap w);

worldmap check_moves(worldmap w, int x);

route *search(worldmap w, int Y, int X, Node *current, int depth);

worldmap read_turn(worldmap w, game_settings s);
worldmap initialize_map(worldmap w);
worldmap cleanup_map(worldmap w);
worldmap reset_map(worldmap world);
worldmap update_ants(worldmap w, int i, int j, int in, int jn, Node *ant);
void print_map(worldmap w);
void read_initialization(game_settings *s);
worldmap send_orders(worldmap w);
worldmap find_route(worldmap w, int Y, int X, int Yn, int Xn, Node *ant);
