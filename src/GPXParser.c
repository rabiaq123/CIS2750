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
 * certain functions, specifically traverseGPXtree() and storeTrk(),
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
    if (fileName == NULL || strcmp(fileName, "") == 0) return NULL;
    xmlDoc* file = NULL;
    xmlNode* rootElement = NULL;

    //attempt to parse XML file
    file = xmlReadFile(fileName, NULL, 0);
    if (file == NULL) return NULL; //return if not a well-formed XML
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
    myGPXdoc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    myGPXdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
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
                if (!storeWpt(curNode, myGPXdoc, NULL, NULL)) { //store wpt in waypoints list of GPXdoc struct
                    return false;
                }
            } else if (strcmp((char*)curNode->name, "rte") == 0) {
                if (!storeRte(curNode, myGPXdoc)) { //store rte in routes list of GPXdoc struct
                    return false;
                }
            } else if (strcmp((char*)curNode->name, "trk") == 0) {
                if (!storeTrk(curNode, myGPXdoc)) { //store trk in tracks list of GPXdoc struct
                    return false;
                }
            }
        }

        //traverse tree to get the node's children
        if (!traverseGPXtree(curNode->children, myGPXdoc)) {
            return false;
        }
    }

    return true;
}


bool storeTrk(xmlNode* curNode, GPXdoc* myGPXdoc) {
    Track* newTrk;

    newTrk = (Track*)calloc(1, sizeof(Track));

    //must not be NULL, may be empty
    newTrk->name = (char*)calloc(300, sizeof(char));
    newTrk->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    newTrk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    //store content: name, trkseg(s) i.e list(s) of waypoints, and other data
    for (xmlNode* xmlTrkChild = curNode->children; xmlTrkChild != NULL; xmlTrkChild = xmlTrkChild->next) {
        if (xmlTrkChild->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)xmlTrkChild->name, "name") == 0) { //do not store 'name' child content in otherData list
                storeTrkName(xmlTrkChild, newTrk);
            } else if (strcmp((char*)xmlTrkChild->name, "trkseg") == 0) {
                if (!storeTrkSeg(xmlTrkChild, newTrk)) {
                    deleteTrack(newTrk);
                    return false;
                }
            } else {
                if (!storeTrkOtherData(xmlTrkChild, newTrk)) { //other data's name and content must not be empty strings
                    deleteTrack(newTrk);
                    return false;
                }
            }
        }
    }

    insertBack(myGPXdoc->tracks, newTrk);

    return true;
}


void storeTrkName(xmlNode* xmlTrkChild, Track* newTrk) {
    char buffer[300] = {'\0'};
    bool isEmpty = false;
    int len = 0;

    if (xmlTrkChild->children == NULL) { //name in Track struct must not be NULL
        isEmpty = true;
    } else {
        if ((xmlChar)xmlTrkChild->children->content[0] == '\0' || (xmlChar)xmlTrkChild->children->content[0] == '\n') {
            isEmpty = true;
        }
    }

    //store 'name' content in Track struct
    if (!isEmpty) {
        strcpy(buffer, (char*)xmlTrkChild->children->content);
        len = strlen(buffer); //strlen() excludes NULL terminator
        strcpy(newTrk->name, buffer);
    } else {
        strcpy(newTrk->name, "");
    }

    newTrk->name = (char*)realloc(newTrk->name, len + 1);

    return;
}


bool storeTrkSeg(xmlNode* xmlTrkChild, Track* newTrk) {
    TrackSegment* newTrkSeg = (TrackSegment*)calloc(1, sizeof(TrackSegment));
    
    //must not be NULL, may be empty
    newTrkSeg->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

    if (xmlTrkChild->children != NULL) { //if children exist for 'trkseg' element
        for (xmlNode* xmlTrkSegChild = xmlTrkChild->children->next; xmlTrkSegChild != NULL; xmlTrkSegChild = xmlTrkSegChild->next) {
            if (strcmp((char*)(xmlTrkSegChild->name), "trkpt") == 0)  {
                if (!storeWpt(xmlTrkSegChild, NULL, NULL, newTrkSeg)) { //store 'trkpt' in 'waypoints' list of TrackSegment struct
                    deleteTrackSegment(newTrkSeg);
                    return false;
                }
            }
        }
    }

    insertBack(newTrk->segments, newTrkSeg);

    return true;
}


