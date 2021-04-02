#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main() {
    GPXdoc* doc = NULL;
    // char* GPXstring;
    // char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/uploads/simple.gpx";
    // char schema[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/parser/gpx.xsd";

    char creator[6] = {'\0'};
    strcpy(creator, "rabia");
    char filename[30] = {'\0'};
    strcpy(filename, "/uploads/thingsss.gpx");
    int creatorLen = strlen(creator);

    if (!createNewGPX(filename, creator, creatorLen)) printf("WRONG WRONG WRONG\n");

    printf("\n");

    // free(GPXstring);
    deleteGPXdoc(doc);
    doc = NULL;

    return 0;
}