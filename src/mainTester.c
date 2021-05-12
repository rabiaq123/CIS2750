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
    char file[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/SampleGPXfiles/simple.gpx";
    char schema[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/A2-files/gpx.xsd";

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

    GPXstring = GPXdocToString(doc);
    /*
    printf(GPXstring);
    printf("file is: %s\n", file);
    printf("getNumWaypoints: %d, \ngetNumRoutes: %d, \ngetNumTracks: %d, \ngetNumSegments: %d, \ngetNumGPXData: %d\n",
            getNumWaypoints(doc), getNumRoutes(doc), getNumTracks(doc), getNumSegments(doc), getNumGPXData(doc));
    */

    free(GPXstring);
    deleteGPXdoc(doc);
    doc = NULL;

    return 0;
}