bool storeTrkOtherData(xmlNode* xmlTrkChild, Track* newTrk) {
    char nameBuffer[256] = {'\0'}, contentBuffer[300] = {'\0'};
    GPXData* otherTrackData; //objects in otherData list are of type GPXData
    int len = 0;

    //if other children exist in 'trk', their name must not be an empty string (GPXData struct specifications)
    if ((xmlChar)xmlTrkChild->name[0] == '\0' || (xmlChar)xmlTrkChild->name[0] == '\n') {
        return false;
    }

    //if other children exist in 'trk', GPXData's content must not be an empty string
    if (xmlTrkChild->children == NULL) {
        return false;
    } else {
        if ((xmlChar)xmlTrkChild->children->content[0] == '\0' || (xmlChar)xmlTrkChild->children->content[0] == '\n') {
            return false;
        }
    }

    //parse other data's name and content
    strcpy(nameBuffer, (char*)xmlTrkChild->name);
    strcpy(contentBuffer, (char *)xmlTrkChild->children->content);
    len = strlen(contentBuffer); //strlen() excludes NULL terminator

    //store parsed data into GPXData struct
    otherTrackData = (GPXData*)malloc(sizeof(GPXData) + (len + 1) * sizeof(char)); //malloc for len+1 chars in value[]
    strcpy(otherTrackData->name, nameBuffer);
    strcpy(otherTrackData->value, contentBuffer);

    insertBack(newTrk->otherData, otherTrackData);

    return true;
}


bool storeRte(xmlNode* curNode, GPXdoc* myGPXdoc) {
    Route* newRte;

    newRte = (Route*)calloc(1, sizeof(Route));

    //must not be NULL, may be empty
    newRte->name = (char*)calloc(300, sizeof(char));
    newRte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    newRte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    //store content: name, rtepts (waypoints), and other data
    for (xmlNode* xmlRteChild = curNode->children; xmlRteChild != NULL; xmlRteChild = xmlRteChild->next) {
        if (xmlRteChild->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)xmlRteChild->name, "name") == 0) { //do not store 'name' child content in otherData list
                storeRteName(xmlRteChild, newRte);
            } else if (strcmp((char*)xmlRteChild->name, "rtept") == 0) {
                if (!storeWpt(xmlRteChild, NULL, newRte, NULL)) { //store 'rtept' in waypoints list of Route struct
                    deleteRoute(newRte);
                    return false;
                }
            } else {
                if (!storeRteOtherData(xmlRteChild, newRte)) { //other data's name and content must not be empty strings
                    deleteRoute(newRte);
                    return false;
                }
            }
        }
    }

    insertBack(myGPXdoc->routes, newRte);

    return true;
}


bool storeRteOtherData(xmlNode* xmlRteChild, Route* newRte) {
    char nameBuffer[256] = {'\0'}, contentBuffer[300] = {'\0'};
    GPXData* otherRteData; //objects in otherData list are of type GPXData
    int len = 0;

    //if other children exist in 'rte', their name must not be an empty string (GPXData struct specifications)
    if ((xmlChar)xmlRteChild->name[0] == '\0' || (xmlChar)xmlRteChild->name[0] == '\n') {
        return false;
    }

    //if other children exist in 'rte', GPXData's content must not be an empty string
    if (xmlRteChild->children == NULL) {
        return false;
    } else {
        if ((xmlChar)xmlRteChild->children->content[0] == '\0' || (xmlChar)xmlRteChild->children->content[0] == '\n') {
            return false;
        }
    }

    //parse other data's name and content
    strcpy(nameBuffer, (char*)xmlRteChild->name);
    strcpy(contentBuffer, (char *)xmlRteChild->children->content);
    len = strlen(contentBuffer); //strlen() excludes NULL terminator

    //store parsed data into GPXData struct
    otherRteData = (GPXData*)malloc(sizeof(GPXData) + (len + 1) * sizeof(char)); //malloc for len+1 chars in value[]
    strcpy(otherRteData->name, nameBuffer);
    strcpy(otherRteData->value, contentBuffer);

    insertBack(newRte->otherData, otherRteData);

    return true;
}


