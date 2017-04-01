#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "0936196.h"

worldmap initialize_map(worldmap world){
	world.map = (cell **) malloc(world.rows * sizeof(cell*));
	for(int i = 0; i < world.rows; i++){
		world.map[i] = (cell *) malloc(world.cols * sizeof(cell));
	}
	if(world.map == NULL){
		printf("Error allocating memory");
	}else{
		world.ants = List_create();
		world.ants->counter = 0;
		world.ants->idcounter = 0;
		world = reset_map(world);
	}
	return world;
}

worldmap cleanup_map(worldmap world){
	for(int i = 0; i < world.rows; i++){
		free(world.map[i]);
	}
	free(world.map);
	return world;
}

worldmap check_moves(worldmap w, int x){
	int enemyant;
	int friendlyant;
	int hill;
	int friendlyhill;
	int food;
	if(x == 0){
		enemyant = -8;
		friendlyant = -2;
		hill = 100;
		friendlyhill = 0;
		food = 100;
	}else if(x == 1){
		enemyant = 100;
		friendlyant = 5;
		hill = 10000;
		friendlyhill = 0;
		food = 100;
	}else if(x == 2){
		enemyant = -7;
		friendlyant = 10;
		hill = 50;
		friendlyhill = 0;
		food = 100;
	}else if(x == 3){
		enemyant = 0;
		friendlyant = 0;
		hill = 0;
		friendlyhill = 3;
		food = 0;
	}else{
		enemyant = 0;
		friendlyant = 3;
		hill = 100;
		friendlyhill = 0;
		food = 100;

	}
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			if(w.map[i][j].type == CELL_ANT && w.map[i][j].owner != 0){
				w.map[i][j].move = enemyant;
			}else if(w.map[i][j].type == CELL_ANT && w.map[i][j].owner == 0){
				w.map[i][j].move = friendlyant;
			}else if(w.map[i][j].type == CELL_HILL&& w.map[i][j].owner != 0){
				w.map[i][j].move = hill;
			}else if(w.map[i][j].type == CELL_HILL&& w.map[i][j].owner == 0){
				w.map[i][j].move = 0;				
				w.map[i+1][j].move = friendlyhill;
				w.map[i-1][j].move = friendlyhill;
				w.map[i][j+1].move = friendlyhill;
				w.map[i][j-1].move = friendlyhill;
			}else if(w.map[i][j].type == CELL_ANT_ON_HILL&& w.map[i][j].owner != 0){
				w.map[i][j].move = hill - enemyant;
			}else if(w.map[i][j].type == CELL_FOOD){
				w.map[i][j].move = food;
			}else if(w.map[i][j].type == CELL_WATER){
				w.map[i][j].move = -1;
			}else{
				w.map[i][j].move = 0;
			}
			if(i==0){
				if(j!=0){
					if(w.map[i][j-1].move < -1){
						w.map[i][j].move = w.map[i][j-1].move+1;
					}
					else if((w.map[i][j-1].move > w.map[i][j].move + 1) && (w.map[i][j-1].move > 0)){
						w.map[i][j].move = w.map[i][j-1].move -1;
					}
				}		
			}else{
				if(w.map[i][j-1].move < -1 || w.map[i-1][j].move < -1){
					if(w.map[i][j-1].move < w.map[i-1][j].move){
						w.map[i][j].move = w.map[i][j-1].move+1;
					}else{
						w.map[i][j].move = w.map[i-1][j].move+1;
					}
				}
				else if(w.map[i][j].move >= 0){
					if((w.map[i][j-1].move > w.map[i][j].move + 1) && (w.map[i][j-1].move > 0)){
						w.map[i][j].move = w.map[i][j-1].move -1;
					}
					if((w.map[i-1][j].move > w.map[i][j].move + 1) && (w.map[i-1][j].move > 0)){
						w.map[i][j].move = w.map[i-1][j].move -1;
					}
				}
			
			}
		}
	}
	for(int i = w.rows-1; i >=0; i--){
		for(int j = w.cols-1; j >= 0; j--){
			if(w.map[i][mod(j+1,w.cols)].move < -1 || w.map[mod(i+1,w.rows)][j].move < -1){
				if(w.map[i][mod(j+1,w.cols)].move < w.map[mod(i+1,w.rows)][j].move){
					w.map[i][j].move = w.map[i][mod(j+1,w.cols)].move+1;
				}else{
					w.map[i][j].move = w.map[mod(i+1,w.rows)][j].move+1;
				}
			}
			else if(w.map[i][j].move >= 0){
				if((w.map[i][mod(j+1,w.cols)].move > w.map[i][j].move + 1) && (w.map[i][mod(j+1,w.cols)].move > 0)){
					w.map[i][j].move = w.map[i][mod(j+1,w.cols)].move -1;
				}
				if((w.map[mod(i+1,w.rows)][j].move > w.map[i][j].move + 1) && (w.map[mod(i+1,w.rows)][j].move > 0)){
					w.map[i][j].move = w.map[mod(i+1,w.rows)][j].move -1;
				}
			}
		}
	}

	return w;
}




