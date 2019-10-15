#include<stdio.h> 
#include<fcntl.h>
#include<stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/stat.h>
#include "../inc/struct.h"
int size;
warehouseitem *itemlist;
long int clientlistid[4];
int convertToLower(char * stringy){
	int i;
	for (i = 0; stringy[i]; i++){
		stringy[i] = tolower(stringy[i]);
	}
	return 0;
}


void myHandle(int num){
	char input1[100];
	printf ("%s ","shell>");
	fgets(input1,30,stdin);

	input1[strcspn(input1, "\n")] = 0;

	char *firstWord;
	firstWord = strtok(input1," ");
	convertToLower(firstWord);
	if(strcmp(input1,"list")==0){


		char *theNumber = strtok(NULL," ");
		if(theNumber != NULL){
			int y;
			theNumber[strcspn(theNumber, "\n")] = 0;
			long int clientid;
			sscanf(theNumber,"%ld",&clientid);
			int x;
			for(x=0;x<4;x++){
				if(clientlistid[x] == clientid){
					goto foundid;
				}
			}
			printf("%s\n","There is no client with such id");
			goto finsh;
foundid:

			printf("%s\n","Here are the art from the this client");
			printf("%s\t%s\t%s\t%s\n","Art Name","Record ID","Client ID","Valid Bit(0 for valid 1 of used");
			for(y=0;y<size;y++){
				long int clientide;
				sscanf(itemlist[y].clientid,"%ld",&clientide);
				if(clientide == clientid){ 
					printf("%s\t%d\t%s\t%d\n",itemlist[y].artname,itemlist[y].recordid,itemlist[y].clientid,itemlist[y].useornot);
				}
			}
			goto finsh;
		}else{
			int p;
			printf("%s\t%s\n","Number(# of Clients)","Client Id");
			for(p=0;p<4;p++){
				if(clientlistid[p] != 9){
					printf("%d\t%ld\n",p+1,clientlistid[p]);
				}
			}
			goto finsh;
		}
	}else if(strcmp(firstWord,"dump")==0){
		int y;
		printf("%s\t%s\t%s\t%s\n","Art Name","Record ID","Client ID","Valid Bit(0 for valid 1 of used");
		for(y=0;y<size;y++){
			printf("%s\t\t%d\t\t%s\t%d\n",itemlist[y].artname,itemlist[y].recordid,itemlist[y].clientid,itemlist[y].useornot);
		}
		goto finsh;
	}else if(strcmp(firstWord,"exit")==0){
		free(itemlist);
		exit(0);
	}else{
		printf("%s\n","Commands not found");
		goto finsh;	
	}
finsh:
	return;
}