void storeRteName(xmlNode* xmlRteChild, Route* newRte) {
    char buffer[300] = {'\0'};
    bool isEmpty = false;
    int len = 0;

    if (xmlRteChild->children == NULL) { //name in Route struct must not be NULL
        isEmpty = true;
    } else {
        if ((xmlChar)xmlRteChild->children->content[0] == '\0' || (xmlChar)xmlRteChild->children->content[0] == '\n') {
            isEmpty = true;
        }
    }

    //store 'name' content in Route struct
    if (!isEmpty) {
        strcpy(buffer, (char*)xmlRteChild->children->content);
        len = strlen(buffer); //strlen() excludes NULL terminator
        strcpy(newRte->name, buffer);
    } else {
        strcpy(newRte->name, "");
    }

    newRte->name =(char*)realloc(newRte->name, len + 1);

    return;
}


bool storeWpt(xmlNode* curNode, GPXdoc* myGPXdoc, Route* curRte, TrackSegment* curTrkSeg) {
    Waypoint* newWpt;
    xmlAttr* attr;
    int numAttr = 0;

    newWpt = (Waypoint*)calloc(1, sizeof(Waypoint));

    //must not be NULL, but may be empty
    newWpt->name = (char*)calloc(300, sizeof(char));
    newWpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    //store Waypoint attributes in Waypoint struct
    for (attr = curNode->properties; attr != NULL; attr = attr->next) { //must have 2 Waypoint attributes
        numAttr++;
    }
    if (attr == NULL && numAttr != 2) { //Waypoint must have 2 attributes
        deleteWaypoint(newWpt);
        return false;
    }
    for (attr = curNode->properties; attr != NULL; attr = attr->next) {
        if (!storeWptAttributes(attr, newWpt)) { //latitude and longitude must be initialized
            deleteWaypoint(newWpt);
            return false;
        }
    }

    //store content other than attributes: waypoint name and other data
    for (xmlNode* xmlWptChild = curNode->children; xmlWptChild != NULL; xmlWptChild = xmlWptChild->next) {
        if (xmlWptChild->type == XML_ELEMENT_NODE) {
            if (strcmp((char*)xmlWptChild->name, "name") == 0) {
                storeWptName(xmlWptChild, newWpt);
            } else {
                if (!storeWptOtherData(xmlWptChild, newWpt)) { //content name and value must not be empty strings
                    deleteWaypoint(newWpt);
                    return false;
                }
            }
        }
    }

    //determine which struct's waypoint list to add new waypoint into
    if (myGPXdoc != NULL) {
        insertBack(myGPXdoc->waypoints, newWpt);
    } else if (curRte != NULL) {
        insertBack(curRte->waypoints, newWpt);
    } else if (curTrkSeg != NULL) {
        insertBack(curTrkSeg->waypoints, newWpt);
    }

    return true;
}


bool storeWptAttributes(xmlAttr* attr, Waypoint* newWpt) {
    double wptAttrVal;
    char *ptr;
    char buffer[100] = {'\0'};
    xmlNode* value;

    if (strcmp((char*)attr->name, "lon") != 0 && strcmp((char*)attr->name, "lat") != 0) { //can only be 'lat' or 'lon'
        return false;
    }

    value = attr->children;

    //error-checking for no latitude/longitude value (core data of the wpt)
    if (value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char *)value->content);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //attribute content must be initialized
        return false;
    }

    //store in Waypoint struct
    wptAttrVal = strtod(buffer, &ptr); //convert char* to type double
    if (strcmp((char*)attr->name, "lon") == 0) {
        newWpt->longitude = wptAttrVal;   
    } else {
        newWpt->latitude = wptAttrVal;
    }

    return true;
}


