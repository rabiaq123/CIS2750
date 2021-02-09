/**
 * Name: Rabia Qureshi
 * StudentID: 1046427
 * Email: rqureshi@uoguelph.ca
 */

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

/**
 * Note that as allowed by Professor Nikitenko,
 * certain functions, specifically parseXMLfiles() and printElementNames(),
 * contain sample code from the libXmlExample.c file provided in class,
 * which is based on the LibXML example from:
 * http://www.xmlsoft.org/examples/tree1.c
 */


GPXdoc* createGPXdoc(char* fileName) {
    /*
     * initializes the library and checks potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    //error-checking for invalid filename
    if (fileName == NULL || strcmp(fileName, "") == 0) {
        return NULL;
    }

    xmlDoc* file = NULL;
    xmlNode* rootElement = NULL;

    //attempt to parse XML file
    file = xmlReadFile(fileName, NULL, 0);
    if (file == NULL) { //return if not a well-formed XML
        return NULL;
    }
    rootElement = xmlDocGetRootElement(file); //get the root element node 'gpx'

    GPXdoc* myGPXdoc = (GPXdoc*)calloc(1, sizeof(GPXdoc)); //calloc() initializes memory allocated to 0
    initializeReqLists(myGPXdoc);

    if (!traverseGPXtree(rootElement, myGPXdoc)) { //incorrectly-formatted GPX file
        xmlFreeDoc(file);
        deleteGPXdoc(myGPXdoc);
        return NULL;
    }

    xmlFreeDoc(file);

    return myGPXdoc;
}


void initializeReqLists(GPXdoc* myGPXdoc) {
    myGPXdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
}


bool traverseGPXtree(xmlNode* node, GPXdoc* myGPXdoc) {
    xmlNode* curNode = node;

    //load XML tree into GPX struct
    for (curNode = node; curNode != NULL; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) {

            if (strcmp((char*)curNode->name, "gpx") == 0) {
                if (!storeGpxAttributes(curNode, myGPXdoc)) { //store root node's attributes
                    return false;
                }
            } else if (strcmp((char*)curNode->name, "wpt") == 0) {
                if (!storeWpt(curNode, myGPXdoc)) { //store one wpt
                    return false;
                }
            } /*else if (strcmp((char*)curNode->name, "trk") == 0) {

            } else if (strcmp((char*)curNode->name, "rte") == 0) {

            } else if (strcmp((char*)curNode->name, "trkseg") == 0) {

            } else if (strcmp((char*)curNode->name, "rtept") == 0) {

            }*/
        }

        //traverse tree to get the node's children
        if (!traverseGPXtree(curNode->children, myGPXdoc)) {
            return false;
        }
    }

    return true;
}


bool storeWpt(xmlNode* curNode, GPXdoc* myGPXdoc) {
    Waypoint* newWpt;
    xmlAttr* attr;
    xmlNode* wptChild;

    newWpt = (Waypoint*)calloc(1, sizeof(Waypoint));
    newWpt->name = (char*)calloc(300, sizeof(char)); //'name' in Waypoint struct must always be initialized

    //store Waypoint attributes in Waypoint struct
    for (attr = curNode->properties; attr != NULL; attr = attr->next) {
        xmlNode* value = attr->children; //value associated with attribute

        if (strcmp((char*)attr->name, "lon") == 0) { //store the longitude
            if (!storeWptLongitude(value, newWpt)) {
                return false;
            }
        } else if (strcmp((char*)attr->name, "lat") == 0) { //store the latitude
            if (!storeWptLatitude(value, newWpt)) {
                return false;
            }
        }
    }

    //store content
    for (wptChild = curNode->children; wptChild != NULL; wptChild = wptChild->next) {
        if (wptChild->type == XML_ELEMENT_NODE) {

            if (strcmp((char*)wptChild->name, "name") == 0) {
                storeWptName(wptChild, newWpt);
            } 
        }
    }

    insertBack(myGPXdoc->waypoints, newWpt);

    return true;
}


void storeWptName(xmlNode* wptChild, Waypoint* newWpt) {
    char buffer[300] = {'\0'};
    int len;

    //error-checking for incorrectly formatted Waypoint name
    if(wptChild->children->content == NULL) {
        return;
    }

    strcpy(buffer, (char*)wptChild->children->content);
    len = strlen(buffer); //strlen() excludes NULL terminator

    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return;
    }

    //store in Waypoint struct
    strcpy(newWpt->name, buffer);
    newWpt->name = realloc(newWpt->name, len + 1);
}


bool storeWptLatitude(xmlNode* value, Waypoint* newWpt) {
    double lat;
    char *ptr;
    char buffer[100] = {'\0'};

    //error-checking for no latitude (core data of the wpt)
    if(value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char*)value->content);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return false;
    }

    //store in Waypoint struct
    lat = strtod(buffer, &ptr); //convert char* to type double
    newWpt->latitude = lat;

    return true;
}


