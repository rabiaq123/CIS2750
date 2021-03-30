#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main() {
    GPXdoc* doc = NULL;
    char* GPXstring;
    char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/uploads/Memorial_Forest_Loop(1rt_0trk_0seg_430m).gpx";
    char schema[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/cis2750w21_project/parser/gpx.xsd";

    //attempt to parse XML file
    doc = createValidGPXdoc(file, schema);

    if (doc == NULL) { //error-checking
        printf("Error: could not parse file %s\n", file);
        return 0;
    }

    if (!validateGPXDoc(doc, schema)) {
        printf("invalid GPX\n");
    }
    writeGPXdoc(doc, "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/SampleGPXfiles/doc.gpx");

    //int len = numRoutesWithLength(doc, 4220.00, 10.00);
    //printf("1.\nin main NUMROUTESWITHLENGTH: %d\n", numRoutesWithLength(doc, 1960.00, 10.00));
    //printf("2.\nin main NUMROUTESWITHLENGTH: %d\n", numRoutesWithLength(doc, 4220.00, 10.00));
    //printf("3.\nin main NUMROUTESWITHLENGTH: %d\n", numRoutesWithLength(doc, 200.00, 10.00));

    GPXstring = GPXdocToString(doc);
    /*
    printf(GPXstring);
    printf("file is: %s\n", file);
    printf("getNumWaypoints: %d, \ngetNumRoutes: %d, \ngetNumTracks: %d, \ngetNumSegments: %d, \ngetNumGPXData: %d\n",
            getNumWaypoints(doc), getNumRoutes(doc), getNumTracks(doc), getNumSegments(doc), getNumGPXData(doc));
    */

    //find Route children nodes of root node
    Route *rtePtr;
    ListIterator iter = createIterator(doc->routes);
    while ((rtePtr = nextElement(&iter)) != NULL) {
        if (routeToJSON(rtePtr) != NULL) {

        printf(routeToJSON(rtePtr));
        }
    }
    printf("\n");

    free(GPXstring);
    deleteGPXdoc(doc);
    doc = NULL;

    return 0;
}