bool storeWptOtherData(xmlNode* xmlWptChild, Waypoint* newWpt) {
    char nameBuffer[256] = {'\0'}, contentBuffer[300] = {'\0'};
    GPXData* otherWptData; //objects in otherData list are of type GPXdata
    int len = 0;

    //if other children exist in 'wpt', their name must not be an empty string (GPXData struct specifications)
    if ((xmlChar)xmlWptChild->name[0] == '\0' || (xmlChar)xmlWptChild->name[0] == '\n') {
        return false;
    }

    //if other children exist in 'wpt', GPXData's content must not be an empty string
    if (xmlWptChild->children == NULL) {
        return false;
    } else {
        if ((xmlChar)xmlWptChild->children->content[0] == '\0' || (xmlChar)xmlWptChild->children->content[0] == '\n') {
            return false;
        }
    }

    //parse other data's name and content
    strcpy(nameBuffer, (char*)xmlWptChild->name);
    strcpy(contentBuffer, (char *)xmlWptChild->children->content);
    len = strlen(contentBuffer); //strlen() excludes NULL terminator

    //store parsed data into GPXData struct
    otherWptData = (GPXData*)malloc(sizeof(GPXData) + (len + 1) * sizeof(char)); //malloc for len+1 chars in value[]
    strcpy(otherWptData->name, nameBuffer);
    strcpy(otherWptData->value, contentBuffer);

    insertBack(newWpt->otherData, otherWptData);

    return true;
}


void storeWptName(xmlNode* xmlWptChild, Waypoint* newWpt) {
    char buffer[300] = {'\0'};
    bool isEmpty = false;
    int len = 0;

    if (xmlWptChild->children == NULL) { //name in Waypoint struct must not be NULL
        isEmpty = true;
    } else {
        if ((xmlChar)xmlWptChild->children->content[0] == '\0' || (xmlChar)xmlWptChild->children->content[0] == '\n') {
            isEmpty = true;
        }
    }

    //store 'name' content in Waypoint struct
    if (!isEmpty) {
        strcpy(buffer, (char *)xmlWptChild->children->content);
        len = strlen(buffer); //strlen() excludes NULL terminator
        strcpy(newWpt->name, buffer);
    } else {
        strcpy(newWpt->name, "");
    }

    newWpt->name = (char*)realloc(newWpt->name, len + 1);

    return;
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
    if (value->content == NULL) {
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


int getNumSegments(const GPXdoc* doc) {
    Track* trkPtr;
    int numSegments = 0;

    if (doc == NULL) {
        return 0;
    } 

    if (getNumTracks(doc) != 0) {
        ListIterator iter = createIterator(doc->tracks);
        while ((trkPtr = nextElement(&iter)) != NULL) {
            numSegments += getLength(trkPtr->segments);
        }
    }

    return numSegments;
}


void deleteTrackSegment(void *data) {
    TrackSegment* tempTrkSegment;

    if (data == NULL) {
        return;
    }

    tempTrkSegment = (TrackSegment*)data;

    freeList(tempTrkSegment->waypoints);
    free(tempTrkSegment);
}


int compareTrackSegments(const void *first, const void *second) {
    TrackSegment* tempTrkSeg1;
    TrackSegment* tempTrkSeg2;
    char buffer1[100] = {'\0'}, buffer2[100] = {'\0'};
    
    if (first == NULL || second == NULL) { //error-checking
        return 0;
    }

    tempTrkSeg1 = (TrackSegment*)first;
    tempTrkSeg2 = (TrackSegment*)second;

    Waypoint* tempWpt1 = (Waypoint*)getFromFront(tempTrkSeg1->waypoints);
    Waypoint* tempWpt2 = (Waypoint*)getFromFront(tempTrkSeg2->waypoints);
    strcpy(buffer1, tempWpt1->name);
    strcpy(buffer2, tempWpt2->name);

    return strcmp((char*)tempTrkSeg1->waypoints, (char*)tempTrkSeg2->waypoints);
}


char* trackSegmentToString(void* data) {
    char* result;
    char* buffer;
    TrackSegment* tempTrkSeg;
    int len;
    
    if (data == NULL) {
        return NULL;
    }
    
    result = (char*)calloc(2000, sizeof(char));
    
    tempTrkSeg = (TrackSegment*)data;
    sprintf(result, "\nTrack Segment:");

    //track points (waypoints)
    buffer = toString(tempTrkSeg->waypoints);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }

    len = strlen(result); //strlen() excludes NULL terminator
    result = (char*)realloc(result, len + 1);
        
    return result;
}


int getNumTracks(const GPXdoc* doc) {
    int numTracks;

    if (doc == NULL) {
        return 0;
    }

    numTracks = getLength(doc->tracks);
    if (numTracks == -1) {
        return 0;
    } else {
        return numTracks;
    }
}


void deleteTrack(void *data) {
    Track* tempTrk;

    if (data == NULL) {
        return;
    }

    tempTrk = (Track*)data;

    free(tempTrk->name);
    freeList(tempTrk->otherData);
    freeList(tempTrk->segments);
    free(tempTrk);
}


int compareTracks(const void *first, const void *second) {
    Track* tempTrk1;
    Track* tempTrk2;
    
    if (first == NULL || second == NULL) { //error-checking
        return 0;
    }

    tempTrk1 = (Track*)first;
    tempTrk2 = (Track*)second;
    
    return strcmp((char*)tempTrk1->name, (char*)tempTrk2->name);
}


char* trackToString(void* data) {
    char* result;
    char* buffer;
    Track* tempTrk;
    int len;
    
    if (data == NULL) {
        return NULL;
    }
    
    result = (char*)calloc(2000, sizeof(char));
    
    tempTrk = (Track*)data;
    sprintf(result, "\n*********************** \nTRACK:");

    //name
    strcat(result, "\nname: ");
    strcat(result, tempTrk->name);
    //other data
    buffer = toString(tempTrk->otherData);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }
    //track segments
    buffer = toString(tempTrk->segments);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }

    strcat(result, "\n***********************");

    len = strlen(result); //strlen() excludes NULL terminator
    result = (char*)realloc(result, len + 1);
        
    return result;
}


