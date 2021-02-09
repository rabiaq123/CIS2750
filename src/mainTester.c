#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main() {
    GPXdoc* createdGPXobj = NULL;
    char* GPXstring;
    char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/ListExample/simple.gpx";

    //attempt to parse XML file
    createdGPXobj = createGPXdoc(file);

    if (createdGPXobj == NULL) { //error-checking
        printf("Error: could not parse file %s\n", file);
    }

    GPXstring = GPXdocToString(createdGPXobj);
    printf(GPXstring);
    
    free(GPXstring);
    deleteGPXdoc(createdGPXobj);
    createdGPXobj = NULL;

    return 0;
}