int main(int argc, char *argv[])
{
	if( argc == 1){
		printf("Please enter a size for database");
		exit(0);
	}
	int file8,file9;
	file8 = mkfifo("fifo_server",0666); 
	if(file8<0) {
		printf("Unable to create a fifo");
		exit(0);
	}

	file9 = mkfifo("fifo_client",0666);

	if(file9<0) {
		printf("Unable to create a fifo");
		exit(0);
	}

	//FILE *file1;
	int fifo_server,fifo_client;
	//int choice;
	char buf[1000];
	fifo_server = open("fifo_server",O_RDWR);
	if(fifo_server<1) {
		printf("Error opening file");
	}

	fifo_client = open("fifo_client",O_RDWR);

	if(fifo_server<1) {
		printf("Error opening file");
	}
	printf("Server on\n");
	int p;
	for (p=0;p<4;p++){
		clientlistid[p] =9;
	}


	sscanf(argv[1],"%d",&size);
	itemlist = malloc(size*sizeof(warehouseitem));
	int x;
	for (x=0;x<size;x++){
		strcpy(itemlist[x].artname,"null");
		itemlist[x].recordid = x;
		strcpy(itemlist[x].clientid,"99999999");
		itemlist[x].useornot = 0;
	}
	while(1){

		signal(SIGINT,myHandle);
		read(fifo_server,buf,1000);
		char *firstWord;
		firstWord = strtok(buf," ");
		sleep(1);
		if(strcmp("exit",buf)==0){
			printf("Server OFF\n");
			break;
		}else if(strcmp(firstWord,"threadid")==0){
			char *theNumber = strtok(NULL," ");
			long int clientidnumber;
			sscanf(theNumber,"%ld",&clientidnumber);
			int x;
			for(x=0;x<4;x++){
				if(clientlistid[x] == 9){
					clientlistid[x] = clientidnumber;
					goto done;
				}
			}
			int color;

done:
			for (color = 0;color < 4;color++){
				if(clientlistid[color]==clientidnumber){
					if(color ==0){
						printf("\033[1;31m");
						printf("%s%s\n","Has connected with a client with id ",theNumber);
						printf("\033[0m;");
					}else if(color == 1){
						printf("\033[1;32m");
						printf("%s%s\n","Has connected with a client with id ",theNumber);
						printf("\033[0m;");
					}else if(color == 2){
						printf("\033[1;33m");
						printf("%s%s\n","Has connected with a client with id ",theNumber);
						printf("\033[0m;");
					}else{
						printf("\033[1;34m");
						printf("%s%s\n","Has connected with a client with id ",theNumber);
						printf("\033[0m;");
					}
				}
			}
			write(fifo_client,theNumber,1000);
		}else if(strcmp(firstWord,"findaindex")==0){
			char *theNumber = strtok(NULL," ");
			long int clientidnumber;
			sscanf(theNumber,"%ld",&clientidnumber);
			int x;
			int theindexfree;
			for(x =0; x < size; x++){
				if (itemlist[x].useornot ==0){
					strcpy(itemlist[x].clientid,theNumber);
					itemlist[x].useornot = 1;
					theindexfree = x;
					goto foundindex;
				}
			}
			
			int colore;
			for (colore = 0;colore < 4;colore++){
				if(clientlistid[colore]==clientidnumber){
					if(colore ==0){
						printf("\033[1;31m");
						printf("Remote database couldn't find a spot for reserve for client %s\n",theNumber);
						printf("\033[0m;");
					}else if(colore == 1){
						printf("\033[1;32m");
						printf("Remote database couldn't find a spot for reserve for client %s\n",theNumber);
						printf("\033[0m;");
					}else if(colore == 2){
						printf("\033[1;33m");
						printf("Remote database couldn't find a spot for reserve for client %s\n",theNumber);
						printf("\033[0m;");
					}else{
						printf("\033[1;34m");
						printf("Remote database couldn't find a spot for reserve for client %s\n",theNumber);
						printf("\033[0m;");
					}
				}
			}
			char str[1000]="Remote database couldn't find a spot for reserve";
			write(fifo_client,str,1000);
			continue;
			int color;
foundindex:

			for (color = 0;color < 4;color++){
				if(clientlistid[color]==clientidnumber){
					if(color ==0){
						printf("\033[1;31m");
						printf("Remote database found a spot for reserve for client %s at index %d\n",theNumber,theindexfree);
						printf("\033[0m;");
					}else if(color == 1){
						printf("\033[1;32m");
						printf("Remote database found a spot for reserve for client %s at index %d\n",theNumber,theindexfree);
						printf("\033[0m;");
					}else if(color == 2){
						printf("\033[1;33m");
						printf("Remote database found a spot for reserve for client %s at index %d\n",theNumber,theindexfree);
						printf("\033[0m;");
					}else{
						printf("\033[1;34m");
						printf("Remote database found a spot for reserve for client %s at index %d\n",theNumber,theindexfree);
						printf("\033[0m;");
					}
				}
			}
			char hello[1000];
			sprintf(hello,"%d",theindexfree);
			write(fifo_client,hello,1000);	
		}else if(strcmp(firstWord,"deleteaindex")==0){
			char *theNumber = strtok(NULL," ");
			int mynumber;
			sscanf(theNumber,"%d",&mynumber);
			int x;
			for(x =0; x< size; x++){
				if(x == mynumber){
					strcpy(itemlist[x].artname,"null");
					long int clientidnumber;
					sscanf(itemlist[x].clientid,"%ld",&clientidnumber);
					int color;
					for (color = 0;color < 4;color++){
						if(clientlistid[color]==clientidnumber){
							if(color ==0){
								printf("\033[1;31m");
								printf("Remote database has remove the art entry at index %s for client %s\n",theNumber,itemlist[x].clientid);
								printf("\033[0m;");
							}else if(color == 1){
								printf("\033[1;32m");
								printf("Remote database has remove the art entry at index %s for client %s\n",theNumber,itemlist[x].clientid);
								printf("\033[0m;");
							}else if(color == 2){
								printf("\033[1;33m");
								printf("Remote database has remove the art entry at index %s for client %s\n",theNumber,itemlist[x].clientid);
								printf("\033[0m;");
							}else{
								printf("\033[1;34m");
								printf("Remote database has remove the art entry at index %s for client %s\n",theNumber,itemlist[x].clientid);
								printf("\033[0m;");
							}
						}
					}
					strcpy(itemlist[x].clientid,"99999999");
					itemlist[x].useornot = 0;
				}
			}

			char hello[1000]="The art entry has been deallocated.";
			write(fifo_client,hello,1000);	
		}else if(strcmp(firstWord,"findart")==0){
			char *theNumber = strtok(NULL," ");
			int mynumber;
			sscanf(theNumber,"%d",&mynumber);
			int x;
			for(x =0; x< size; x++){
				if(x == mynumber){
					if(strcmp(itemlist[x].artname,"null")==0){
						long int clientidnumber;
						sscanf(itemlist[x].clientid,"%ld",&clientidnumber);
						int color;
						for (color = 0;color < 4;color++){
							if(clientlistid[color]==clientidnumber){
								if(color ==0){
									printf("\033[1;31m");
									printf("There was no art name for which client %s was searching for.\n",itemlist[x].clientid);
									printf("\033[0m;");
								}else if(color == 1){
									printf("\033[1;32m");
									printf("There was no art name for which client %s was searching for.\n",itemlist[x].clientid);
									printf("\033[0m;");
								}else if(color == 2){
									printf("\033[1;33m");
									printf("There was no art name for which client %s was searching for.\n",itemlist[x].clientid);
									printf("\033[0m;");
								}else{
									printf("\033[1;34m");
									printf("There was no art name for which client %s was searching for.\n",itemlist[x].clientid);
									printf("\033[0m;");
								}
							}
						}
						char hello[1000]="There was no name.";
						write(fifo_client,hello,1000);	
					}else{
						long int clientidnumber;
						sscanf(itemlist[x].clientid,"%ld",&clientidnumber);
						int color;
						for (color = 0;color < 4;color++){
							if(clientlistid[color]==clientidnumber){
								if(color ==0){
									printf("\033[1;31m");
									printf("Found the art name: %s for client %s\n",itemlist[x].artname,itemlist[x].clientid);
									printf("\033[0m;");
								}else if(color == 1){
									printf("\033[1;32m");
									printf("Found the art name: %s for client %s\n",itemlist[x].artname,itemlist[x].clientid);
									printf("\033[0m;");
								}else if(color == 2){
									printf("\033[1;33m");
									printf("Found the art name: %s for client %s\n",itemlist[x].artname,itemlist[x].clientid);
									printf("\033[0m;");
								}else{
									printf("\033[1;34m");
									printf("Found the art name: %s for client %s\n",itemlist[x].artname,itemlist[x].clientid);
									printf("\033[0m;");
								}
							}
						}
						write(fifo_client,itemlist[x].artname,1000);
					}
				}
			}
		}else if(strcmp(firstWord,"insertart")==0){
			char *theNumber = strtok(NULL," ");
			char *theArt = strtok(NULL,"");
			int mynumber;
			sscanf(theNumber,"%d",&mynumber);
			int x;
			for(x =0; x< size; x++){
				if(x == mynumber){
					strcpy(itemlist[x].artname,theArt);
					long int clientidnumber;
					sscanf(itemlist[x].clientid,"%ld",&clientidnumber);
					int color;
					for (color = 0;color < 4;color++){
						if(clientlistid[color]==clientidnumber){
							if(color ==0){
								printf("\033[1;31m");
								printf("Client %s set record %d to %s.\n",itemlist[x].clientid,itemlist[x].recordid,theArt);
								printf("\033[0m;");
							}else if(color == 1){
								printf("\033[1;32m");
								printf("Client %s set record %d to %s.\n",itemlist[x].clientid,itemlist[x].recordid,theArt);
								printf("\033[0m;");
							}else if(color == 2){
								printf("\033[1;33m");
								printf("Client %s set record %d to %s.\n",itemlist[x].clientid,itemlist[x].recordid,theArt);
								printf("\033[0m;");
							}else{
								printf("\033[1;34m");
								printf("Client %s set record %d to %s.\n",itemlist[x].clientid,itemlist[x].recordid,theArt);
								printf("\033[0m;");
							}
						}
					}

					char hello[1000]="Remote database has sucessfully change art name for the art entry.";
					write(fifo_client,hello,1000);	
				}
			}

		}else if(strcmp(firstWord,"clearit")==0){
			char *theNumber = strtok(NULL," ");
			int x;
			for(x=0;x<size;x++){
				if(strcmp(itemlist[x].clientid,theNumber)==0){

					strcpy(itemlist[x].artname,"null");
					itemlist[x].recordid = x;
					strcpy(itemlist[x].clientid,"99999999");
					itemlist[x].useornot = 0;
				}
			}

			long int clientid;
			sscanf(theNumber,"%ld",&clientid);
			int color;
			for (color = 0;color < 4;color++){
				if(clientlistid[color]==clientid){
					if(color ==0){
						printf("\033[1;31m");
						printf("Freed all art entries for client %s and has disconnected\n",theNumber);
						printf("\033[0m;");
					}else if(color == 1){
						printf("\033[1;32m");
						printf("Freed all art entries for client %s and has disconnected\n",theNumber);
						printf("\033[0m;");
					}else if(color == 2){
						printf("\033[1;33m");
						printf("Freed all art entries for client %s and has disconnected\n",theNumber);
						printf("\033[0m;");
					}else{
						printf("\033[1;34m");
						printf("Freed all art entries for client %s and has disconnected\n",theNumber);
						printf("\033[0m;");
					}
				}
			}

			int pp;
			for(pp=0; pp< 4;pp++){
				if(clientlistid[pp] == clientid){
					clientlistid[pp] = 9;
				}
			}

			char hello[1000]="All art entries has been freed.";
			write(fifo_client,hello,1000);
		}
		else if(strcmp("",buf)!=0){
			printf("%s\n",buf);
			write(fifo_client,buf,1000);
		}
		memset(buf,0,sizeof(buf));
	}

	close(fifo_server);
	close(fifo_client);
	unlink("fifo_server");
	unlink("fifo_client");
	return 0;
}