int getNumRoutes(const GPXdoc* doc) {
    int numRoutes;

    if (doc == NULL) {
        return 0;
    }

    numRoutes = getLength(doc->routes);
    if (numRoutes == -1) {
        return 0;
    } else {
        return numRoutes;
    }
}


void deleteRoute(void* data) {
    Route* tempRte;

    if (data == NULL) {
        return;
    }

    tempRte = (Route*)data;

    free(tempRte->name);
    freeList(tempRte->waypoints);
    freeList(tempRte->otherData);
    free(tempRte);
}


int compareRoutes(const void* first, const void* second) {
    Route* tempRte1;
    Route* tempRte2;
    
    if (first == NULL || second == NULL) { //error-checking
        return 0;
    }
    
    tempRte1 = (Route*)first;
    tempRte2 = (Route*)second;
    
    return strcmp((char*)tempRte1->name, (char*)tempRte2->name);
}


char* routeToString(void* data) {
    char* result;
    char* buffer = NULL;
    Route* tempRte;
    int len;
    
    if (data == NULL) {
        return NULL;
    }
    
    result = (char*)calloc(2000, sizeof(char));
    
    tempRte = (Route*)data;
    sprintf(result, "\n*********************** \nROUTE:");

    //name
    strcat(result, "\nname: ");
    strcat(result, tempRte->name);
    //other data
    buffer = toString(tempRte->otherData);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }
    // route points (waypoints)
    buffer = toString(tempRte->waypoints);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }

    strcat(result, "\n***********************");

    len = strlen(result); //strlen() excludes NULL terminator

    char* temp = (char*)realloc(result, len + 1);
    if (temp!= NULL) {
        result = temp;
    }

    return result;
}


Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    Waypoint* wptPtr;
    
    if(doc == NULL || name == NULL) {
        return NULL;
    }

    ListIterator iter = createIterator(doc->waypoints);
    while ((wptPtr = nextElement(&iter)) != NULL) {
        if (strcmp(wptPtr->name, name) == 0) {
            return wptPtr;
        }
    }

    return NULL; //if waypoint element with passed in name not found, return NULL
}


