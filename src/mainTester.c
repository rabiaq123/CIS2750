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
    char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/ListExample/Reyn-Rozh_parking.gpx";
    // char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/ListExample/simple.gpx";
    // char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/SampleGPXfiles/GHTC_James-Victoria(1rt_0trk_0seg_1960m).gpx";

    //attempt to parse XML file
    createdGPXobj = createGPXdoc(file);

    if (createdGPXobj == NULL) { //error-checking
        printf("Error: could not parse file %s\n", file);
    }

    deleteGPXdoc(createdGPXobj);
    createdGPXobj = NULL;

    return 0;
}