#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "0936196.h"

worldmap world;
game_settings settings;

void main(void)
{
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
		world.razing = 0;
		world = reset_vision(world);
		world = reset_danger(world);
		world = read_turn(world, settings);
		world = check_vision(world, settings);
		//print_map(world);
		if(world.razing == 1 && previousraze == 0){
			fprintf(stderr, "still razing!\n");
			world = assign_squads(world);
		}else if(world.razing == 0){
			fprintf(stderr, "no razing!\n");
			Node *current = world.ants->head;
			for(int i = 0; i < world.ants->counter; i++){
				current->item->role = GATHERER;
				current = current->next;
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
	world = cleanup_map(world);
}

worldmap send_orders(worldmap w){
			//fprintf(stderr, "checking row %d and col %d with type %d and owner %d\n", i,j,w.map[i][j].type,w.map[i][j].owner);
	int move = -1;
	Node *current = w.ants->head;	
	for(int z = 0; z < w.ants->counter; z++){
		fprintf(stderr, "Searching move for ant %d\n",current->item->id);	
		if(current->item->alive == 1){
			int i = current->item->y;
			int j = current->item->x;
			if(current->item->moving == 0){
				if(current->item->role == GATHERER){
					if(current->item->fight == 0){
						fprintf(stderr, "ant %d is a gatherer!\n", current->item->id);				
						int route_found = 0;
						if(w.ants->counter > 10){
							current->item->routes = search(w,i,j, current, 8);
						}else{
							current->item->routes = search(w,i,j, current, 25);
						}
						current->item->routenr = 0;
						if(strcmp(current->item->routes->routelist, "x") != 0){
							current->item->moving = 0;
							w.map[current->item->routes->y][current->item->routes->x].found++;
							if(w.map[current->item->routes->y][current->item->routes->x].found>2){
								w.map[current->item->routes->y][current->item->routes->x].type = CELL_DIRT; //****leave?****
							}
							fprintf(stderr, "ant %d is now on his way!\n", current->item->id);
						}else{
							current->item->routes = search_vision(w,i,j,current);
							if(strcmp(current->item->routes->routelist, "x") != 0){
								current->item->moving = 0;
								//w.map[current->item->routes.y][current->item->routes.x].type = CELL_DIRT;
								//fprintf(stderr, "ant %d is now on his way!\n", current->item->id);
							}else{
								int max = w.map[current->item->y][current->item->x].vision;
								int new[4] = {0};
								new[0] = w.map[mod((current->item->y-1),w.rows)][current->item->x].vision;
								new[1] = w.map[current->item->y][mod((current->item->x +1),w.cols)].vision;
								new[2] = w.map[mod((current->item->y +1),w.rows)][current->item->x].vision;
								new[3] = w.map[current->item->y][mod((current->item->x -1),w.cols)].vision;
								char move = 'x';
								for(int b = 0; b < 4; b++){
									if(new[b] >= max){
										if(i == 0 && w.map[mod((current->item->y-1),w.rows)][current->item->x].type != CELL_WATER&&w.map[mod((current->item->y-1),w.rows)][current->item->x].type != CELL_ANT&&w.map[mod((current->item->y-1),w.rows)][current->item->x].danger == 0){
											move = 'N';
											max = new[i];
										}else if(i==1 && w.map[current->item->y][mod((current->item->x +1),w.cols)].type != CELL_WATER&&w.map[current->item->y][mod((current->item->x+1),w.cols)].type != CELL_ANT&&w.map[current->item->y][mod((current->item->x+1),w.cols)].danger == 0){
											move = 'E';
											max = new[i];
										}else if(i==2 && w.map[mod((current->item->y +1),w.rows)][current->item->x].type != CELL_WATER&&w.map[mod((current->item->y+1),w.rows)][current->item->x].type != CELL_ANT&&w.map[mod((current->item->y+1),w.rows)][current->item->x].danger == 0){
											move = 'S';
											max = new[i];
										}else if(i==3 && w.map[current->item->y][mod((current->item->x -1),w.cols)].type != CELL_WATER&&w.map[current->item->y][mod((current->item->x-1),w.cols)].type != CELL_ANT &&w.map[current->item->y][mod((current->item->x-1),w.cols)].danger == 0){
											move = 'W';
											max = new[i];
										}
									}
								}
								switch(move){
									case 'N':
										if(w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){ 
											printf("o %d %d N\n", i, j);
											//fprintf(stderr,"order: o %d %d N\n", i, j);
											w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
											current->item->lastmove = N;
										}
									break;
									case 'E':
										if(w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){ 
											printf("o %d %d E\n", i, j);
											//fprintf(stderr,"order: o %d %d E\n", i, j);
											w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
											current->item->lastmove = E;
										}
									break;
									case 'S':
										if(w.map[mod((i+1),w.rows)][j].type != CELL_WATER&&w.map[mod((i+1),w.rows)][j].type != CELL_ANT){ 
											printf("o %d %d S\n", i, j);
											//fprintf(stderr,"order: o %d %d S\n", i, j);
											w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
											current->item->lastmove = S;
										}
									break;
									case 'W':
										if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){ 
											printf("o %d %d W\n", i, j);
											//fprintf(stderr,"order: o %d %d W\n",i, j);
											w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
											current->item->lastmove = W;
										}	
									break;
									default:
										//fprintf(stderr,"invalid move\n");
									break;							

								}
							}
						
						}
					}else{
						if(current->item->fightready == 0){
							if(current->item->fightturns != 0){
								int x = current->item->x;
								int y = current->item->y;
								int number = w.map[y][x].danger;
								if(w.map[mod(y-1,w.rows)][x].danger == number+1 && w.map[mod((y-1),w.rows)][x].type != CELL_WATER&&w.map[mod((y-1),w.rows)][x].type != CELL_ANT){
									printf("o %d %d N\n", y, x);
									//fprintf(stderr,"order: o %d %d N\n", y, x);
									w = update_ants(w,y,x,mod(y-1,w.rows),x,current);
									current->item->lastmove = N;
									current->item->fightturns--;
								}else if(w.map[mod(y+1,w.rows)][x].danger == number+1 &&w.map[mod((y+1),w.rows)][x].type != CELL_WATER&&w.map[mod((y+1),w.rows)][x].type != CELL_ANT){
									printf("o %d %d S\n", y, x);
									//fprintf(stderr,"order: o %d %d S\n", y, x);
									w = update_ants(w,y,x,mod(y+1,w.rows),x,current);
									current->item->lastmove = N;
									current->item->fightturns--;
								}else if(w.map[y][mod((x-1),w.cols)].danger == number+1 && w.map[y][mod((x-1),w.cols)].type != CELL_WATER&&w.map[y][mod((x-1),w.cols)].type != CELL_ANT){
									printf("o %d %d W\n", y, x);
									//fprintf(stderr,"order: o %d %d W\n",y, x);
									w = update_ants(w,y,x,y,mod(x-1,w.cols), current);
									current->item->lastmove = W;
									current->item->fightturns--;
								}else if(w.map[y][mod((x+1),w.cols)].danger == number+1 && w.map[y][mod((x+1),w.cols)].type != CELL_WATER&&w.map[y][mod((x+1),w.cols)].type != CELL_ANT){
									printf("o %d %d E\n", y, x);
									//fprintf(stderr,"order: o %d %d E\n",y, x);
									w = update_ants(w,y,x,y,mod(x+1,w.cols), current);
									current->item->lastmove = W;
									current->item->fightturns--;
								}		
							}
						}else{
							int x = current->item->x;
							int y = current->item->y;
							int number = w.map[y][x].danger;
							if(w.map[mod(y-1,w.rows)][x].danger == number-1 && w.map[mod((y-1),w.rows)][x].type != CELL_WATER&&w.map[mod((y-1),w.rows)][x].type != CELL_ANT){
								printf("o %d %d N\n", y, x);
								//fprintf(stderr,"order: o %d %d N\n", y, x);
								w = update_ants(w,y,x,mod(y-1,w.rows),x,current);
								current->item->lastmove = N;
								current->item->fightturns--;
							}else if(w.map[mod(y+1,w.rows)][x].danger == number-1 &&w.map[mod((y+1),w.rows)][x].type != CELL_WATER&&w.map[mod((y+1),w.rows)][x].type != CELL_ANT){
								printf("o %d %d S\n", y, x);
								//fprintf(stderr,"order: o %d %d S\n", y, x);
								w = update_ants(w,y,x,mod(y+1,w.rows),x,current);
								current->item->lastmove = N;
								current->item->fightturns--;
							}else if(w.map[y][mod((x-1),w.cols)].danger == number-1 && w.map[y][mod((x-1),w.cols)].type != CELL_WATER&&w.map[y][mod((x-1),w.cols)].type != CELL_ANT){
								printf("o %d %d W\n", y, x);
								//fprintf(stderr,"order: o %d %d W\n",y, x);
								w = update_ants(w,y,x,y,mod(x-1,w.cols), current);
								current->item->lastmove = W;
								current->item->fightturns--;
							}else if(w.map[y][mod((x+1),w.cols)].danger == number-1 && w.map[y][mod((x+1),w.cols)].type != CELL_WATER&&w.map[y][mod((x+1),w.cols)].type != CELL_ANT){
								printf("o %d %d E\n", y, x);
								//fprintf(stderr,"order: o %d %d E\n",y, x);
								w = update_ants(w,y,x,y,mod(x+1,w.cols), current);
								current->item->lastmove = W;
								current->item->fightturns--;
							}		

						}
					}
					if(current->item->moving == 0){ //shoulde be 0
						/*int friends = 0;
						int enemies = 0;
						for(int a = -5; a <=5 ; a++){
							for(int b = -5; b <= 5; b++){
								if(w.map[mod(i+a,w.rows)][mod(j+b,w.cols)].type == CELL_ANT){
									if(w.map[mod(i+a,w.rows)][mod(j+b,w.cols)].owner == 0){
										friends++;
									}else{
										enemies++;
									}
								}
							}
						}if(enemies > friends || friends < 10){
							if(move!= 0){
								move = 0;
								world = check_moves(world,0);
							}
						}else{
							if(move!= 4){
								move = 4;
								world = check_moves(world,4);
							}
						}
						if(w.map[mod(i+1,w.rows)][j].move > w.map[mod(i-1,w.rows)][j].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j+1,w.cols)].move && w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_WATER&&w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_ANT){
							printf("o %d %d S\n", i, j);
							fprintf(stderr,"order: o %d %d S\n", i, j);
							w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
							current->item->lastmove = S;			
						}else if(w.map[i][mod(j+1,w.cols)].move > w.map[mod(i-1,w.rows)][j].move && w.map[i][mod(j+1,w.cols)].move > w.map[i][mod(j-1,w.cols)].move && w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){
							printf("o %d %d E\n", i, j);
							fprintf(stderr,"order: o %d %d E\n", i, j);
							w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
							current->item->lastmove = E;
						}else if(w.map[mod(i-1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){
							printf("o %d %d N\n", i, j);
							fprintf(stderr,"order: o %d %d N\n", i, j);
							w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
							current->item->lastmove = N;
						}else if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){
							printf("o %d %d W\n", i, j);
							fprintf(stderr,"order: o %d %d W\n",i, j);
							w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
							current->item->lastmove = W;
						}else{
							fprintf(stderr,"no good move\n");
						}*/
						if(current->item->routes != NULL){
							if(current->item->routes->routelist != NULL){
								//fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
								int i = current->item->y;
								int j = current->item->x;
								if(i != current->item->targety || j != current->item->targetx){
									switch(current->item->routes->routelist[current->item->routenr]){
										case 'N':
											if(w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){ 
												printf("o %d %d N\n", i, j);
												//fprintf(stderr,"order: o %d %d N\n", i, j);
												w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
												current->item->lastmove = N;
												current->item->routenr++;
											}
										break;
										case 'E':
											if(w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){ 
												printf("o %d %d E\n", i, j);
												//fprintf(stderr,"order: o %d %d E\n", i, j);
												w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
												current->item->lastmove = E;
												current->item->routenr++;
											}
										break;
										case 'S':
											if(w.map[mod((i+1),w.rows)][j].type != CELL_WATER&&w.map[mod((i+1),w.rows)][j].type != CELL_ANT){ 
												printf("o %d %d S\n", i, j);
												//fprintf(stderr,"order: o %d %d S\n", i, j);
												w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
												current->item->lastmove = S;
												current->item->routenr++;
					
											}
										break;
										case 'W':
											if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){ 
												printf("o %d %d W\n", i, j);
												//fprintf(stderr,"order: o %d %d W\n",i, j);
												w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
												current->item->lastmove = W;
												current->item->routenr++;
			
											}	
										break;
										default:
											//fprintf(stderr,"invalid move\n");
										break;
									}
								}
							}
						}
					}
					
				}else if(current->item->role == SOLDIER){
					fprintf(stderr, "ant %d is a soldier!\n", current->item->id);	
					current->item->routes = search_hill(w,i,j, current);
					current->item->routenr = 0;
					if(strcmp(current->item->routes->routelist, "x") != 0){	
						fprintf(stderr, "ant %d found a hill!\n", current->item->id);		
						//fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
						int i = current->item->y;
						int j = current->item->x;
						if(i != current->item->targety || j != current->item->targetx){
							switch(current->item->routes->routelist[current->item->routenr]){
								case 'N':
									if(w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){ 
										printf("o %d %d N\n", i, j);
										//fprintf(stderr,"order: o %d %d N\n", i, j);
										w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
										current->item->lastmove = N;
										current->item->routenr++;
									}
								break;
								case 'E':
									if(w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){ 
										printf("o %d %d E\n", i, j);
										//fprintf(stderr,"order: o %d %d E\n", i, j);
										w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
										current->item->lastmove = E;
										current->item->routenr++;
									}
								break;
								case 'S':
									if(w.map[mod((i+1),w.rows)][j].type != CELL_WATER&&w.map[mod((i+1),w.rows)][j].type != CELL_ANT){ 
										printf("o %d %d S\n", i, j);
										//fprintf(stderr,"order: o %d %d S\n", i, j);
										w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
										current->item->lastmove = S;
										current->item->routenr++;
			
									}
								break;
								case 'W':
									if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){ 
										printf("o %d %d W\n", i, j);
										//fprintf(stderr,"order: o %d %d W\n",i, j);
										w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
										current->item->lastmove = W;
										current->item->routenr++;
	
									}	
								break;
								default:
									//fprintf(stderr,"invalid move\n");
								break;
							}
						}

					}else{
						fprintf(stderr, "No hill found\n");
					}			
					/*int x = current->item->x;
					int y = current->item->y;
					int friends = 0;
					int enemies = 0;
					int squadready = 0;
					for(int a = -5; a <=5 ; a++){
						for(int b = -5; b <= 5; b++){
							if(w.map[mod(i+a,w.rows)][mod(j+b,w.cols)].type == CELL_ANT){
								if(w.map[mod(i+a,w.rows)][mod(j+b,w.cols)].owner == 0){
									friends++;
								}else{
									enemies++;
								}
							}
						}
					}
					if(friends > enemies){
						squadready = 1;
					}
					if(squadready){
						if(move!=1){
							move = 1;
							world = check_moves(world,1);
							fprintf(stderr, "ant %d is a soldier!\n", current->item->id);	
							if(w.map[mod(i+1,w.rows)][j].move > w.map[mod(i-1,w.rows)][j].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j+1,w.cols)].move && w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_WATER&&w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_ANT){
								printf("o %d %d S\n", i, j);
								fprintf(stderr,"order: o %d %d S\n", i, j);
								w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
								current->item->lastmove = S;			
							}else if(w.map[i][mod(j+1,w.cols)].move > w.map[mod(i-1,w.rows)][j].move && w.map[i][mod(j+1,w.cols)].move > w.map[i][mod(j-1,w.cols)].move && w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){
								printf("o %d %d E\n", i, j);
								fprintf(stderr,"order: o %d %d E\n", i, j);
								w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
								current->item->lastmove = E;
							}else if(w.map[mod(i-1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){
								printf("o %d %d N\n", i, j);
								fprintf(stderr,"order: o %d %d N\n", i, j);
								w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
								current->item->lastmove = N;
							}else if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){
								printf("o %d %d W\n", i, j);
								fprintf(stderr,"order: o %d %d W\n",i, j);
								w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
								current->item->lastmove = W;
							}else{
								fprintf(stderr,"no good move\n");
							}
						}
					}else{
						if(move != 2){
							move = 2;
							world = check_moves(world,2);
							fprintf(stderr, "ant %d is a soldier who wants to find his squad!\n", current->item->id);	
							if(w.map[mod(i+1,w.rows)][j].move > w.map[mod(i-1,w.rows)][j].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j+1,w.cols)].move && w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_WATER&&w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_ANT){
								printf("o %d %d S\n", i, j);
								fprintf(stderr,"order: o %d %d S\n", i, j);
								w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
								current->item->lastmove = S;			
							}else if(w.map[i][mod(j+1,w.cols)].move > w.map[mod(i-1,w.rows)][j].move && w.map[i][mod(j+1,w.cols)].move > w.map[i][mod(j-1,w.cols)].move && w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){
								printf("o %d %d E\n", i, j);
								fprintf(stderr,"order: o %d %d E\n", i, j);
								w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
								current->item->lastmove = E;
							}else if(w.map[mod(i-1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){
								printf("o %d %d N\n", i, j);
								fprintf(stderr,"order: o %d %d N\n", i, j);
								w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
								current->item->lastmove = N;
							}else if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){
								printf("o %d %d W\n", i, j);
								fprintf(stderr,"order: o %d %d W\n",i, j);
								w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
								current->item->lastmove = W;
							}else{
								fprintf(stderr,"no good move\n");
							}
						}
					}
					*/
					
				}else{
					//fprintf(stderr, "ant %d is a defender!\n", current->item->id);	
					if(move!= 3){
						move = 3;
						world = check_moves(world,3);
					}
					if(w.map[mod(i+1,w.rows)][j].move > w.map[mod(i-1,w.rows)][j].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod(i+1,w.rows)][j].move > w.map[i][mod(j+1,w.cols)].move && w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_WATER&&w.map[mod((mod(i+1,w.rows)),w.rows)][j].type != CELL_ANT){
						printf("o %d %d S\n", i, j);
						//fprintf(stderr,"order: o %d %d S\n", i, j);
						w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
						current->item->lastmove = S;			
					}else if(w.map[i][mod(j+1,w.cols)].move > w.map[mod(i-1,w.rows)][j].move && w.map[i][mod(j+1,w.cols)].move > w.map[i][mod(j-1,w.cols)].move && w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){
						printf("o %d %d E\n", i, j);
						//fprintf(stderr,"order: o %d %d E\n", i, j);
						w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
						current->item->lastmove = E;
					}else if(w.map[mod(i-1,w.rows)][j].move > w.map[i][mod(j-1,w.cols)].move && w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){
						printf("o %d %d N\n", i, j);
						//fprintf(stderr,"order: o %d %d N\n", i, j);
						w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
						current->item->lastmove = N;
					}else if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){
						printf("o %d %d W\n", i, j);
						//fprintf(stderr,"order: o %d %d W\n",i, j);
						w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
						current->item->lastmove = W;
					}else{
						//fprintf(stderr,"no good move\n");
					}			
				}
			}
			else{
				if(current->item->routes != NULL){
					if(current->item->routes->routelist != NULL){
						//fprintf(stderr, "Ant %d is still moving towards target!\n It will try to move in %c from (%d,%d)\n", current->item->id, current->item->routes->routelist[current->item->routenr],current->item->x,current->item->y);
						int i = current->item->y;
						int j = current->item->x;
						if(i != current->item->targety || j != current->item->targetx){
							switch(current->item->routes->routelist[current->item->routenr]){
								case 'N':
									if(w.map[mod((i-1),w.rows)][j].type != CELL_WATER&&w.map[mod((i-1),w.rows)][j].type != CELL_ANT){ 
										printf("o %d %d N\n", i, j);
										//fprintf(stderr,"order: o %d %d N\n", i, j);
										w = update_ants(w,i,j,mod(i-1,w.rows),j,current);
										current->item->lastmove = N;
										current->item->routenr++;
									}
								break;
								case 'E':
									if(w.map[i][mod((j+1),w.cols)].type != CELL_WATER&&w.map[i][mod((j+1),w.cols)].type != CELL_ANT){ 
										printf("o %d %d E\n", i, j);
										//fprintf(stderr,"order: o %d %d E\n", i, j);
										w = update_ants(w,i,j,i,mod(j+1,w.cols),current);
										current->item->lastmove = E;
										current->item->routenr++;
									}
								break;
								case 'S':
									if(w.map[mod((i+1),w.rows)][j].type != CELL_WATER&&w.map[mod((i+1),w.rows)][j].type != CELL_ANT){ 
										printf("o %d %d S\n", i, j);
										//fprintf(stderr,"order: o %d %d S\n", i, j);
										w = update_ants(w,i,j,mod(i+1,w.rows),j,current);
										current->item->lastmove = S;
										current->item->routenr++;
					
									}
								break;
								case 'W':
									if(w.map[i][mod((j-1),w.cols)].type != CELL_WATER&&w.map[i][mod((j-1),w.cols)].type != CELL_ANT){ 
										printf("o %d %d W\n", i, j);
										//fprintf(stderr,"order: o %d %d W\n",i, j);
										w = update_ants(w,i,j,i,mod(j-1,w.cols), current);
										current->item->lastmove = W;
										current->item->routenr++;
			
									}	
								break;
								default:
									//fprintf(stderr,"invalid move\n");
								break;
							}
				
						}else{
							//fprintf(stderr,"order finished!'(\n");
							current->item->moving = 0;
						}
					}
				}
			}
		current = current->next;
		//fprintf(stderr, "the ant was alive and a move was found!\n the next ant to check will become: %d\n", current->item->id);
		}
		else{
			Node *next = current->next;
			//fprintf(stderr, "The ant has been removed! the next ant to check will become: %d\n", current->item->id);
			world.map[current->item->y][current->item->x].type = CELL_DIRT;
			world.map[current->item->y][current->item->x].owner = -1;
			List_remove(world.ants, current);
			current = next;
		} //pls be the correct position <3
	}
	return w;	
}