Track* getTrack(const GPXdoc* doc, char* name) {
    Track* trkPtr;
    
    if(doc == NULL || name == NULL) {
        return NULL;
    }

    ListIterator iter = createIterator(doc->tracks);
    while ((trkPtr = nextElement(&iter)) != NULL) {
        if (strcmp(trkPtr->name, name) == 0) {
            return trkPtr;
        }
    }

    return NULL; //if track element with passed in name not found, return NULL
}


Route* getRoute(const GPXdoc* doc, char* name) {
    Route* rtePtr;
    
    if(doc == NULL || name == NULL) {
        return NULL;
    }

    ListIterator iter = createIterator(doc->routes);
    while ((rtePtr = nextElement(&iter)) != NULL) {
        if (strcmp(rtePtr->name, name) == 0) {
            return rtePtr;
        }
    }

    return NULL; //if route element with passed in name not found, return NULL
}



int getNumWaypoints(const GPXdoc* doc) {
    int numWaypoints;

    if (doc == NULL) {
        return 0;
    }

    numWaypoints = getLength(doc->waypoints);
    if (numWaypoints == -1) {
        return 0;
    } else {
        return numWaypoints;
    }
}


void deleteWaypoint(void* data) {
    Waypoint* tempWpt;

    if (data == NULL) {
        return;
    }

    tempWpt = (Waypoint*)data;
    
    free(tempWpt->name);
    freeList(tempWpt->otherData);
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
    char* result;
    Waypoint* tempWpt;
    int len;
    
    if (data == NULL) {
        return NULL;
    }
    
    result = (char*)calloc(2000, sizeof(char));
    
    //name, lat, lon
    tempWpt = (Waypoint*)data;
    sprintf(result, "\n*********************** \nWAYPOINT:"
                    "\nname: %s \nlat: %f \nlon: %f", tempWpt->name, tempWpt->latitude, tempWpt->longitude);
    //other data
    char* buffer = toString(tempWpt->otherData);
    if (buffer != NULL) {
        strcat(result, buffer);
        free(buffer);
    }

    strcat(result, "\n***********************");

    len = strlen(result); //strlen() excludes NULL terminator
    result = (char*)realloc(result, len + 1);
        
    return result;
}


int getNumGPXData(const GPXdoc* doc) {
    Track* trkPtr;
    TrackSegment* trksegPtr;
    Route* rtePtr;
    Waypoint* wptPtr, *rteptPtr, *trkptPtr;
    int numGpxData = 0;
    char buffer[100] = {'\0'};

    if (doc == NULL) {
        return 0;
    }

    if (getNumTracks(doc) != 0) {
        ListIterator iter = createIterator(doc->tracks);
        while ((trkPtr = nextElement(&iter)) != NULL) {
            numGpxData += getLength(trkPtr->otherData); //get num GPXData structs
            //for track name
            strcpy(buffer, trkPtr->name);
            if (buffer[0] != '\0' || strcmp(buffer, "") != 0) { //increment count unless empty
                numGpxData++;
            }
            //for track segments
            ListIterator iterTrkSeg = createIterator(trkPtr->segments);
            while ((trksegPtr = nextElement(&iterTrkSeg)) != NULL) {
                //for track points
                ListIterator iterTrkPt = createIterator(trksegPtr->waypoints);
                while ((trkptPtr = nextElement(&iterTrkPt)) != NULL) {
                    numGpxData += getLength(trkptPtr->otherData); //get num GPXData structs
                    //for track point name
                    memset(buffer, 0, 100); //clear contents
                    strcpy(buffer, trkptPtr->name);
                    if (buffer[0] != '\0' || strcmp(buffer, "") != 0) { //increment count unless empty
                        numGpxData++;
                    }
                }
            }

        }
    }
    if (getNumRoutes(doc) != 0) {
        ListIterator iter = createIterator(doc->routes);
        while ((rtePtr = nextElement(&iter)) != NULL) {
            numGpxData += getLength(rtePtr->otherData); //get num GPXData structs
            //for route name
            strcpy(buffer, rtePtr->name);
            if (buffer[0] != '\0' || strcmp(buffer, "") != 0) { //increment count unless empty
                numGpxData++;
            }
            //for route points
            ListIterator iterRtept = createIterator(rtePtr->waypoints);
            while ((rteptPtr = nextElement(&iterRtept)) != NULL) {
                numGpxData += getLength(rteptPtr->otherData); //get num GPXData structs in rtept
                //for route point name
                memset(buffer, 0, 100); //clear contents
                strcpy(buffer, rteptPtr->name);
                if (buffer[0] != '\0' || strcmp(buffer, "") != 0) { //increment count unless empty
                    numGpxData++;
                }
            }
        }
    }
    if (getNumWaypoints(doc) != 0) {
        ListIterator iter = createIterator(doc->waypoints);
        while ((wptPtr = nextElement(&iter)) != NULL) {
            numGpxData += getLength(wptPtr->otherData); //get num GPXData structs
            //for waypoint name
            strcpy(buffer, wptPtr->name);
            if (buffer[0] != '\0' || strcmp(buffer, "") != 0) { //increment unless empty
                numGpxData++;
            }
        }
    }

    return numGpxData;
}



