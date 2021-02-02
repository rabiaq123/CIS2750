#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main() {
    int isParsed = 0; //if error in parsing XML, set to 1
    char filename[] = "/home/undergrad/0/rqureshi/Desktop/W21/CIS2750/ListExample/Reyn-Rozh_parking.gpx";

    //attempt to parse XML file
    isParsed = parseXMLfile(filename);

    if (isParsed == 1) { //error-checking
        printf("Error: could not parse file %s\n", filename);
    }

    return 0;
}