bool storeWptLongitude(xmlNode* value, Waypoint* newWpt) {
    double lon;
    char *ptr;
    char buffer[100] = {'\0'};

    //error-checking for no latitude (core data of the wpt)
    if(value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char*)value->content);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return false;
    }

    //store in Waypoint struct
    lon = strtod(buffer, &ptr); //convert char* to type double
    newWpt->longitude = lon;

    return true;
}


void deleteWaypoint(void* data) {
    Waypoint* tempWpt;

    if (data == NULL) {
        return;
    }

    tempWpt = (Waypoint*)data;
    
    free(tempWpt->name);
    free(tempWpt);
}


int compareWaypoints(const void* first, const void* second) {
    Waypoint* tempWpt1;
	Waypoint* tempWpt2;
	
	if (first == NULL || second == NULL) { //error-checking
		return 0;
	}
	
	tempWpt1 = (Waypoint*)first;
	tempWpt2 = (Waypoint*)second;
	
	return strcmp((char*)tempWpt1->name, (char*)tempWpt2->name);
}


char* waypointToString(void* data) {
    char* result = (char*)calloc(1000, sizeof(char));
	Waypoint* tempWpt;
	int len;
	
	if (data == NULL) {
		return NULL;
	}
	
	tempWpt = (Waypoint*)data;
    sprintf(result, "\nWaypoint: \n*********************** \nLAT: %f \nLON: %f", tempWpt->latitude, tempWpt->longitude);
    strcat(result, "\nNAME: ");
    strcat(result, tempWpt->name);
		
	len = strlen(result); //strlen() excludes NULL terminator
	result = realloc(result, len + 1);
		
	return result;
}


bool storeGpxAttributes(xmlNode* curNode, GPXdoc* myGPXdoc) {
    xmlAttr* attr;

    //store the namespace
    if (!storeGpxNamespace(curNode, myGPXdoc)) { //empty or non-existent ns
        return false;
    }

    //NOTE that libxml2 documentation calls xmlAttr values “properties” instead of “attributes”
    for (attr = curNode->properties; attr != NULL; attr = attr->next) {
        xmlNode* value = attr->children; //value associated with attribute

        if (strcmp((char*)attr->name, "version") == 0) { //store the version
            if (!storeGpxVersion(value, myGPXdoc)) {
                return false;
            }
        } else if (strcmp((char*)attr->name, "creator") == 0) { //store the creator
            if (!storeGpxCreator(value, myGPXdoc)) {
                return false;
            }
        }
    }
    
    return true;
}


bool storeGpxCreator(xmlNode* value, GPXdoc* myGPXdoc) {
    char buffer[1000] = {'\0'};
    int len;

    //error-checking for incorrectly formatted creator value
    if(value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char*)value->content);
    len = strlen(buffer); //strlen() excludes NULL terminator

    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return false;
    }

    //store in GPX struct
    myGPXdoc->creator = (char *)calloc(len + 1, sizeof(char));
    strcpy(myGPXdoc->creator, buffer);

    return true;
}


bool storeGpxVersion(xmlNode* value, GPXdoc* myGPXdoc) {
    double versionNum;
    char* ptr;
    char buffer[100] = {'\0'};

    //error-checking for incorrectly formatted version number
    if (value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char*)value->content);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return false;
    }

    //store in GPX struct
    versionNum = strtod(buffer, &ptr); //convert char* to type double
    myGPXdoc->version = versionNum;

    return true;
}


bool storeGpxNamespace(xmlNode* rootNode, GPXdoc* myGPXdoc) {
    char buffer[256] = {'\0'};

    //error-checking for incorrectly formatted namespace
    if (rootNode->ns == NULL) {
        return false;
    }
    strcpy(buffer, (char*)rootNode->ns->href);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string
        return false;
    }

    //store in GPX struct
    strcpy(myGPXdoc->namespace, buffer);

    return true;
}


char* GPXdocToString(GPXdoc* doc) {
    char* result = (char*)calloc(10000, sizeof(char));
    char* buffer = NULL;
    int len;

    //gpx (root node) attributes
    sprintf(result, "GPX doc: \n*********************** \nNAMESPACE: %s \nVERSION: %f \nCREATOR: %s", 
            doc->namespace, doc->version, doc->creator);
    //waypoints
    buffer = toString(doc->waypoints);
    strcat(result, buffer);
    strcat(result, "\n");

    len = strlen(result); //strlen() excludes NULL terminator
    result = realloc(result, len + 1);
    
    free(buffer);

    return result;
}


void deleteGPXdoc(GPXdoc *doc) {

    if (doc != NULL) { //error-checking
        free(doc->creator);
        freeList(doc->waypoints);
        free(doc);
        doc = NULL;
    }

    xmlCleanupParser();
}