route *search(worldmap w, int Y, int X, Node *current, int depth){

	//fprintf(stderr,"searching for a way to food using BFS\n");
	w = bfs_reset(w);
	ant *queue = List_create();
	List_insert(queue);
	//List_print(queue);
	queue->head->bfs->n = 0;
	queue->head->bfs->x = X;
	queue->head->bfs->y = Y;
	queue->counter = 1;
	int Xn = current->item->x; 
	int Yn = current->item->y;
	w.map[Y][X].bfs = 0;
	w.map[Y][X].bfsvisited = 1;
	int noroute = 0;
	int longest = 999;
	//FOR MAP, VISITED = 0
	int maxdepth = depth;
	int iterations = 0;
	clock_t start = clock(), diff;
	while(1){
		iterations++;
		int x = queue->head->bfs->x;
		int y = queue->head->bfs->y;
		if(queue->counter == 0){
			//fprintf(stderr,"No route possible\n");
			noroute = 1;
			break;
		}
		if(w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_FOOD || (w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_ANT_ON_HILL && w.map[queue->head->bfs->y][queue->head->bfs->x].owner != 0) || (w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_HILL && w.map[queue->head->bfs->y][queue->head->bfs->x].owner != 0) || w.map[queue->head->bfs->y][queue->head->bfs->x].vision == 0){ //queue->head->bfs->x == Xn && queue->head->bfs->y == Yn
			Xn = queue->head->bfs->x;
			Yn = queue->head->bfs->y;	
			//fprintf(stderr,"found food on (%d,%d)\n",Xn,Yn);		
			longest = queue->head->bfs->n;
			break;
		}else{

			if(w.map[mod(y+1,w.rows)][x].type != CELL_WATER && w.map[mod(y+1,w.rows)][x].bfsvisited == 0 && (w.map[mod(y+1,w.rows)][x].danger == 0 || w.map[mod(y+1,w.rows)][x].vision != 0)){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y+1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y+1,w.rows);
				w.map[mod(y+1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[mod(y-1,w.rows)][x].type != CELL_WATER && w.map[mod(y-1,w.rows)][x].bfsvisited == 0 && (w.map[mod(y-1,w.rows)][x].danger == 0|| w.map[mod(y-1,w.rows)][x].vision != 0)){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y-1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y-1,w.rows);
				w.map[mod(y-1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[y][mod(x+1,w.cols)].type != CELL_WATER && w.map[y][mod(x+1,w.cols)].bfsvisited == 0 && (w.map[y][mod(x+1,w.cols)].danger == 0 || w.map[y][mod(x+1,w.cols)].vision != 0)){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x+1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x+1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x+1,w.cols)].bfsvisited = 1;
			}
			if(w.map[y][mod(x-1,w.cols)].type != CELL_WATER && w.map[y][mod(x-1,w.cols)].bfsvisited == 0 && (w.map[y][mod(x-1,w.cols)].danger == 0 || w.map[y][mod(x-1,w.cols)].vision != 0)){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x-1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x-1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x-1,w.cols)].bfsvisited = 1;
			}
			List_remove(queue,queue->head);
		}
	}

	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	//fprintf(stderr, "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
	//fprintf(stderr, "The BFS took %d loops\n", iterations);
	if(!noroute){
		//print_bfs(w);
		route *routed = (route *)malloc(sizeof(route)*1);
		char *routes = (char *)malloc(sizeof(char)*longest);
		routed->routelist = routes;
		int Xl = Xn;
		int Yl = Yn;
		current->item->targetx = Xn;
		current->item->targety = Yn;
		current->item->moving = 0;
		int num = longest;

		//fprintf(stderr, "the route will take %d turns \nroute : ", longest);
		routed->x = Xn;
		routed->y = Yn;
		for(int i = longest-1; i >= 0; i--){
			if(w.map[mod(Yl+1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'N';
				num--;
				Yl = mod(Yl+1,w.rows);
			}else if(w.map[mod(Yl-1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'S';
				num--;
				Yl = mod(Yl-1,w.rows);
			}else if(w.map[Yl][mod(Xl+1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'W';
				num--;
				Xl = mod(Xl+1,w.cols);
			}else if(w.map[Yl][mod(Xl-1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'E';
				num--;
				Xl = mod(Xl-1,w.cols);
			}else{
				//fprintf(stderr, "Something went wrong in the BFS!\n");
			}
			//fprintf(stderr, "%c", routed->routelist[i]);
		}
		//fprintf(stderr, "\n");
		List_destroy(queue);
		return routed;
	}
	else{
		//fprintf(stderr, "Returning x\n");
		route *wrong = (route *)malloc(sizeof(route));
		wrong->routelist = "x";
		List_destroy(queue);
		return wrong;
	}

	
}


route *search_vision(worldmap w, int Y, int X, Node *current){

	//fprintf(stderr,"searching for a way to vision using BFS\n");
	w = bfs_reset(w);
	ant *queue = List_create();
	List_insert(queue);
	//List_print(queue);
	queue->head->bfs->n = 0;
	queue->head->bfs->x = X;
	queue->head->bfs->y = Y;
	queue->counter = 1;
	int Xn = current->item->x; 
	int Yn = current->item->y;
	w.map[Y][X].bfs = 0;
	w.map[Y][X].bfsvisited = 1;
	int noroute = 0;
	int longest = 30;
	//FOR MAP, VISITED = 0
	int maxdepth = 30;
	if(w.ants->counter > 50){
		if(w.ants->counter < 100){
			maxdepth = 20;
		}else if(w.ants->counter < 150){
			maxdepth = 15;
		}else{
			maxdepth = 10;
		}
	}
	int iterations = 0;
	clock_t start = clock(), diff;
	while(1){
		iterations++;
		int x = queue->head->bfs->x;
		int y = queue->head->bfs->y;
		if(queue->counter == 0){
			//fprintf(stderr,"No route possible\n");
			noroute = 1;
			break;
		}
		if(w.map[queue->head->bfs->y][queue->head->bfs->x].vision == 0){ //queue->head->bfs->x == Xn && queue->head->bfs->y == Yn
			Xn = queue->head->bfs->x;
			Yn = queue->head->bfs->y;	
			//fprintf(stderr,"found food on (%d,%d)\n",Xn,Yn);		
			longest = queue->head->bfs->n;
			break;
		}else{

			if(w.map[mod(y+1,w.rows)][x].type != CELL_WATER && w.map[mod(y+1,w.rows)][x].bfsvisited == 0 && w.map[mod(y+1,w.rows)][x].danger == 0 ){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y+1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y+1,w.rows);
				w.map[mod(y+1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[mod(y-1,w.rows)][x].type != CELL_WATER && w.map[mod(y-1,w.rows)][x].bfsvisited == 0 && w.map[mod(y-1,w.rows)][x].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y-1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y-1,w.rows);
				w.map[mod(y-1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[y][mod(x+1,w.cols)].type != CELL_WATER && w.map[y][mod(x+1,w.cols)].bfsvisited == 0 && w.map[y][mod(x+1,w.cols)].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x+1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x+1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x+1,w.cols)].bfsvisited = 1;
			}
			if(w.map[y][mod(x-1,w.cols)].type != CELL_WATER && w.map[y][mod(x-1,w.cols)].bfsvisited == 0 && w.map[y][mod(x-1,w.cols)].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x-1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x-1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x-1,w.cols)].bfsvisited = 1;
			}
			List_remove(queue,queue->head);
		}
	}

	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	//fprintf(stderr, "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
	//fprintf(stderr, "The BFS took %d loops\n", iterations);
	if(!noroute){
		//print_bfs(w);
		route *routed = (route *)malloc(sizeof(route)*1);
		char *routes = (char *)malloc(sizeof(char)*longest);
		routed->routelist = routes;
		int Xl = Xn;
		int Yl = Yn;
		current->item->targetx = Xn;
		current->item->targety = Yn;
		current->item->moving = 0;
		int num = longest;

		//fprintf(stderr, "the route will take %d turns \nroute : ", longest);
		routed->x = Xn;
		routed->y = Yn;
		for(int i = longest-1; i >= 0; i--){
			if(w.map[mod(Yl+1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'N';
				num--;
				Yl = mod(Yl+1,w.rows);
			}else if(w.map[mod(Yl-1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'S';
				num--;
				Yl = mod(Yl-1,w.rows);
			}else if(w.map[Yl][mod(Xl+1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'W';
				num--;
				Xl = mod(Xl+1,w.cols);
			}else if(w.map[Yl][mod(Xl-1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'E';
				num--;
				Xl = mod(Xl-1,w.cols);
			}else{
				//fprintf(stderr, "Something went wrong in the BFS!\n");
			}
			//fprintf(stderr, "%c", routed->routelist[i]);
		}
		//fprintf(stderr, "\n");
		List_destroy(queue);
		return routed;
	}
	else{
		//fprintf(stderr, "Returning x\n");
		route *wrong = (route *)malloc(sizeof(route));
		wrong->routelist = "x";
		List_destroy(queue);
		return wrong;
	}

	
}



route *search_hill(worldmap w, int Y, int X, Node *current){

	//fprintf(stderr,"searching for a way to an enmey hill using BFS\n");
	w = bfs_reset(w);
	ant *queue = List_create();
	List_insert(queue);
	//List_print(queue);
	queue->head->bfs->n = 0;
	queue->head->bfs->x = X;
	queue->head->bfs->y = Y;
	queue->counter = 1;
	int Xn = current->item->x; 
	int Yn = current->item->y;
	w.map[Y][X].bfs = 0;
	w.map[Y][X].bfsvisited = 1;
	int noroute = 0;
	int longest = 999;
	//FOR MAP, VISITED = 0
	int maxdepth = 9999;
	if(w.ants->counter > 50){
		if(w.ants->counter < 100){
			maxdepth = 50;
		}else if(w.ants->counter < 150){
			maxdepth = 40;
		}else{
			maxdepth = 30;
		}
	}
	int iterations = 0;
	clock_t start = clock(), diff;
	while(1){
		iterations++;
		int x = queue->head->bfs->x;
		int y = queue->head->bfs->y;
		if(queue->counter == 0){
			//fprintf(stderr,"No route possible\n");
			noroute = 1;
			break;
		}
		if(w.map[queue->head->bfs->y][queue->head->bfs->x].owner != 0 && (w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_HILL || w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_ANT_ON_HILL) ){ //queue->head->bfs->x == Xn && queue->head->bfs->y == Yn
			Xn = queue->head->bfs->x;
			Yn = queue->head->bfs->y;	
			//fprintf(stderr,"found food on (%d,%d)\n",Xn,Yn);		
			longest = queue->head->bfs->n;
			break;
		}else{

			if(w.map[mod(y+1,w.rows)][x].type != CELL_WATER && w.map[mod(y+1,w.rows)][x].bfsvisited == 0){// && w.map[mod(y+1,w.rows)][x].danger == 0 ){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y+1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y+1,w.rows);
				w.map[mod(y+1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[mod(y-1,w.rows)][x].type != CELL_WATER && w.map[mod(y-1,w.rows)][x].bfsvisited == 0){// && w.map[mod(y-1,w.rows)][x].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[mod(y-1,w.rows)][x].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = x;
				queue->head->prev->bfs->y = mod(y-1,w.rows);
				w.map[mod(y-1,w.rows)][x].bfsvisited = 1;
			}
			if(w.map[y][mod(x+1,w.cols)].type != CELL_WATER && w.map[y][mod(x+1,w.cols)].bfsvisited == 0){// && w.map[y][mod(x+1,w.cols)].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x+1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x+1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x+1,w.cols)].bfsvisited = 1;
			}
			if(w.map[y][mod(x-1,w.cols)].type != CELL_WATER && w.map[y][mod(x-1,w.cols)].bfsvisited == 0){// && w.map[y][mod(x-1,w.cols)].danger == 0){
				List_append(queue);
				queue->head->prev->bfs->n = queue->head->bfs->n + 1;
				w.map[y][mod(x-1,w.cols)].bfs = queue->head->prev->bfs->n;
				queue->head->prev->bfs->x = mod(x-1,w.cols);
				queue->head->prev->bfs->y = y;
				w.map[y][mod(x-1,w.cols)].bfsvisited = 1;
			}
			List_remove(queue,queue->head);
		}
	}

	diff = clock() - start;
	int msec = diff * 1000 / CLOCKS_PER_SEC;
	//fprintf(stderr, "Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
	//fprintf(stderr, "The BFS took %d loops\n", iterations);
	if(!noroute){
		print_bfs(w);
		route *routed = (route *)malloc(sizeof(route)*1);
		char *routes = (char *)malloc(sizeof(char)*longest);
		routed->routelist = routes;
		int Xl = Xn;
		int Yl = Yn;
		current->item->targetx = Xn;
		current->item->targety = Yn;
		current->item->moving = 0;
		int num = longest;

		fprintf(stderr, "the route will take %d turns \nroute : ", longest);
		routed->x = Xn;
		routed->y = Yn;
		for(int i = longest-1; i >= 0; i--){
			if(w.map[mod(Yl+1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'N';
				num--;
				Yl = mod(Yl+1,w.rows);
			}else if(w.map[mod(Yl-1,w.rows)][Xl].bfs == num-1){
				routed->routelist[i] = 'S';
				num--;
				Yl = mod(Yl-1,w.rows);
			}else if(w.map[Yl][mod(Xl+1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'W';
				num--;
				Xl = mod(Xl+1,w.cols);
			}else if(w.map[Yl][mod(Xl-1,w.cols)].bfs == num-1){
				routed->routelist[i] = 'E';
				num--;
				Xl = mod(Xl-1,w.cols);
			}else{
				fprintf(stderr, "Something went wrong in the BFS!\n");
			}
			fprintf(stderr, "%c", routed->routelist[i]);
		}
		fprintf(stderr, "\n");
		List_destroy(queue);
		return routed;
	}
	else{
		//fprintf(stderr, "Returning x\n");
		route *wrong = (route *)malloc(sizeof(route));
		wrong->routelist = "x";
		List_destroy(queue);
		return wrong;
	}

	
}


worldmap bfs_reset(worldmap w){
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			w.map[i][j].bfs = -1;
			w.map[i][j].bfsvisited = 0;
		}
	}
	return w;
}

void bfs_print_visited(worldmap w){
	fprintf(stderr, "printing visited\n");
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			//fprintf(stderr, "%d",w.map[i][j].bfsvisited);
		}
		fprintf(stderr, "\n");
	}
}


void print_map(worldmap w){
	char ant_letters[10] = {'a','b','c','d','e','f','g','h','i','j'};
	char ant_hill_letters[10] = {'A','B','C','D','E','F','G','H','I','J'};
	for(int i = 0; i < w.rows; i++){
	fprintf(stderr,"m ");
		for(int j = 0; j < w.cols; j++){
			switch(w.map[i][j].type){
			case CELL_DIRT:
				fprintf(stderr,".");
				break;
			case CELL_WATER:
				fprintf(stderr,"%%");
				break;
			case CELL_ANT:
				fprintf(stderr,"%c",ant_letters[w.map[i][j].owner]);
				break;
			case CELL_FOOD:
				fprintf(stderr,"*");
				break;
			case CELL_HILL:
				fprintf(stderr,"%d",w.map[i][j].owner);
				break;
			case CELL_ANT_ON_HILL:
				fprintf(stderr,"%c",ant_hill_letters[w.map[i][j].owner]);
				break;
																					
			}
		}
	fprintf(stderr,"\n");
	}
}

void print_bfs(worldmap w){
	for(int i = 0; i < w.rows; i++){
	fprintf(stderr,"%d ", i);
		for(int j = 0; j < w.cols; j++){
				if(w.map[i][j].bfs == -1){
					fprintf(stderr,".");
				}else{
					fprintf(stderr,"%d", w.map[i][j].bfs);
				}

		}
	fprintf(stderr,"\n");
	}
}



worldmap reset_map(worldmap world){
	for(int i = 0; i < world.rows; i++){
		for(int j = 0; j < world.cols; j++){
			world.map[i][j].type = CELL_DIRT;
			world.map[i][j].owner = -1;
		}
	}
	return world;

}

worldmap soft_reset_map(worldmap world){
	for(int i = 0; i < world.rows; i++){
		for(int j = 0; j < world.cols; j++){
			if(world.map[i][j].type != CELL_WATER && world.map[i][j].type != CELL_HILL && (world.map[i][j].type != CELL_ANT || world.map[i][j].owner != 0)){ 
				world.map[i][j].type = CELL_DIRT;
				world.map[i][j].owner = -1;
				world.map[i][j].bfs = -1;
			}
			world.map[i][j].found = 0;
		}
	}
	Node *current = world.ants->head;
	for(int i = 0; i < world.ants->counter; i++){	
		current->item->alive = 0;
		if(current->item->x == current->item->targetx && current->item->y == current->item->targety){
			current->item->moving = 0;
		}
		current = current->next;
	}
	return world;
}

void read_initialization(game_settings *s){
	char *line = NULL;
	size_t linelength = 0;
	char *key = NULL;
	int value = 0;
	while(getline(&line, &linelength, stdin) != 0){	
		int first = 1;
		int end = 0; 
		int end2 = 0;
		int start = 0;
		for(int i = 0; i < linelength; i++){ 		
			if(line[i] == ' ' || line[i] == '\0' || line[i] == '\n'){
				if(first == 1){
					end = i;
					start  = i+1;
					first = 0;
				}else{
					end2 = i;
					break;
				}
			}else if(i == (linelength-1)){
				if(end == 0){
					end = i+1;
					end2 = end;
				}else{
					end2 = i+1;
				}
			}	
		}
		if (end2-end == 1){
			end2 = end;
		}
		//printf("end 2:%d\nend 1:%d\n", end2, end);
		key = (char *) malloc(sizeof(char)*end);
		for(int i = 0; i < end; i++){
			//printf("I can go through the first loop!");
			key[i] = line[i];
		}
		//printf("Linelenght: %zu, end: %d, end2: %d, string: %s, totalstring: %s.\n",linelength,end,end2,key,line);
		if(key != NULL){
			if(strcmp(key,"ready") == 0){
				free(key);
				break;	
			}
		}
		int multiplier = 1;
		value = 0;
		for(int i = end2 - 1; i > end ; i--){
			//printf("I can go through the second loop!");
			value += multiplier*(line[i]-'0'); //-'0'?
			multiplier *= 10;
			//printf("%d * %d = intermediate value: %d\n", line[i] - '0', multiplier, value);
		}
		if(strcmp(key, "turn") == 0){
			s->turn = value;
		}
		else if(strcmp(key, "loadtime") == 0){
			s->loadtime = value;
		}
		else if(strcmp(key, "turntime") == 0){
			s->turntime = value;
		}
		else if(strcmp(key, "rows") == 0){
			s->rows = value;
		}
		else if(strcmp(key, "cols") == 0){
			s->cols = value;
		}
		else if(strcmp(key, "turns") == 0){
			s->turns = value;
		}
		else if(strcmp(key, "viewradius2") == 0){
			s->viewradius2 = value;
		}
		else if(strcmp(key, "attackradius2") == 0){
			s->attackradius2 = value;
		}
		else if(strcmp(key, "spawnradius2") == 0){
			s->spawnradius2 = value;
		}
		else if(strcmp(key, "player_seed") == 0){
s->player_seed = value;
		}
		//printf("key: %s\nvalue: %d\n", key, value);
		free(key);	
	}
	free(line);
}

worldmap update_ants(worldmap w, int i, int j, int in, int jn, Node *current){
	if(w.map[in][jn].type != CELL_WATER){
		if(jn < 0 || jn > w.cols){
			//fprintf(stderr, "amount of columns: %d, previous number j: %d\n",w.cols,jn);
			jn = mod(jn,w.cols);
		}	
		if(in < 0 || in > w.rows){
			//fprintf(stderr, "amount of rows: %d, previous number i: %d\n",w.rows, in);
			in = mod(in,w.rows);
		}
		//fprintf(stderr, "Moving ant %d, from (%d,%d) to (%d,%d)\n",current->item->id, j, i, jn, in);	
		if(w.map[i][j].type == CELL_ANT_ON_HILL){
			w.map[i][j].type = CELL_HILL;
		}else{
			w.map[i][j].type = CELL_DIRT;
			w.map[i][j].owner = -1;
		}
		w.map[in][jn].owner = 0;
		if(w.map[in][jn].type == CELL_HILL){
			w.map[in][jn].type = CELL_ANT_ON_HILL;
		}else{
			w.map[in][jn].type = CELL_ANT;
		}	
		current->item->y = in;
		current->item->x = jn;
	}else{
		//fprintf(stderr, "Could not move!\n");
	}
	return w;
}


worldmap read_turn(worldmap w, game_settings s){
	w.razing = 0;
	char *line = NULL;
	size_t linelength = 0;
	char *key = NULL;
	char *end = "end";
	char *delimiters = " \n";
	char *saveptr;
	w = soft_reset_map(w);
	while(getline(&line, &linelength, stdin) != 0){	
		//printf("Linelenght: %zu, end: %d, end2: %d, string: %s, totalstring: %s.\n",linelength,end,end2,key,line);
		if(strcmp(line,"\n")!=0){
			long row = 0;
			long col = 0;
			long owner = 0;
			//fprintf(stderr,"I HAVE BEEN HERE, text: ");
			key = (char *) malloc(sizeof(char)*linelength);
			key = strtok_r(line, delimiters, &saveptr);
			//fprintf(stderr, "%s\n", key);
			if(key != NULL){
				if(strcmp(key,"end") == 0){
					w.end = 1;	
				}
		
				if(strcmp(key, "go") == 0){
					w.end = 0;
					break;
				}
				else if(strcmp(key, "turn") == 0){
					w.turn = strtol(strtok(saveptr, delimiters), &key, 10);	
					//fprintf(stderr, "saving a turn %d\n", w.turn);			
				}
				else if(strcmp(key, "w") == 0){
					row = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					col = strtol(saveptr,&key,10);
					//fprintf(stderr, "saving a water on row %ld and col %ld\n", row,col);
					w.map[row][col].type = CELL_WATER;
				}
				else if(strcmp(key, "f") == 0){
					row = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					col = strtol(saveptr,&key,10);
					//fprintf(stderr, "saving a food on row %ld and col %ld\n", row,col);
					w.map[row][col].type = CELL_FOOD;
				}
				else if(strcmp(key, "h") == 0){
					row = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					col = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					owner = strtol(saveptr,&key,10);
					//fprintf(stderr, "saving a hill of %ld on row %ld and col %ld\n", owner, row,col);
					w.map[row][col].type = CELL_HILL;
					w.map[row][col].owner = (int)owner;
					if((int)owner != 0){
						w.razing = 1;
						w.razex = col;
						w.razey = row;
						fprintf(stderr, "razing!!!\n");
					}
				}
				else if(strcmp(key, "a") == 0){
					row = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					col = strtol(strtok_r(saveptr, delimiters, &saveptr), &key, 10);
					owner = strtol(saveptr,&key,10);
					//fprintf(stderr, "saving an ant of %ld on row %ld and col %ld\n", owner, row,col);
					w.map[row][col].owner = (int)owner;
					if(w.map[row][col].type == CELL_HILL){
						w.map[row][col].type = CELL_ANT_ON_HILL;
					}else{
						w.map[row][col].type = CELL_ANT;
					}
					if(owner != 0){
						w = set_danger(w,s,col,row);
					}else{
						int antfound = 0;
						Node *current = w.ants->head;
						for(int i = 0 ; i < w.ants->counter; i++){
							if(row == current->item->y && col == current->item->x){
								//fprintf(stderr, "Ant %d is alive!\n",current->item->id);
								current->item->alive = 1;
								current->item->fight = 0;
								antfound = 1;
								break;
							
							}
							current = current->next;
						}
						if(!antfound && owner == 0){ //delete last when bug is found!
							List_insert(w.ants);
							w.ants->head->item->y = row;
							w.ants->head->item->x = col;
							w.ants->head->item->alive = 1;
							w.ants->head->item->moving = 0;
							//fprintf(stderr, "Ant %d has spawned!\n",w.ants->head->item->id);
							w.antnr++;
						}
					}
				}
			}
		}	
	}
	free(line);
	return w;
}

worldmap assign_squads(worldmap w){
	int soldiers = w.ants->counter * 0.5;
	w = bfs_reset(w);
	ant *queue = List_create();
	List_insert(queue);
	//List_print(queue);
	queue->head->bfs->n = 0;
	queue->head->bfs->x = w.razex;
	queue->head->bfs->y = w.razey;
	queue->counter = 1;
	w.map[w.razey][w.razex].bfs = 0;
	w.map[w.razey][w.razex].bfsvisited = 1;
	int found = 0;
	int Xn, Yn;
	int noroute = 0;
	int longest = 999;
	//FOR MAP, VISITED = 0
	int maxdepth = 9999;
	int iterations = 0;
	clock_t start = clock(), diff;
	while(1){
		iterations++;
		int x = queue->head->bfs->x;
		int y = queue->head->bfs->y;
		if(queue->counter == 0){
			//fprintf(stderr,"No route possible\n");
			noroute = 1;
			break;
		}
		if(w.map[queue->head->bfs->y][queue->head->bfs->x].type == CELL_ANT && w.map[queue->head->bfs->y][queue->head->bfs->x].owner == 0){
			Xn = queue->head->bfs->x;
			Yn = queue->head->bfs->y;	
			fprintf(stderr,"found soldier on (%d,%d)\n",Xn,Yn);
			Node *current = w.ants->head;
			for(int i = 0; i < w.ants->counter; i++){
				if(current->item->x == Xn && current->item->y == Yn){
					current->item->role = SOLDIER;
					fprintf(stderr,"ant %d is became a soldier!\n",current->item->id);
					break;
				}
				current = current->next;
			}
			found++;
			if(found > soldiers){
				break;
			}		
		}

		if(w.map[mod(y+1,w.rows)][x].type != CELL_WATER && w.map[mod(y+1,w.rows)][x].bfsvisited == 0){
			List_append(queue);
			queue->head->prev->bfs->n = queue->head->bfs->n + 1;
			w.map[mod(y+1,w.rows)][x].bfs = queue->head->prev->bfs->n;
			queue->head->prev->bfs->x = x;
			queue->head->prev->bfs->y = mod(y+1,w.rows);
			w.map[mod(y+1,w.rows)][x].bfsvisited = 1;
		}
		if(w.map[mod(y-1,w.rows)][x].type != CELL_WATER && w.map[mod(y-1,w.rows)][x].bfsvisited == 0){
			List_append(queue);
			queue->head->prev->bfs->n = queue->head->bfs->n + 1;
			w.map[mod(y-1,w.rows)][x].bfs = queue->head->prev->bfs->n;
			queue->head->prev->bfs->x = x;
			queue->head->prev->bfs->y = mod(y-1,w.rows);
			w.map[mod(y-1,w.rows)][x].bfsvisited = 1;
		}
		if(w.map[y][mod(x+1,w.cols)].type != CELL_WATER && w.map[y][mod(x+1,w.cols)].bfsvisited == 0){
			List_append(queue);
			queue->head->prev->bfs->n = queue->head->bfs->n + 1;
			w.map[y][mod(x+1,w.cols)].bfs = queue->head->prev->bfs->n;
			queue->head->prev->bfs->x = mod(x+1,w.cols);
			queue->head->prev->bfs->y = y;
			w.map[y][mod(x+1,w.cols)].bfsvisited = 1;
		}
		if(w.map[y][mod(x-1,w.cols)].type != CELL_WATER && w.map[y][mod(x-1,w.cols)].bfsvisited == 0){
			List_append(queue);
			queue->head->prev->bfs->n = queue->head->bfs->n + 1;
			w.map[y][mod(x-1,w.cols)].bfs = queue->head->prev->bfs->n;
			queue->head->prev->bfs->x = mod(x-1,w.cols);
			queue->head->prev->bfs->y = y;
			w.map[y][mod(x-1,w.cols)].bfsvisited = 1;
		}
		List_remove(queue,queue->head);
	}	

}




worldmap assign_squads2(worldmap w){
	int defenders = 0;
	int soldiers = 0;
	/*if(w.razing == 0){
		defenders = w.ants->counter * 0;
		soldiers = w.ants->counter * 0;
	}else{*/
		defenders = w.ants->counter * 0;
		soldiers = w.ants->counter * 0.5;
		//fprintf(stderr, "assigning %d soldiers!\n", soldiers);
	//}
	Node *current = w.ants->head;
	for(int i = 0; i < soldiers; i++){
		/*if(current->item->squad == 0){
			w.ants->nrsquads = 0;
			for(int j = 0; j < 3; j++){
				if(w.ants->squadsize[j] >= 4){
					w.ants->nrsquads++;
				}else{
					w.ants->nrsquads++;
					current->item->squad = j;
					current->item->role = SOLDIER;
					w.ants->squadmembers[j][w.ants->squadsize[j]] = current->item->id;
					w.ants->squadsize[j]++;
					break;
				}
			}
		}*/
		current->item->role = SOLDIER;
		current = current->next;
	}
	for(int i = soldiers; i < defenders+soldiers; i++){
		current->item->squad = 0;
		current->item->role = DEFENDER;
		current = current->next;		
	}
	for(int i = soldiers+defenders; i < w.ants->counter; i++){
		current->item->squad = 0;
		current->item->role = GATHERER;
		current = current->next;
	}
		
}

void print_danger(worldmap w){
	fprintf(stderr, "printing danger map\n");
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			fprintf(stderr, "%d", w.map[i][j].danger);
		}
		fprintf(stderr, "\n");
	}
}

worldmap reset_danger(worldmap w){
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			w.map[i][j].danger = 0;
		}
	}
	return w;
}

worldmap set_danger(worldmap w, game_settings g, int x0, int y0){
	double radius = sqrt(g.attackradius2)+1;
	int radiusr = (int)radius;
	int error = pow(radiusr,2)-pow(radius,2);
	int x = radiusr;
	int y = 0;
	int friends = 0;
	int fight = 0;
	int ring = 0;
	int firstring = 0;
	while(x >= y){
		for(int a = x0-x-3; a <= x0+x+3; a++){
			if(a >= x0-x && a <= x0+x){
				w.map[mod(y0+y,w.rows)][mod(a,w.cols)].danger = 1;
				w.map[mod(y0-y,w.rows)][mod(a,w.cols)].danger = 1;
			}else{
				w.map[mod(y0+y,w.rows)][mod(a,w.cols)].danger = abs(a-x0)-x+1;
				w.map[mod(y0-y,w.rows)][mod(a,w.cols)].danger = abs(a-x0)-x+1;
				if(w.map[mod(y0+y,w.rows)][mod(a,w.cols)].type == CELL_ANT && w.map[mod(y0+y,w.rows)][mod(a,w.cols)].owner == 0){
					friends++;
					if(friends > 1){
						fight = 1;
						ring = abs(a-x0)-x+1;
					}else{
						firstring = abs(a-x0)-x+1;
					}
				}
				if(w.map[mod(y0-y,w.rows)][mod(a,w.cols)].type == CELL_ANT && w.map[mod(y0-y,w.rows)][mod(a,w.cols)].owner == 0){
					friends++;
					if(friends > 1){
						fight = 1;
						ring = abs(a-x0)-x+1;
					}else{
						firstring = abs(a-x0)-x+1;
					}
				}
			}
		}
		for(int a = x0-y-3; a <= x0+y+3; a++){
			if(a >= x0-y && a <= x0+y){
				w.map[mod(y0+x,w.rows)][mod(a,w.cols)].danger = 1;
				w.map[mod(y0-x,w.rows)][mod(a,w.cols)].danger = 1;
			}else{
				w.map[mod(y0+x,w.rows)][mod(a,w.cols)].danger = abs(a-x0)-y+1;
				w.map[mod(y0-x,w.rows)][mod(a,w.cols)].danger = abs(a-x0)-y+1;
				if(w.map[mod(y0+x,w.rows)][mod(a,w.cols)].type == CELL_ANT && w.map[mod(y0+x,w.rows)][mod(a,w.cols)].owner == 0){
					friends++;
					if(friends > 1){
						fight = 1;
						ring = abs(a-x0)-x+1;
					}else{
						firstring = abs(a-x0)-x+1;
					}
				}
				if(w.map[mod(y0-x,w.rows)][mod(a,w.cols)].type == CELL_ANT && w.map[mod(y0-x,w.rows)][mod(a,w.cols)].owner == 0){
					friends++;
					if(friends > 1){
						fight = 1;
						ring = abs(a-x0)-x+1;
					}else{
						firstring = abs(a-x0)-x+1;
					}
				}
			}
		}
		if(error <= 0){
			y+=1;
			error += 2*y + 1;
		}else{
			x-=1;
			error -= 2*x + 1;
		}
	}
	if(fight){
		//fprintf(stderr, "ants are fighting!\n");
		int fightready = 0;
		if(firstring == ring){
			fightready = 1;
		}
		for(int i = firstring; i <= ring; i++){ 
			error = pow(radiusr+i,2)-pow(radius+i,2);
			x = radiusr;
			y = 0;
			while(x>=y){
				if(w.map[mod(y0+y,w.rows)][mod(x0+x,w.cols)].type == CELL_ANT && w.map[mod(y0+y,w.rows)][mod(x0+x,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0+x,w.cols) && current->item->y == mod(y0+y,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0+x,w.rows)][mod(x0+y,w.cols)].type == CELL_ANT && w.map[mod(y0+x,w.rows)][mod(x0+y,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0+y,w.cols) && current->item->y == mod(y0+x,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0+x,w.rows)][mod(x0-y,w.cols)].type == CELL_ANT && w.map[mod(y0+x,w.rows)][mod(x0-y,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0-y,w.cols) && current->item->y == mod(y0+x,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0+y,w.rows)][mod(x0-x,w.cols)].type == CELL_ANT && w.map[mod(y0+y,w.rows)][mod(x0-x,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0-x,w.cols) && current->item->y == mod(y0+y,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0-y,w.rows)][mod(x0-x,w.cols)].type == CELL_ANT && w.map[mod(y0-y,w.rows)][mod(x0-x,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0-x,w.cols) && current->item->y == mod(y0-y,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0-x,w.rows)][mod(x0-y,w.cols)].type == CELL_ANT && w.map[mod(y0-x,w.rows)][mod(x0-y,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0-y,w.cols) && current->item->y == mod(y0-x,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0-x,w.rows)][mod(x0+y,w.cols)].type == CELL_ANT && w.map[mod(y0-x,w.rows)][mod(x0+y,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0+y,w.cols) && current->item->y == mod(y0-x,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(w.map[mod(y0-y,w.rows)][mod(x0+x,w.cols)].type == CELL_ANT && w.map[mod(y0-y,w.rows)][mod(x0+x,w.cols)].owner == 0){
					Node *current = w.ants->head;
					for(int j = 0; j < w.ants->counter; j++){
						if(current->item->x == mod(x0+x,w.cols) && current->item->y == mod(y0-y,w.rows)){
							current->item->fight = 1;
							current->item->fightturns = ring-i;
							current->item->fightready = fightready;
							break;
						}
						current = current->next;
					}
				}
				if(error <= 0){
					y+=1;
					error += 2*y + 1;
				}else{
					x-=1;
					error -= 2*x + 1;
				}
			}
		}
	}
	return w;
}





void print_vision(worldmap w){
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			fprintf(stderr, "%d", w.map[i][j].vision);
		}
		fprintf(stderr, "\n");
	}
}

worldmap reset_vision(worldmap w){
	for(int i = 0; i < w.rows; i++){
		for(int j = 0; j < w.cols; j++){
			w.map[i][j].vision = 0;
		}
	}
}

worldmap check_vision(worldmap w, game_settings g){
	Node *current = w.ants->head;
	for(int i = 0; i < w.ants->counter; i++){
		double radius = sqrt(g.viewradius2*2);
		int radiusr = (int)radius;
		int error = pow(radiusr,2)-pow(radius,2);
		int x = radiusr;
		int y = 0;
		int x0 = current->item->x;
		int y0 = current->item->y;
		while(x >= y){
			for(int a = x0-x; a <= x0+x; a++){
				if(w.map[mod(y0+y,w.rows)][mod(a,w.cols)].vision == 0 || w.map[mod(y0+y,w.rows)][mod(a,w.cols)].vision > abs(a-x0)+1+y){
					w.map[mod(y0+y,w.rows)][mod(a,w.cols)].vision = abs(a-x0)+1+y;
				}
				if(w.map[mod(y0-y,w.rows)][mod(a,w.cols)].vision == 0 || w.map[mod(y0-y,w.rows)][mod(a,w.cols)].vision > abs(a-x0)+1+y){
					w.map[mod(y0-y,w.rows)][mod(a,w.cols)].vision = abs(a-x0)+1+y;
				}
			}
			for(int a = x0-y; a <= x0+y; a++){
				if(w.map[mod(y0+x,w.rows)][mod(a,w.cols)].vision == 0 || w.map[mod(y0+x,w.rows)][mod(a,w.cols)].vision > abs(a-x0)+1+x){
					w.map[mod(y0+x,w.rows)][mod(a,w.cols)].vision = abs(a-x0)+1+x;
				}
				if(w.map[mod(y0-x,w.rows)][mod(a,w.cols)].vision == 0 || w.map[mod(y0-x,w.rows)][mod(a,w.cols)].vision > abs(a-x0)+1+x){
					w.map[mod(y0-x,w.rows)][mod(a,w.cols)].vision = abs(a-x0)+1+x;
				}
			}
			if(error <= 0){
				y+=1;
				error += 2*y + 1;
			}else{
				x-=1;
				error -= 2*x + 1;
			}
		}
		current = current->next;
	}
}

char vision_change(worldmap w, game_settings g, Node *current){
	double radius = sqrt(g.viewradius2);
	int counter;
	int radiusr = (int)radius;
	int error = pow(radiusr,2)-pow(radius,2);
	int x = radiusr;
	int y = 0;
	int x0 = 0;
	int y0 = 0;
	int new[4] = {0};
	for(int i = 0; i < 4; i++){
		counter = 0;
		if(i == 0){
			int x0 = current->item->x;
			int y0 = (current->item->y) -1;
		}else if(i==1){
			int x0 = (current->item->x) +1;
			int y0 = current->item->y;
		}else if(i==2){
			int x0 = current->item->x;
			int y0 = (current->item->y) +1;
		}else if(i==3){
			int x0 = (current->item->x) -1;
			int y0 = current->item->y;
		}
		while(x >= y){
			for(int a = x0-x; a <= x0+x; a++){
				if(w.map[mod(y0+y,w.rows)][mod(a,w.cols)].vision == 0){
					counter++;
				}
				if(w.map[mod(y0-y,w.rows)][mod(a,w.cols)].vision == 0){
					counter++;
				}
			}
			for(int a = x0-y; a <= x0+y; a++){
				if(w.map[mod(y0+x,w.rows)][mod(a,w.cols)].vision == 0){
					counter++;
				}
				if(w.map[mod(y0-x,w.rows)][mod(a,w.cols)].vision == 0){
					counter++;
				}
			}
			if(error <= 0){
				y+=1;
				error += 2*y + 1;
			}else{
				x-=1;
				error -= 2*x + 1;
			}
		}
		new[i] = counter;
	}
	char answer = 'x';
	int max = 0;
	for(int i = 0; i < 4; i++){
		if(new[i] >= max){
			if(i == 0 && w.map[mod((current->item->y-1),w.rows)][current->item->x].type != CELL_WATER&&w.map[mod((current->item->y-1),w.rows)][current->item->x].type != CELL_ANT){
				answer = 'N';
				max = new[i];
			}else if(i==1 && w.map[current->item->y][mod((current->item->x +1),w.cols)].type != CELL_WATER&&w.map[current->item->y][mod((current->item->x+1),w.cols)].type != CELL_ANT){
				answer = 'E';
				max = new[i];
			}else if(i==2 && w.map[mod((current->item->y +1),w.rows)][current->item->x].type != CELL_WATER&&w.map[mod((current->item->y+1),w.rows)][current->item->x].type != CELL_ANT){
				answer = 'S';
				max = new[i];
			}else if(i==3 && w.map[current->item->y][mod((current->item->x -1),w.cols)].type != CELL_WATER&&w.map[current->item->y][mod((current->item->x-1),w.cols)].type != CELL_ANT){
				answer = 'W';
				max = new[i];
			}
		}
	}
	return answer;
}











/*modulo*/

int mod(int a, int b){
	if(b < 0){ //you can check for b == 0 separately and do what you want
		return mod(a, -b);   
	}
	int mod = a % b;
	if(mod < 0){
		mod+=b;
	}
	return mod;
}















/*linked list commands*/

void List_print(ant *list){
	Node *start = list->head;
	for(int i = 0; i < list->counter; i++){
		fprintf(stderr,"-> id: %i\n",start->item->id);
		start = start->next;
	}
}

void List_append(ant *list){
	if(list->counter!=0){	
		Node *previous = list->head->prev;
		/*for(int i = 1; i < list->counter; i++){
			previous = previous->next;	
		}*/
		Node *new;
		Item *newitem; 
		BFS *newbfs;
		newitem = (Item *)malloc(sizeof(Item));
		newbfs = (BFS *)malloc(sizeof(BFS));
		new = (Node *)malloc(sizeof(Node));
		new->bfs = newbfs;
		newitem->id = list->idcounter;
		new->item = newitem;
		Node *next = previous->next;
		previous->next = new;
		new->prev = previous;
		new->next = next;
		next->prev = new;
		list->counter++;
		list->idcounter++;
	}else{
		Node *start;
		Item *item;
		BFS *bfs;
		item = (Item *)malloc(sizeof(Item));
		start = (Node *)malloc(sizeof(Node));
		bfs = (BFS *)malloc(sizeof(BFS));
		item->id = 0;
		start->item = item;
		start->bfs = bfs;
		start->next = start;
		start->prev = start;
		list->head = start;
		list->counter++;
	}
}

void List_putfirst(ant *list, Node *node){
	Node *previous = list->head;
	int found = 0;
	for(int i = 0; i < list->counter; i++){
		if(previous == node){
			found = 1;
			break;
		}else{	
			previous = previous->next;
		}	
	}
	if(found){
		Node *before = previous->prev;
		Node *after = previous->next;
		Node *last = list->head->prev;
		before->next = after;
		after->prev = before;		
		previous->next = list->head;
		previous->prev = last;
		list->head->prev = previous;
		last->next = previous;
		list->head = previous;	
	}else{
		printf("Error, node not found!\n");
	}

}

void List_sort(ant* list){
	int *id;
	Node *previous = list->head;
	id = (int *)malloc(sizeof(int)*list->counter);
	for(int i = 0; i < list->counter; i++){
		id[i] = previous->item->id;		
		previous = previous->next;
	}
	//bubble sort
	int finished = 0;
	while(!finished){
		finished = 1;
		for(int i = 0; i < (list->counter-1);i++){
			if(id[i+1] > id[i]){
				finished = 0;
				int temp = id[i];
				id[i] = id[i+1];
				id[i+1] = temp;
			}
		}
	}
	for(int i = 0; i < list->counter; i++){
	    Node* tomove = List_find(list,id[i]);
            List_putfirst(list,tomove);
	}

}

void List_remove(ant *list, Node *node){
	Node *previous = list->head;
	int found = 0;
	for(int i = 0; i < list->counter; i++){
		if(previous == node){
			found = 1;
			break;
		}else{	
			previous = previous->next;
		}	
	}
	if(found){
		//fprintf(stderr, "List before deleting ant %d\n",node->item->id);
		//List_print(list);
		//fprintf(stderr, "Deleted ant %d! New list will be printed:\n", node->item->id);
		if(previous == list->head){
			list->head = previous->next;
		}
		Node *before = previous->prev;
		Node *after = previous->next;
		before->next = after;
		after->prev = before;
		free(previous->item);
		free(previous);
		list->counter--;
		//List_print(list);
	}else{
		fprintf(stderr,"Error, node not found!\n");
	}
}

void List_destroy(ant* list){
	Node *previous = list->head;
	for(int i = 0; i < list->counter; i++){
		Node *next = previous->next;
		free(previous->item);
		free(previous);
		previous = next;
	}
	list->counter = 0;
}

void List_insert(ant *list){
	if(list->counter != 0){
		Node *next = list->head;
		Node *last = list->head->prev;
		Node *new;
		Item *newitem; 
		BFS *newbfs;
		newitem = (Item *)malloc(sizeof(Item));
		new = (Node *)malloc(sizeof(Node));
		newbfs = (BFS *)malloc(sizeof(BFS));
		newitem->id = list->idcounter;
		new->item = newitem;
		new->bfs = newbfs;
		next->prev = new;
		new->next = next;
		new->prev = last;
		last->next = new;
		list->head = new;
		list->counter++;
		list->idcounter++;
	}
	else{
		Node *start;
		Item *item;
		BFS *bfs;
		bfs = (BFS *)malloc(sizeof(BFS));
		item = (Item *)malloc(sizeof(Item));
		start = (Node *)malloc(sizeof(Node));
		item->id = 0;
		start->item = item;
		start->bfs = bfs;
		start->next = start;
		start->prev = start;
		list->head = start;
		list->counter++;
		list->idcounter++;
	}
}


ant *List_create(){
	ant *list;
	list = (ant *)malloc(sizeof(ant));
}



Node *List_find(ant *list, int id){
	Node *previous = list->head;
	for(int i = 0; i < list->counter; i++){
		if(previous->item->id == id){
			return previous;
		}else{
			previous = previous->next;	
		}
	}
	return 0;
}







































worldmap find_route(worldmap w, int Y, int X, int Yn, int Xn, Node *current)
{
	int i = Y;
	int j = X;
	current->item->targetx = Xn;
	current->item->targety = Yn;
	fprintf(stderr,"searching for a way to (%d,%d)\n",Xn, Yn);
	if(Yn > Y){
		if(w.map[i+1][j].type != CELL_WATER && w.map[i+1][j].type != CELL_ANT){
			printf("o %d %d S\n", i, j);
			fprintf(stderr,"order: o %d %d S\n",i, j);
			current->item->lastmove = S;
			w = update_ants(w,i,j,i+1,j, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}
		else if(Xn > X){
			if(w.map[i][j+1].type != CELL_WATER && w.map[i][j+1].type != CELL_ANT){
				printf("o %d %d E\n", i, j);
				fprintf(stderr,"order: o %d %d E\n",i, j);
				current->item->lastmove = E;
				w = update_ants(w,i,j,i,j+1, current);
				current->item->moving = 1;
				current->item->water = 0;
				return w;
			}
		}else if(Xn < X){
			if(w.map[i][j-1].type != CELL_WATER && w.map[i][j-1].type != CELL_ANT){
				printf("o %d %d W\n", i, j);
				fprintf(stderr,"order: o %d %d W\n",i, j);
				current->item->lastmove = W;
				w = update_ants(w,i,j,i,j-1, current);
				current->item->moving = 1;
				current->item->water = 0;
				return w;
			}
		}else{
			fprintf(stderr,"path blocked!\n");
			current->item->moving = 0;
			return w;
		}
	}else if(Yn < Y){
		if(w.map[i-1][j].type != CELL_WATER&&w.map[i-1][j].type != CELL_ANT){
			printf("o %d %d N\n", i, j);
			fprintf(stderr,"order: o %d %d N\n",i, j);
			current->item->lastmove = N;
			w = update_ants(w,i,j,i-1,j, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}
		else if((Xn > X)&&(w.map[i][j+1].type != CELL_WATER&&w.map[i][j+1].type != CELL_ANT)){
			printf("o %d %d E\n", i, j);
			fprintf(stderr,"order: o %d %d E\n",i, j);
			current->item->lastmove = E;
			w = update_ants(w,i,j,i,j+1, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}else if((Xn < X)&&(w.map[i][j-1].type != CELL_WATER&&w.map[i][j-1].type != CELL_ANT)){
			printf("o %d %d W\n", i, j);
			fprintf(stderr,"order: o %d %d W\n",i, j);
			current->item->lastmove = W;
			w = update_ants(w,i,j,i,j-1, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}else{
			fprintf(stderr,"path blocked!\n");
			current->item->moving = 0;
			return w;
		}

	}
	else if(Xn > X){
		if(w.map[i][j+1].type != CELL_WATER&&w.map[i][j+1].type != CELL_ANT){
			printf("o %d %d E\n", i, j);
			fprintf(stderr,"order: o %d %d E\n",i, j);
			current->item->lastmove = E;
			w = update_ants(w,i,j,i,j+1, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}
	}else if(Xn < X){
		if(w.map[i][j-1].type != CELL_WATER&&w.map[i][j-1].type != CELL_ANT){
			printf("o %d %d W\n", i, j);
			fprintf(stderr,"order: o %d %d W\n",i, j);
			current->item->lastmove = W;
			w = update_ants(w,i,j,i,j-1, current);
			current->item->moving = 1;
			current->item->water = 0;
			return w;
		}
	}else{
		fprintf(stderr,"path blocked!\n");
		current->item->moving = 0;
		return w;
	}
	fprintf(stderr,"path blocked!\n");
	current->item->moving = 0;
	return w;	
}

