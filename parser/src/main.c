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

    char filename[300] = {'\0'};
   // strcpy(filename, "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/uploads/simple.gpx");
    strcpy(filename, "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/uploads/GHTC_James-Stone.gpx");
    // double wpt1Lat = 70;
    // double wpt1Lon = 70;
    // double wpt2Lat = 80;
    // double wpt2Lon = 80;

    // if (!addRouteToGPXWrapper(filename, wpt1Lat, wpt1Lon, wpt2Lat, wpt2Lon)) printf("WRONG WRONG WRONG\n");
    printf(getAllRouteComponentsJSON(filename));
    printf("\n");

    // free(GPXstring);
    deleteGPXdoc(doc);
    doc = NULL;

    return 0;
}