#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main() {
    //printf("in main()\n");
    GPXdoc* createdGPXobj = NULL;
    char filename[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/ListExample/Reyn-Rozh_parking.gpx";


    //attempt to parse XML file
    createdGPXobj = createGPXdoc(filename);

    if (createdGPXobj == NULL) { //error-checking
        printf("Error: could not parse file %s\n", filename);
    }

    free(createdGPXobj);

    return 0;
}