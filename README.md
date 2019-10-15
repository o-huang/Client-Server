This is a client-server system where we will be demonstrating how we can establish a connection between a client and a server using fifo. Additioanlly, in the program we will be demonstrating how virtual memory and cache work in the program. So there are two parts to the program. There is the client where we will have a two-level page table and there is the remote database(the server) where we will be saving our data(in this case we will be saving art pieces which simply be a name). 

<<<<<<< HEAD
How to run the program:  
	First make the server with: make theserver 
and then run ./whouse (and a certain integer for size of your data structure size in the remote database)    
	Then you can run the client with: make theclient  
	and then run ./wclient    

	What each commands in the client does:  
	Start:  
	it will start a session. It will create/initilize to two-page table. It will also create a thread and keep it running. It will send the threadid to the server in which the server will save the threadid to know which client has connected to it.  

	IMPORTANT DETAILS OF THE TWO PAGE TABLE:  
	My implementation of the two page table is a array of structs. And within each struct, it will point to another struct which will contain 4 ints which can be used to store the translation of the IDs. NOTE,EACH INT HAS BEEN INITLIZED TO 99999999 AS DEFAULT VALUE. THE TWO LEVEL PAGE TABLES CAN ONLY HOLD A TOTAL OF 64 ITEMS, SINCE THE FIRST LEVEL ONLY HAS 16 INDICES AND 4 FOR EACH SECOND LEVEL. 

	Alloc:  
	This command goes into the remote database and tells it to find a empty index in it database for which the client can save its art name in. The server will then return the empty index back to the client in which will be saved in the two page table.  

	Dealloc ID:  
	Dealloc ID accepts a ID number which will be number of 8 bits(exp 00000000). The first 2 bits mean nothing. The second 4 bits means the index of the first level page table and the last 2 bits means the index of the second level page table. For example, if I wrote 00000000 it mean 0 index of the first level page table since 0000 is 0 in decimal. Additionally, we get 0 for the second level page table since 00 also mean 0 in decimal. So if we do dealloc 00000000 we are trying to remove save index at that location if there is one and go to the remote database and clear the index of whatever was saved in the two level page table.	

	Read ID:
	This command will read the retrieve and read the art name from the remote database with the provided ID. If there is none then a error message will appear.  
=======
How to run the program:    
	First make the server with: make theserver 
		and then run ./whouse (and a certain integer for size of your data structure size in the remote database)      
	Then you can run the client with: make theclient  
		and then run ./wclient      

What each commands in the client does:

	Start:  
		it will start a session. It will create/initilize to two-page table. It will also create a thread and keep it running. It will send the threadid to the server in which the server will save the threadid to know which client has connected to it.  

	IMPORTANT DETAILS OF THE TWO PAGE TABLE:  
		My implementation of the two page table is a array of structs. And within each struct, it will point to another struct which will contain 4 ints which can be used to store the translation of the IDs. NOTE,EACH INT HAS BEEN INITLIZED TO 99999999 AS DEFAULT VALUE. THE TWO LEVEL PAGE TABLES CAN ONLY HOLD A TOTAL OF 64 ITEMS, SINCE THE FIRST LEVEL ONLY HAS 16 INDICES AND 4 FOR EACH SECOND LEVEL. 

	Alloc:  
		This command goes into the remote database and tells it to find a empty index in it database for which the client can save its art name in. The server will then return the empty index back to the client in which will be saved in the two page table.  
	
	Dealloc ID:  
		Dealloc ID accepts a ID number which will be number of 8 bits(exp 00000000). The first 2 bits mean nothing. The second 4 bits means the index of the first level page table and the last 2 bits means the index of the second level page table. For example, if I wrote 00000000 it mean 0 index of the first level page table since 0000 is 0 in decimal. Additionally, we get 0 for the second level page table since 00 also mean 0 in decimal. So if we do dealloc 00000000 we are trying to remove save index at that location if there is one and go to the remote database and clear the index of whatever was saved in the two level page table.	

	Read ID:
		This command will read the retrieve and read the art name from the remote database with the provided ID. If there is none then a error message will appear.  
>>>>>>> 3b7d840f80e3935416b2e0f1f6f6512d317bf2cf

	Store ID:  
	This command set art name Z to the specified art entry. If there is already one then it will just replace it.  

	Close: 
<<<<<<< HEAD
	Close clear all art entries in the two-page table and in the remote server. SO anything that has to do with the current session with be gone. The thread that has been running will also be stopped. Everything will be set to default again.
=======
		Close clear all art entries in the two-page table and in the remote server. SO anything that has to do with the current session with be gone. The thread that has been running will also be stopped. Everything will be set to default again.
>>>>>>> 3b7d840f80e3935416b2e0f1f6f6512d317bf2cf

	Exit:
	Exit the program and free anything that need to be freed.


	INFOTAB(IMPORTANT):
		So for my infotab the first page will print number from 0 -15 to indicate the 16 indexes that are there. Additionally it will print empty or not empty, meaning if the second page will contain anything or not. By default if every int in the second page table is 99999999 it will be empty, but if any of them are not then a not empty will be printed. The second page table is similart in that it will print 0-3 the indexes and then the value of Record ID if there is one. By default it will be 99999999. 




<<<<<<< HEAD
		What each command in warehouse db.c does:
=======
	So the server will keep running until SIGINT SINGAL is catched. When catch a shell will occur in which the user has 4 commands that can occur.  
	List:  
		List will just print a number 1-4 which is just a number showing how many clients there are. Nothing special about that number. And then it will also prints its ID next to it which will be the thread id that the clients has sended over. CAN ONLY HAVE A TOTAL OF 4 CLIENTS!! 
>>>>>>> 3b7d840f80e3935416b2e0f1f6f6512d317bf2cf

		So the server will keep running until SIGINT SINGAL is catched. When catch a shell will occur in which the user has 4 commands that can occur.  
		List:  
		List will just print a number 1-4 which is just a number showing how many clients there are. Nothing special about that number. And then it will also prints its ID next to it which will be the thread id that the clients has sended over. CAN ONLY HAVE A TOTAL OF 4 CLIENTS!! 

		List X:  
		List X will print art entries used by X(X here will have to be the thread ID which you have to enter). It will print the artname, recordid, clientid, and validbit(a int to signify if used or not).  

		Dump:  
		Dump will basically print all entries it has saved. THe format will be the same as in LIST X.  

		Exit: Exit will stop the server and free anything needed.  