void deleteGpxData(void* data) {
    GPXData* tempOtherData;

    if (data == NULL) {
        return;
    }

    tempOtherData = (GPXData*)data;
    
    free(tempOtherData);
}


char* gpxDataToString(void* data) {
    char* result;
    GPXData* tempOtherData;
    int len;
    
    if (data == NULL) {
        return NULL;
    }

    result = (char*)calloc(2000, sizeof(char));
    
    tempOtherData = (GPXData*)data;
    sprintf(result, "other data: \n-- name: %s \n-- value: %s", tempOtherData->name, tempOtherData->value);

    len = strlen(result); //strlen() excludes NULL terminator
    result = (char*)realloc(result, len + 1);
        
    return result;
}


int compareGpxData(const void* first, const void* second) {
    GPXData* tempOtherData1;
    GPXData* tempOtherData2;
    
    if (first == NULL || second == NULL) { //error-checking
        return 0;
    }

    tempOtherData1 = (GPXData*)first;
    tempOtherData2 = (GPXData*)second;
    
    return strcmp((char*)tempOtherData1->name, (char*)tempOtherData2->name);
}


char* GPXdocToString(GPXdoc* doc) {
    char* result = (char*)calloc(10000, sizeof(char));
    char* buffer = NULL;
    int len;

    //GPX (ROOT NODE) ATTRIBUTES
    sprintf(result, "\n-------------------GPX DOC OBJECT TO STRING:-------------------"
                    "\n*********************** \nGPX NODE:"
                    "\nnamespace: %s \nversion: %f \ncreator: %s", doc->namespace, doc->version, doc->creator);
    strcat(result, " \n***********************");
    
    //WAYPOINTS
    strcat(result, "\n\n! DISPLAYING ALL WPTS !");
    buffer = toString(doc->waypoints);
    strcat(result, buffer);
    strcat(result, "\n");
    free(buffer);

    //ROUTES
    
    strcat(result, "\n! DISPLAYING ALL ROUTES !");
    buffer = toString(doc->routes);
    strcat(result, buffer);
    strcat(result, "\n");
    free(buffer);
    
    //TRACKS
    strcat(result, "\n! DISPLAYING ALL TRACKS !");
    buffer = toString(doc->tracks);
    strcat(result, buffer);
    strcat(result, "\n");
    free(buffer);

    strcat(result,"------------------END OF DOC OBJECT TO STRING------------------\n");

    len = strlen(result); //strlen() excludes NULL terminator
    result = (char*)realloc(result, len + 1);
    
    return result;
}


void deleteGPXdoc(GPXdoc *doc) {

    if (doc != NULL) { //error-checking
        free(doc->creator);
        freeList(doc->waypoints);
        freeList(doc->routes);
        freeList(doc->tracks);
        free(doc);
        doc = NULL;
    }

    xmlCleanupParser();
}
