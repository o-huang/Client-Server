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
firststuff *arrayfirstpage;
quickstuff *cachearray;
int fifo_server,fifo_client;
int itreleased;
int convertToLower(char * stringy){
	int i;
	for (i = 0; stringy[i]; i++){
		stringy[i] = tolower(stringy[i]);
	}
	return 0;
}

int checkIfInt(char *str) {
	int i;
	for(i = 0; str[i];i++){
		if(isdigit(str[i]) == 0){
			return 7;
		}
	}
	return 0;
}

int convertToDecimal(int number){
	int thebinary = number;
	int thedecimal =0;
	int start = 1;
	int remainder;
	while(thebinary >0){
		remainder = thebinary % 10;
		thedecimal = thedecimal + remainder * start;
		thebinary = thebinary/10;
		start = start * 2;

	}
	return thedecimal;
}
void sigusr1(int num){
	signal(SIGUSR1,sigusr1);
	pthread_exit(0);
}

void *threads(void* arg){
	signal(SIGUSR1,sigusr1);
	while(1){}
	pthread_exit(0);

}

int returndecimal(char * string);

int main(){
	int sessionstart = 0;
	char input1[100];
	
	pthread_t tids;
loopy:
	printf ("%s\n","Enter a command:");
	fgets(input1,30,stdin);
	input1[strcspn(input1, "\n")] = 0;
	char *firstWord;
	firstWord = strtok(input1," ");
	convertToLower(firstWord);
	if(strcmp(firstWord,"start")==0){
		itreleased = 0;
		if(sessionstart == 0){
			arrayfirstpage = malloc(16*sizeof(firststuff));
			int x;
			for(x=0;x<16;x++){
				arrayfirstpage[x].secondpage.first = 99999999;
				arrayfirstpage[x].secondpage.second =99999999;
				arrayfirstpage[x].secondpage.third =99999999;
				arrayfirstpage[x].secondpage.fourth =99999999;
			}
			cachearray = malloc(4*sizeof(cachearray));
			int tt;
			for(tt=0;tt<4;tt++){
				strcpy(cachearray[tt].entryId,"null");
				strcpy(cachearray[tt].artname,"null");
			}

			pthread_create(&tids,NULL,threads,NULL);

			sessionstart = 1;
			fifo_server=open("fifo_server",O_RDWR);
			if(fifo_server < 0) {
				printf("Error in opening file");
				exit(0);
			}
			fifo_client=open("fifo_client",O_RDWR);
			if(fifo_client < 0) {
				printf("Error in opening file");
				exit(0);
			}

			long int threadid = tids;
			char result[1000];
			sprintf(result,"%ld",threadid);
			char hi[1000] = "threadid ";
			strcat(hi,result);
			write(fifo_server,hi,1000);
			read(fifo_client,hi,sizeof(hi));
			printf("Message recieved from database: Connected to client %s\n",hi);

		}else if(sessionstart !=0){
			printf("%s\n","There is already another session. Can't start another");
		}
		goto loopy;
	}
	if(strcmp(firstWord,"alloc")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		int thefirstindex;
		int thesecondindex;
		int x;
		for(x=0;x<16;x++){
			if(arrayfirstpage[x].secondpage.first == 99999999){
				thefirstindex = x;
				thesecondindex = 1;
				goto foundempty;
			}
			if(arrayfirstpage[x].secondpage.second ==99999999){
				thefirstindex = x;
				thesecondindex = 2;
				goto foundempty;
			}
			if(arrayfirstpage[x].secondpage.third ==99999999){
				thefirstindex = x;
				thesecondindex = 3;
				goto foundempty;
			}
			if(arrayfirstpage[x].secondpage.fourth == 99999999){
				thefirstindex = x;
				thesecondindex = 4;
				goto foundempty;
			}
		}
		printf("%s\n","No more space for new art entry");
		goto loopy;
		char str[1000];
foundempty:
		sprintf(str,"%ld",tids);
		char another[1000] ="findaindex ";
		strcat(another,str);
		write(fifo_server, another, sizeof(str));

		read(fifo_client,str,sizeof(str));
			if(strcmp(str,"Remote database couldn't find a spot for reserve")==0){
				printf("Message recieved from database: There is no more spot for reserve\n");
				goto loopy;
			}else{
				printf("Message recieved from database: art entry stored at index %s in the remote database\n",str);
			}
		int databaseindex;
		sscanf(str,"%d",&databaseindex);
		int h;
		for(h=0;h<16;h++){
			if(h == thefirstindex){
				if(thesecondindex ==1){
					arrayfirstpage[h].secondpage.first = databaseindex;
				}
				if(thesecondindex ==2){
					arrayfirstpage[h].secondpage.second = databaseindex;
				}
				if(thesecondindex ==3){
					arrayfirstpage[h].secondpage.third = databaseindex;
				}
				if(thesecondindex ==4){
					arrayfirstpage[h].secondpage.fourth = databaseindex;
				}
			}
		}


		goto loopy;
	}
	if(strcmp(firstWord,"dealloc")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		char *theNumber = strtok(NULL," ");
		int num = checkIfInt(theNumber);
		if(num == 7 || strlen(theNumber) != 8){
			printf("Please a number that is 8 digits, e.g. 00000000.\n");
			goto loopy;
		}

		char unusedindex[3];
		char firstindex[5];
		char secondindex[3];
		strncpy(unusedindex,theNumber,2);
		unusedindex[2]='\0';
		if(strcmp(unusedindex,"00")!=0){
			printf("Please enter 8 digit number that start with 00, e.g 00XXXXXX\n");
			goto loopy;
		}
		strncpy(firstindex,theNumber+2,4);
		firstindex[4]='\0';
		strncpy(secondindex,theNumber+6,2);
		secondindex[2]='\0';

		int firstpageindex = returndecimal(firstindex);
		int secondpageindex = returndecimal(secondindex);
		if(firstpageindex == 99 || secondpageindex == 99){
			printf("Please enter a valid 8 digit number between 00000000 - 00111111\n");
			goto loopy;
		}
		int sendindex;
		int loko;
		for(loko =0;loko<16;loko++){
			if(loko == firstpageindex){

				if(secondpageindex ==0 && arrayfirstpage[loko].secondpage.first != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.first;
					arrayfirstpage[loko].secondpage.first = 99999999;
					goto ddit;
				}
				if(secondpageindex ==1 && arrayfirstpage[loko].secondpage.second != 99999999){

					sendindex = arrayfirstpage[loko].secondpage.second;
					arrayfirstpage[loko].secondpage.second = 99999999;
					goto ddit;
				}
				if(secondpageindex ==2 && arrayfirstpage[loko].secondpage.third != 99999999){

					sendindex = arrayfirstpage[loko].secondpage.third;
					arrayfirstpage[loko].secondpage.third = 99999999;
					goto ddit;
				}
				if(secondpageindex ==3 && arrayfirstpage[loko].secondpage.fourth != 99999999){

					sendindex = arrayfirstpage[loko].secondpage.fourth;
					arrayfirstpage[loko].secondpage.fourth = 99999999;
					goto ddit;
				}
				printf("There is no such entry to be removed\n");
				goto loopy;
			}
		}
		char str[1000];
ddit:

		sprintf(str,"%d",sendindex);
		char another[1000] ="deleteaindex ";
		strcat(another,str);
		write(fifo_server, another, sizeof(another));

		read(fifo_client,str,sizeof(str));
		printf("Message recieved from database: %s\n",str);



		goto loopy;
	}
	if(strcmp(firstWord,"read")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		char *theNumber = strtok(NULL," ");
		int num = checkIfInt(theNumber);
		if(num == 7 || strlen(theNumber) != 8){
			printf("Please a number that is 8 digits, e.g. 00000000.\n");
			goto loopy;
		}
		
	

		char unusedindex[3];
		char firstindex[5];
		char secondindex[3];
		strncpy(unusedindex,theNumber,2);
		unusedindex[2]='\0';
		if(strcmp(unusedindex,"00")!=0){
			printf("Please enter 8 digit number that start with 00, e.g 00XXXXXX\n");
			goto loopy;
		}
		strncpy(firstindex,theNumber+2,4);
		firstindex[4]='\0';
		strncpy(secondindex,theNumber+6,2);
		secondindex[2]='\0';

		int firstpageindex = returndecimal(firstindex);
		int secondpageindex = returndecimal(secondindex);
		if(firstpageindex == 99 || secondpageindex == 99){
			printf("Please enter a valid 8 digit number between 00000000 - 00111111\n");
			goto loopy;
		}

		int gg;
		for(gg=0;gg<4;gg++){
			if(strcmp(cachearray[gg].entryId,theNumber)==0){
				printf("cache hit\n");
				printf("The art name is %s.\n",cachearray[gg].artname);
				goto loopy;
			}
		}



		int sendindex;
		int loko;
		for(loko =0;loko<16;loko++){
			if(loko == firstpageindex){
				if(secondpageindex ==0 && arrayfirstpage[loko].secondpage.first != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.first;
					goto dddit;
				}
				if(secondpageindex ==1 && arrayfirstpage[loko].secondpage.second != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.second;
					goto dddit;
				}
				if(secondpageindex ==2 && arrayfirstpage[loko].secondpage.third != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.third;
					goto dddit;
				}
				if(secondpageindex ==3 && arrayfirstpage[loko].secondpage.fourth != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.fourth;
					goto dddit;
				}
				printf("There is no such art name for this entry id to be read.\n");
				goto loopy;
			}
		}
		char str[1000];
dddit:

		sprintf(str,"%d",sendindex);
		char another[1000] ="findart ";
		strcat(another,str);
		write(fifo_server, another, sizeof(another));

		read(fifo_client,str,sizeof(str));
		if(strcmp(str,"There was no name.")==0){
			printf("There was not artname stored in remote database for such entryId\n");
		}else{
			int zz;
			for(zz=0;zz<4;zz++){
				if(strcmp(cachearray[zz].entryId,"null")==0){
					printf("cache miss\n");
					strcpy(cachearray[zz].entryId,theNumber);
					strcpy(cachearray[zz].artname,str);
					printf("The art name is %s.\n",cachearray[zz].artname);
					goto loopy;
				}
			}
			printf("eviction\n");
			int evic;
			for(evic =0;evic<4;evic++){
				if(evic ==3){
					strcpy(cachearray[evic].entryId,theNumber);
					strcpy(cachearray[evic].artname,str);
					printf("The art name is %s.\n",str);
				}
			}

		}

		goto loopy;
	}
	if(strcmp(firstWord,"store")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		char *theNumber = strtok(NULL," ");
		char *theArtName = strtok(NULL,"");
		if(theArtName == NULL){
			printf("Please enter a art name to be stored\n");
			goto loopy;
		}
		int num = checkIfInt(theNumber);
		if(num == 7 || strlen(theNumber) != 8){
			printf("Please a number that is 8 digits, e.g. 00000000.\n");
			goto loopy;
		}

		char unusedindex[3];
		char firstindex[5];
		char secondindex[3];
		strncpy(unusedindex,theNumber,2);
		unusedindex[2]='\0';
		if(strcmp(unusedindex,"00")!=0){
			printf("Please enter 8 digit number that start with 00, e.g 00XXXXXX\n");
			goto loopy;
		}
		strncpy(firstindex,theNumber+2,4);
		firstindex[4]='\0';
		strncpy(secondindex,theNumber+6,2);
		secondindex[2]='\0';

		int firstpageindex = returndecimal(firstindex);
		int secondpageindex = returndecimal(secondindex);
		if(firstpageindex == 99 || secondpageindex == 99){
			printf("Please enter a valid 8 digit number between 00000000 - 00111111\n");
			goto loopy;
		}


		int gg;
		for(gg=0;gg<4;gg++){
			if(strcmp(cachearray[gg].entryId,theNumber)==0){
				strcpy(cachearray[gg].artname,theArtName);
				printf("The art name has been stored in cache since there was instance of it.\n");
				goto loopy;
			}
		}



		int sendindex;
		int loko;
		for(loko =0;loko<16;loko++){
			if(loko == firstpageindex){
				if(secondpageindex ==0 && arrayfirstpage[loko].secondpage.first != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.first;
					goto ddddit;
				}
				if(secondpageindex ==1 && arrayfirstpage[loko].secondpage.second != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.second;
					goto ddddit;
				}
				if(secondpageindex ==2 && arrayfirstpage[loko].secondpage.third != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.third;
					goto ddddit;
				}
				if(secondpageindex ==3 && arrayfirstpage[loko].secondpage.fourth != 99999999){
					sendindex = arrayfirstpage[loko].secondpage.fourth;
					goto ddddit;
				}
				printf("There is no such entry with such entry id\n");
				goto loopy;
			}
		}
		char str[1000];
ddddit:

		sprintf(str,"%d",sendindex);
		char another[1000] ="insertart ";
		strcat(another,str);
		char anotheri[100]=" ";
		strcat(another,anotheri);
		strcat(another,theArtName);
		write(fifo_server, another, sizeof(another));
		read(fifo_client,str,sizeof(str));
		printf("Message recieved from database: %s\n",str);

		goto loopy;
	}
	if(strcmp(firstWord,"close")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		itreleased  = 1;
		sessionstart = 0;
		int x;
		for(x=0;x<16;x++){
			if(arrayfirstpage[x].secondpage.first != 99999999){
				arrayfirstpage[x].secondpage.first = 99999999;
			}
			if(arrayfirstpage[x].secondpage.second != 99999999){
				arrayfirstpage[x].secondpage.second = 99999999;
			}
			if(arrayfirstpage[x].secondpage.third != 99999999){
				arrayfirstpage[x].secondpage.third = 99999999;
			}
			if(arrayfirstpage[x].secondpage.fourth != 99999999){
				arrayfirstpage[x].secondpage.fourth = 99999999;
			}
		}
		char str[1000];
		sprintf(str,"%ld",tids);
		char another[1000]="clearit ";
		strcat(another,str);
		write(fifo_server,another,sizeof(another));
		read(fifo_client,another,sizeof(another));
		printf("Message recieved from database: %s\n",another);
		pthread_kill(tids,SIGUSR1);
		pthread_join(tids,NULL);
		free(arrayfirstpage);
		free(cachearray);
		printf("%s\n","Session has closed");
		goto loopy;
	}
	if(strcmp(firstWord,"infotab")==0){
		if(sessionstart == 0){
			printf("Please start a session first.\n");
			goto loopy;
		}
		int x;
		printf("Index\tEmpty Status\n\n");
		for(x=0;x<16;x++){
			int emptyornot = 0;
			if(arrayfirstpage[x].secondpage.first != 99999999 || arrayfirstpage[x].secondpage.second != 99999999 || arrayfirstpage[x].secondpage.third != 99999999 || arrayfirstpage[x].secondpage.fourth != 99999999){
				emptyornot = 1;
			}
			if(emptyornot == 1){
				printf("%d\t%s\n",x,"Not empty");
			}else{
				printf("%d\t%s\n",x,"Empty");
			}
		}
		char input2[100];
		printf("%s\n","Which page would you like to navigate to(Enter \"back\" to go back)?");
		fgets(input2,30,stdin);
		input2[strcspn(input2, "\n")] = 0;
		if(strcmp(input2,"back")==0){
			goto loopy;
		}else{
			int thepagenumber;
			sscanf(input2,"%d",&thepagenumber);
			int h;
			for(h=0;h<16;h++){
				if(h==thepagenumber){
					printf("Index\tRemote Id in Datebase(99999999 means it empty by default)\n");
					printf("%d\t%d\n",0,arrayfirstpage[h].secondpage.first);
					printf("%d\t%d\n",1,arrayfirstpage[h].secondpage.second);
					printf("%d\t%d\n",2,arrayfirstpage[h].secondpage.third);
					printf("%d\t%d\n",3,arrayfirstpage[h].secondpage.fourth);
				}
			}

		}
		goto loopy;
	}
	if(strcmp(firstWord,"exit")==0){
		if(itreleased == 0){
			printf("Please close current session first before exit\n");
			goto loopy;
		}
		exit(0);
	}
	printf("%s\n","Command not found");
	goto loopy;
}


