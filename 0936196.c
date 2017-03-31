#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "0936196.h"

#include <time.h>



void main(void)
{
	clock_t start = clock(), diff;
	worldmap world;
	game_settings settings;
	int previousraze = 0;
	//fprintf(stderr,"Initializing world!\n");
	read_initialization(&settings);
	world.map = NULL;
	world.rows = settings.rows;
	world.cols = settings.cols;
	world.end = 0;
	world.turn = 0;
	//fprintf(stderr,"Initializing map!\n");
	world = initialize_map(world);
	printf("go\n");
	fflush(stdout);
	//fprintf(stderr, "Processed initialion\n");
	while(!world.end){
		diff = clock() - start;
		int msec = diff * 1000 / CLOCKS_PER_SEC;
		fprintf(stderr,"Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);
		clock_t start = clock(), diff;
		world.razing = 0;
		world = reset_danger(world);
		world = read_turn(world, settings);
		world = check_vision(world, settings);
		world = check_territory(world);
		//print_map(world);
		if(world.razing == 1 && previousraze == 0){
			fprintf(stderr, "still razing!\n");
			world = assign_squads(world);
		}else if(world.razing == 0){
			fprintf(stderr, "no razing!\n");
			Node *current = world.ants->head;
			for(int i = 0; i < world.ants->counter; i++){
				if(current->item->role == SOLDIER){
					current->item->role = GATHERER;
					current = current->next;
				}
			}
		}
		world = send_orders(world);
		//List_print(world.ants);
		if(world.razing){
			previousraze = 1;
		}else{
			previousraze = 0;
		}
		printf("go\n");
		fprintf(stderr, "Turn %d , end = %s \n" , world.turn , world.end ? "true" : "false" ) ;
		fflush(stdout);
		fprintf(stderr, "\n");
		//print_map(world);
	}
	//print_map(world);
	List_destroy(world.ants);
	world = cleanup_map(world);
}

worldmap send_orders(worldmap w){
	//fprintf(stderr, "checking row %d and col %d with type %d and owner %d\n", i,j,w.map[i][j].type,w.map[i][j].owner);
	Node *current = w.ants->head;	
	for(int z = 0; z < w.ants->counter; z++){
		fprintf(stderr, "Searching move for ant %d\n",current->item->id);	
		if(current->item->alive == 1){
			if(current->item->fight == 0){
				int i = current->item->y;
				int j = current->item->x;
				if(current->item->id < w.territories && w.map[i][j].ant != current->item->id && w.ants->counter > 5){
					current->item->routes = search_territory(w,current);
					w = moving(w, i, j, current);
				}else if(current->item->id < w.territories && w.ants->counter > 5){
					current->item->routes = search(w,current,CELL_FOOD);
					fprintf(stderr, "searching for food in territory, ant: %d\n", current->item->id);
					w = moving(w, i, j, current);
				}else{
					if(current->item->role == GATHERER){
						if(w.ants->counter > 5){
							fprintf(stderr, "ant %d is a gatherer!\n", current->item->id);	
							current->item->routes = search_vision(w,current);
							current->item->routenr = 0;
							if(strcmp(current->item->routes->routelist, "x") != 0){		
								fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
								w = moving(w, i, j, current);

							}
						}else{	
							current->item->routes = search_food(w,current);
							current->item->routenr = 0;
							if(strcmp(current->item->routes->routelist, "x") != 0){		
								fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
								w = moving(w, i, j, current);

							}	
						}					
					}else if(current->item->role == SOLDIER){
						fprintf(stderr, "ant %d is a soldier!\n", current->item->id);	
						current->item->routes = search(w,current,CELL_HILL);
						current->item->routenr = 0;
						if(strcmp(current->item->routes->routelist, "x") != 0){	
							fprintf(stderr, "ant %d found a hill!\n", current->item->id);		
							fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
							w = moving(w, i, j, current);
						}else{
							current->item->routes = search(w,current,CELL_ANT_ON_HILL);
							current->item->routenr = 0;
							if(strcmp(current->item->routes->routelist, "x") != 0){	
								fprintf(stderr, "ant %d found a hill!\n", current->item->id);		
								fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
								w = moving(w, i, j, current);
							}
						}			
					}else{
					
					}
				}
			}else{
				

			}
		current = current->next;
		fprintf(stderr, "the ant was alive and a move was found!\n the next ant to check will become: %d\n", current->item->id);
		}
		else{
			Node *next = current->next;
			fprintf(stderr, "The ant has been removed! the next ant to check will become: %d\n", current->item->id);
			w.map[current->item->y][current->item->x].type = CELL_DIRT;
			w.map[current->item->y][current->item->x].owner = -1;
			List_remove(w.ants, current);
			current = next;
		} 
	}
	return w;	
}


