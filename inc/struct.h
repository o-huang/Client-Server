#ifndef STRUCT_H
#define STRUCT_H

#include <stdint.h>
typedef struct secondpage{
	int first;
	int second;
	int third;
	int fourth;
}secondstuff;

typedef struct firstpage{
	struct secondpage secondpage;

}firststuff;

typedef struct cachepage{
	char entryId[500];
	char artname[500];
}quickstuff;

typedef struct warehousestuff {
	char artname[500];
	int recordid;
	char clientid[500];
	int useornot;

}warehouseitem;


#endif /* WAREHOUSE_H */