int returndecimal(char * string){

	if(strcmp(string,"00")==0 || strcmp(string,"0000")==0){
		return 0;
	}
	if(strcmp(string,"01")==0 || strcmp(string,"0001")==0){
		return 1 ;
	}
	if(strcmp(string,"10")==0 || strcmp(string,"0010")==0){
		return 2;
	}
	if(strcmp(string,"11")==0 || strcmp(string,"0011")==0){
		return 3;
	}
	if(strcmp(string,"0100")==0){
		return 4 ;
	}
	if(strcmp(string,"0101")==0){
		return 5;
	}
	if(strcmp(string,"0110")==0){
		return 6;
	}
	if(strcmp(string,"0111")==0){
		return 7;
	}
	if(strcmp(string,"1000")==0){
		return 8;
	}
	if(strcmp(string,"1001")==0){
		return 9;
	}
	if(strcmp(string,"1010")==0){
		return 10;
	}
	if(strcmp(string,"1011")==0){
		return 11;
	}
	if(strcmp(string,"1100")==0){
		return 12;
	}
	if(strcmp(string,"1101")==0){
		return 13;
	}
	if(strcmp(string,"1110")==0){
		return 14;
	}
	if(strcmp(string,"1111")==0){
		return 15;
	}
	return 99;
}
