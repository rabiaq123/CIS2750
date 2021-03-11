/**
 * Name: Rabia Qureshi
 * StudentID: 1046427
 * Email: rqureshi@uoguelph.ca
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>

#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "GPXHelpers.h"

/**
 * Note that as allowed by Professor Nikitenko,
 * the traverseGPXtree() function contains sample code from the 
 * libXmlExample.c file provided in class, which is based on the LibXML example from:
 * http://www.xmlsoft.org/examples/tree1.c.
 * Additionally, the validateXML() function contains sample code from the
 * resource shared in the A2 Module 1 description sheet:
 * http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 */


GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile) {
    xmlDoc* file = NULL;
    xmlNode* rootElement = NULL;

    LIBXML_TEST_VERSION

    //error-checking for invalid arguments
    if (gpxSchemaFile == NULL || strcmp(gpxSchemaFile, "") == 0) return NULL;
    if (fileName == NULL || strcmp(fileName, "") == 0) return NULL;
    if (strstr(fileName, ".gpx") == NULL) return NULL; //check for .gpx substring in filename

    //attempt to validate XML file before creating GPXdoc
    file = xmlReadFile(fileName, NULL, 0);
    if (file == NULL) return NULL; //not a well-formed XML

    if (!validateXML(file, gpxSchemaFile)) { //validate XML file against GPX schema
        xmlFreeDoc(file);
        return NULL;
    }

    rootElement = xmlDocGetRootElement(file); //get root element node 'gpx'
    GPXdoc* myGPXdoc = (GPXdoc*)calloc(1, sizeof(GPXdoc)); //calloc() initializes memory allocated to 0
    initializeReqLists(myGPXdoc);

    //build GPXdoc unless GPX file does not follow GPXParser.h specifications
    if (!traverseGPXtree(rootElement, myGPXdoc)) {
        xmlFreeDoc(file);
        deleteGPXdoc(myGPXdoc);
        return NULL;
    }

    xmlFreeDoc(file);

    return myGPXdoc;
}


bool validateGPXDoc(GPXdoc* gpxDoc, char* gpxSchemaFile) {
    xmlDoc* file;

    //error-checking for invalid arguments
    if (gpxDoc == NULL) return false;
    if (gpxSchemaFile == NULL || strcmp(gpxSchemaFile, "") == 0) return false;
    if (strstr(gpxSchemaFile, ".xsd") == NULL) return false; //check for .xsd substring in schema filename

    //manually check the constraints of GPXdoc struct against specifications in GPXParser.h
    if (!followsSpecifications(gpxDoc)) return false;

    //check whether GPXdoc represents valid GPX image based on GPX schema once converted to XML
    file = convertToXMLDoc(gpxDoc);
    if (!validateXML(file, gpxSchemaFile)) {
        xmlFreeDoc(file);
        return false;
    }

    xmlFreeDoc(file);

    return true;
}


bool followsSpecifications(GPXdoc *gpxDoc) {
    if (gpxDoc->creator == NULL || strcmp(gpxDoc->creator, "") == 0) return false;
    if (gpxDoc->namespace == NULL || strcmp(gpxDoc->namespace, "") == 0) return false;
    if (gpxDoc->routes == NULL|| gpxDoc->tracks == NULL || gpxDoc->waypoints == NULL) return false;

    //validate against constraints for GPXdoc lists
    if (!validateRteGPXDoc(gpxDoc)) return false; //routes list
    if (!validateTrkGPXDoc(gpxDoc)) return false; //tracks list
    if (!validateWptGPXDoc(gpxDoc)) return false; //waypoints list

    return true;
}


bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    if (doc == NULL || fileName == NULL || strcmp(fileName, "") == 0) return false;

    int status;

    xmlDoc* xmlTree = convertToXMLDoc(doc);
    status = xmlSaveFormatFileEnc(fileName, xmlTree, "UTF-8", 1);
    if (status < 0) return false; //could not save to XML file

    xmlFreeDoc(xmlTree);
    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}


bool validateXML(xmlDoc* file, char* gpxSchemaFile) {
    xmlSchema* schema = NULL;
    xmlSchemaParserCtxt* ctxt;
    int status;

    LIBXML_SCHEMAS_ENABLED

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xmlSchemaValidCtxt* ctxt2; //XML schema validation context

    ctxt2 = xmlSchemaNewValidCtxt(schema);
    xmlSchemaSetValidErrors(ctxt2, (xmlSchemaValidityErrorFunc)fprintf, (xmlSchemaValidityWarningFunc)fprintf, stderr);
    status = xmlSchemaValidateDoc(ctxt2, file);
    if (status != 0) return false; //validation failed due to internal or other reason

    xmlSchemaFreeValidCtxt(ctxt2);
    if (schema != NULL) xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();
//    xmlCleanupParser();
    xmlMemoryDump();

    return true;
}


xmlDoc* convertToXMLDoc(GPXdoc* gpxDoc) {
    xmlDoc* doc = NULL;
    xmlNode* rootNode = NULL;
    xmlNs* nsPtr = NULL;
    void* elem;
    ListIterator iter;
    char version[10] = {'\0'};

    LIBXML_TEST_VERSION
    LIBXML_TREE_ENABLED
    LIBXML_OUTPUT_ENABLED
    
    //create a new document, a node, and set it as a root node with its version
    doc = xmlNewDoc(BAD_CAST "1.0");
    rootNode = xmlNewNode(NULL, BAD_CAST "gpx");
    //set 'creator' and 'version' attribute for node
    sprintf(version, "%.1f", gpxDoc->version);
    xmlNewProp(rootNode, BAD_CAST "version", BAD_CAST version);
    xmlNewProp(rootNode, BAD_CAST "creator", BAD_CAST gpxDoc->creator);
    //create 'namespace' and set for node
    nsPtr = xmlNewNs(rootNode, BAD_CAST gpxDoc->namespace, NULL);
    xmlSetNs(rootNode, nsPtr);
    //set node as root element
    xmlDocSetRootElement(doc, rootNode);

    //find Waypoint children nodes of root node
    iter = createIterator(gpxDoc->waypoints);
    while ((elem = nextElement(&iter)) != NULL) {
        Waypoint* newWpt = (Waypoint*)elem;
        createNewWpt(newWpt, rootNode, "wpt");
    }

    //find Route children nodes of root node
    iter = createIterator(gpxDoc->routes);
    while ((elem = nextElement(&iter)) != NULL) {
        Route* newRte = (Route*)elem;
        createNewRte(newRte, rootNode);
    }
    
    //find Track children nodes of root node
    iter = createIterator(gpxDoc->tracks);
    while ((elem = nextElement(&iter)) != NULL) {
        Track* newTrk = (Track*)elem;
        createNewTrk(newTrk, rootNode);
    }

    return doc;
}


void createNewTrk(Track* curTrk, xmlNode* pNode) {
    xmlNode* trkNode = NULL, *trkSegNode;
    void* elem, *elem2;
    ListIterator iter;

    //add Track to parent node
    trkNode = xmlNewChild(pNode, NULL, BAD_CAST "trk", NULL);
    xmlNewChild(trkNode, NULL, BAD_CAST "name", BAD_CAST trkNode->name);

    //for otherData list in Track node
    iter = createIterator(curTrk->otherData);
    while ((elem = nextElement(&iter)) != NULL) {
        GPXData* newOD = (GPXData*)elem;
        xmlNewChild(trkNode, NULL, BAD_CAST newOD->name, BAD_CAST newOD->value);
    }

    //for track segments list in Track node
    iter = createIterator(curTrk->segments);
    while ((elem = nextElement(&iter)) != NULL) {
        TrackSegment* newTrkseg = (TrackSegment*)elem;
        trkSegNode = xmlNewChild(trkNode, NULL, BAD_CAST "trkseg", NULL);
        //for waypoints list in Track node
        ListIterator iter2 = createIterator(newTrkseg->waypoints);
        while ((elem2 = nextElement(&iter2)) != NULL) {
            Waypoint* newTrkpt = (Waypoint*)elem2;
            createNewWpt(newTrkpt, trkSegNode, "trkpt");
        }
    }
}


void createNewWpt(Waypoint* curWpt, xmlNode* pNode, char* nodeName) {
    xmlNode* wptNode = NULL;
    char buffer[256] = {'\0'};
    void* elem;
    
    strcpy(buffer, nodeName);

    //add Waypoint to parent node
    wptNode = xmlNewChild(pNode, NULL, BAD_CAST buffer, NULL); //nodeName can be wpt, rtept, trkpt
    memset(buffer, '\0', 256);
    xmlNewChild(wptNode, NULL, BAD_CAST "name", BAD_CAST curWpt->name);

    //add lat and lon attributes to Waypoint
    sprintf(buffer, "%f", curWpt->latitude);
    xmlNewProp(wptNode, BAD_CAST "lat", BAD_CAST buffer);
    memset(buffer, '\0', 256);
    sprintf(buffer, "%f", curWpt->longitude);
    xmlNewProp(wptNode, BAD_CAST "lon", BAD_CAST buffer);
    memset(buffer, '\0', 256);

    //for otherData list in Waypoint node
    ListIterator iter = createIterator(curWpt->otherData);
    while ((elem = nextElement(&iter)) != NULL) {
        GPXData* newOD = (GPXData*)elem;
        xmlNewChild(wptNode, NULL, BAD_CAST newOD->name, BAD_CAST newOD->value);
    }
}

void createNewRte(Route* curRte, xmlNode* pNode) {
    xmlNode* rteNode = NULL;
    ListIterator iter;
    void* elem;

    //add rte to root node 'gpx'
    rteNode = xmlNewChild(pNode, NULL, BAD_CAST "rte", NULL);
    xmlNewChild(rteNode, NULL, BAD_CAST "name", BAD_CAST curRte->name);

    //for otherData list in Route node
    iter = createIterator(curRte->otherData);
    while ((elem = nextElement(&iter)) != NULL) {
        GPXData* newOD = (GPXData*)elem;
        xmlNewChild(rteNode, NULL, BAD_CAST newOD->name, BAD_CAST newOD->value);
    }

    //for waypoints list in Route node
    iter = createIterator(curRte->waypoints);
    while ((elem = nextElement(&iter)) != NULL) {
        Waypoint* newRtept = (Waypoint*)elem;
        createNewWpt(newRtept, rteNode, "rtept");
    }
}


bool validateWptGPXDoc(GPXdoc* gpxDoc) {
    Waypoint* wptPtr;
    GPXData* gpxDataPtr; //otherData 

    //for every Waypoint node in waypoints list
    ListIterator iterWpt = createIterator(gpxDoc->waypoints);
    while ((wptPtr = nextElement(&iterWpt)) != NULL) {
        if (wptPtr->name == NULL) return false;
        if (wptPtr->latitude == 0.00 || wptPtr->longitude == 0.00) return false;
        if (wptPtr->otherData == NULL) return false;
        //for otherData list in Waypoint node
        ListIterator iterWptOD = createIterator(wptPtr->otherData);
        while ((gpxDataPtr = nextElement(&iterWptOD)) != NULL) {
            if (gpxDataPtr->name == NULL || strcmp(gpxDataPtr->name, "") == 0) return false;
            if (gpxDataPtr->value == NULL || strcmp(gpxDataPtr->value,"") == 0) return false;
        }
    }

    return true;
}


bool validateTrkGPXDoc(GPXdoc* gpxDoc) {
    Track *trkPtr;
    TrackSegment *trksegPtr;
    Waypoint *trkptPtr;
    GPXData* gpxDataPtr; //otherData 

    //for every Track node in tracks list
    ListIterator iterTrk = createIterator(gpxDoc->tracks);
    while ((trkPtr = nextElement(&iterTrk)) != NULL) {
        if (trkPtr->name == NULL) return false;
        if (trkPtr->segments == NULL) return false;
        if (trkPtr->otherData == NULL) return false;

        //for otherData list in Track node
        ListIterator iterTrkOD = createIterator(trkPtr->otherData);
        while ((gpxDataPtr = nextElement(&iterTrkOD)) != NULL) {
            if (gpxDataPtr->name == NULL || strcmp(gpxDataPtr->name, "") == 0) return false;
            if (gpxDataPtr->value == NULL || strcmp(gpxDataPtr->value,"") == 0) return false;
        }

        //for segments list in Track node
        ListIterator iterTrkSeg = createIterator(trkPtr->segments);
        while ((trksegPtr = nextElement(&iterTrkSeg)) != NULL) {
            if (trksegPtr->waypoints == NULL) return false;
            //for waypoints list in TrackSegment node
            ListIterator iterTrkpt = createIterator(trksegPtr->waypoints);
            while ((trkptPtr = nextElement(&iterTrkpt)) != NULL) {
                if (trkptPtr->name == NULL) return false;
                if (trkptPtr->latitude == 0.00 || trkptPtr->longitude == 0.00) return false;
                if (trkptPtr->otherData == NULL) return false;
                //for otherData list in trkpt (Waypoint) node
                ListIterator iterTrkptOD = createIterator(trkptPtr->otherData);
                while ((gpxDataPtr = nextElement(&iterTrkptOD)) != NULL) {
                    if (gpxDataPtr->name == NULL || strcmp(gpxDataPtr->name, "") == 0) return false;
                    if (gpxDataPtr->value == NULL || strcmp(gpxDataPtr->value,"") == 0) return false;
                }
            }
        }
    }

    return true;
}


bool validateRteGPXDoc(GPXdoc* gpxDoc) {
    Route* rtePtr;
    Waypoint* rteptPtr;
    GPXData* gpxDataPtr; //otherData 

    //for every Route node in routes list
    ListIterator iterRte = createIterator(gpxDoc->routes);
    while ((rtePtr = nextElement(&iterRte)) != NULL) {
        if (rtePtr->name == NULL) return false;
        if (rtePtr->waypoints == NULL) return false;
        if (rtePtr->otherData == NULL) return false;

        //for waypoints list in Route node
        ListIterator iterRtept = createIterator(rtePtr->waypoints);
        while ((rteptPtr = nextElement(&iterRtept)) != NULL) {
            if (rteptPtr->name == NULL) return false;
            if (rteptPtr->latitude == 0.00 || rteptPtr->longitude == 0.00) return false;
            if (rteptPtr->otherData == NULL) return false;
            //for otherData list in rtept (Waypoint) node
            ListIterator iterRteptOD = createIterator(rteptPtr->otherData);
            while ((gpxDataPtr = nextElement(&iterRteptOD)) != NULL) {
                if (gpxDataPtr->name == NULL || strcmp(gpxDataPtr->name, "") == 0) return false;
                if (gpxDataPtr->value == NULL || strcmp(gpxDataPtr->value,"") == 0) return false;
            }
        }

        //for otherData list in Route node
        ListIterator iterRteOD = createIterator(rtePtr->otherData);
        while ((gpxDataPtr = nextElement(&iterRteOD)) != NULL) {
            if (gpxDataPtr->name == NULL || strcmp(gpxDataPtr->name, "") == 0) return false;
            if (gpxDataPtr->value == NULL || strcmp(gpxDataPtr->value,"") == 0) return false;
        }
    }

    return true;
}


float round10(float len) {
    int diff, roundedUp, roundedDown;
    
    //if one's digit is 5, round up
    diff = (int)len % 10;
    if (diff == 5) return (len + 5);

    //if one's digit is not 5, round to the nearest 10m
    roundedDown = (int)(len / 10) * 10;
    roundedUp = roundedDown + 10;
    return ((len - roundedDown) > (roundedUp - len))? roundedUp : roundedDown;
}


float getRouteLen(const Route *rte) {
    if (rte == NULL) return 0;
    
    Waypoint* rtept1, *rtept2;
    double lat1, lat2, lon1, lon2;
    float d;

    ListIterator iter = createIterator(rte->waypoints);
    rtept1 = getFromFront(rte->waypoints);
    while ((rtept2 = nextElement(&iter)) != NULL) {
        lat1 = rtept1->latitude;
        lon1 = rtept1->longitude;
        lat2 = rtept2->latitude;
        lon2 = rtept2->longitude;
        d += calcDistance(lat1, lon1, lat2, lon2);
        rtept1 = rtept2; //second wpt will be first in the next iteration (e.g. dist from wpt1->wpt2 and wpt2->wpt3)
    }

    return d;
}


float getTrackLen(const Track* trk) {
    if (trk == NULL) return 0;

    Waypoint* wpt1, *wpt2, *segStart;
    double lat1, lat2, lon1, lon2;
    float d;
    TrackSegment *trkseg;
    int numSegments = 0;

    ListIterator iter = createIterator(trk->segments);
    while ((trkseg = nextElement(&iter)) != NULL) {
        //calculating distance between between adjacent segments
        segStart = getFromFront(trkseg->waypoints);
        numSegments++;
        if (numSegments >= 2) {
            d += calcDistance(lat1, lon1, segStart->latitude, segStart->longitude); //previous trkpt to start of new seg
        }
        //calculating distance between trkpts
        ListIterator iterTrkpt = createIterator(trkseg->waypoints);
        wpt1 = segStart;
        while ((wpt2 = nextElement(&iterTrkpt)) != NULL) {
            lat1 = wpt1->latitude;
            lon1 = wpt1->longitude;
            lat2 = wpt2->latitude;
            lon2 = wpt2->longitude;
            d += calcDistance(lat1, lon1, lat2, lon2);
            wpt1 = wpt2; //second wpt will be first in the next iteration (e.g. wpt1 to wpt2 & wpt2 to wpt3)
        }
    }

    return d;
}


double calcDistance(double lat1, double lon1, double lat2, double lon2) {
    double a, c, dist;

    //convert to radians
    lat1 *= (M_PI/180);
    lon1 *= (M_PI/180);
    lat2 *= (M_PI/180);
    lon2 *= (M_PI/180);
    //middle steps
    double latCalc = sin((lat2 - lat1) / 2.0);
    double lonCalc = sin((lon2 - lon1) / 2.0);
    //calculate haversine
    a = sin(latCalc) * sin(latCalc) + cos(lat1) * cos(lat2) * sin(lonCalc) * sin(lonCalc);
    c = 2.0 * atan2(sqrt(a), sqrt(1-a));
    dist = 6371e3 * c;

    return dist;
}


int numRoutesWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL || len < 0 || delta < 0) return 0;

    Route *rte;
    int numRoutes = 0;

    ListIterator iter = createIterator(doc->routes);
    while((rte = nextElement(&iter)) != NULL) {
        float routeLen = getRouteLen(rte);
        float diff = fabs(routeLen - len);
        if (diff <= delta) numRoutes++; //within acceptable range
    }

    return numRoutes;
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL || len < 0 || delta < 0) return 0;

    Track *trk;
    int numTracks = 0;

    ListIterator iter = createIterator(doc->tracks);
    while ((trk = nextElement(&iter)) != NULL) {
        float trackLen = getTrackLen(trk);
        float diff = fabs(trackLen - len);
        if (diff <= delta) numTracks++;
    }

    return numTracks;
}


bool isLoopRoute(const Route* rte, float delta) {
    if (rte == NULL || delta < 0) return false;
    
    Waypoint *rtept1, *rtept2;
    float lat1, lat2, lon1, lon2;
    float d;

    int numRtepts = getLength(rte->waypoints);
    rtept1 = getFromFront(rte->waypoints);
    rtept2 = getFromBack(rte->waypoints);

    //calculate distance between first and last rtepts and compare to delta
    lat1 = rtept1->latitude;
    lon1 = rtept1->longitude;
    lat2 = rtept2->latitude;
    lon2 = rtept2->longitude;
    d = calcDistance(lat1, lon1, lat2, lon2);

    //check Loop Route condition (satisfied if numRtepts >=4 && d < delta)
    if (numRtepts < 4 && d >= delta) return false;

    return true;
}


bool isLoopTrack(const Track *trk, float delta) {
    if (trk == NULL || delta < 0) return false;

    TrackSegment *trkseg1, *trkseg2;
    Waypoint *trkpt1, *trkpt2;
    TrackSegment *trkseg; //used in loop iteration
    int numTrkpts = 0;
    float lat1, lat2, lon1, lon2;
    float d;

    //get first Track Segment and its first trkpt (Waypoint)
    trkseg1 = getFromFront(trk->segments);
    trkpt1 = getFromFront(trkseg1->waypoints);
    //get last Track Segment and its last trkpt (Waypoint)
    trkseg2 = getFromBack(trk->segments);
    trkpt2 = getFromBack(trkseg2->waypoints);

    //calculate total number of trkpts (Waypoints) in track
    ListIterator iter = createIterator(trk->segments);
    while ((trkseg = nextElement(&iter)) != NULL) {
        numTrkpts += getLength(trkseg->waypoints);
    }

    //calculate distance between first and last trkpts and compare to delta
    lat1 = trkpt1->latitude;
    lon1 = trkpt1->longitude;
    lat2 = trkpt2->latitude;
    lon2 = trkpt2->longitude;
    d = calcDistance(lat1, lon1, lat2, lon2);

    //check Loop Track condition (satisfied if numTrkpts >=4 && d < delta)
    if (numTrkpts < 4 && d >= delta) return false;

//    printf("\n\nNUM TRACK POINTS IS %d and DISTANCE IS %f\n\n", numTrkpts, d);

    return true;
}


List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL) return NULL;

    List *rteList = initializeList(&routeToString, &dummyDelete, &compareRoutes);
    Route *rte;
    Waypoint *rtept1, *rtept2;
    int dist1, dist2;

    //for every Route node in routes list
    ListIterator iter = createIterator(doc->routes);
    while ((rte = nextElement(&iter)) != NULL) {
        //calculate haversine for first rtept
        rtept1 = getFromFront(rte->waypoints);
        dist1 = calcDistance(rtept1->latitude, rtept1->longitude, sourceLat, sourceLong);
        //calculate haversine for destination rtept
        rtept2 = getFromBack(rte->waypoints);
        dist2 = calcDistance(rtept2->latitude, rtept2->longitude, destLat, destLong);
        //if both source and destination are within (<=) delta, add Route to the list
        if (dist1 <= delta && dist2 <= delta) insertBack(rteList, rte);
    }

    if (getLength(rteList) == 0) return 0;

    return rteList;
}


void dummyDelete(void *data) {
    return;
}


List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL) return NULL;

    List *trkList = initializeList(&trackToString, &dummyDelete, &compareTracks);
    Track *trk;
    TrackSegment *trkseg;
    Waypoint *trkpt1, *trkpt2;
    int dist1, dist2;

    //for every Track node in tracks list
    ListIterator iter = createIterator(doc->tracks);
    while ((trk = nextElement(&iter)) != NULL) {
        ListIterator iterTrkseg = createIterator(trk->segments);
        while((trkseg = nextElement(&iterTrkseg)) != NULL) {
            //calculate haversine for first trkpt
            trkpt1 = getFromFront(trkseg->waypoints);
            dist1 = calcDistance(trkpt1->latitude, trkpt1->longitude, sourceLat, sourceLong);
            //calculate haversine for destination trkpt
            trkpt2 = getFromBack(trkseg->waypoints);
            dist2 = calcDistance(trkpt2->latitude, trkpt2->longitude, destLat, destLong);
            //if both source and destination are within (<=) delta, add Track to the list
            if (dist1 <= delta && dist2 <= delta) insertBack(trkList, trk);
        }
    }

    if (getLength(trkList) == 0) return 0;

    return trkList;
}


/***A2 incomplete functions***/


char* trackToJSON(const Track *trk) {
    if (trk == NULL) return "{}";

    return "placeholder";
}


char* routeToJSON(const Route *rte) {
    if (rte == NULL) return "{}";

    return "placeholder";
}


char *routeListToJSON(const List *list){
    if (list == NULL) return "{}";

    return "placeholder";
}


char* trackListToJSON(const List *list) {
    if (list == NULL) return "{}";

    return "placeholder";
}


char* GPXtoJSON(const GPXdoc* gpx) {
    if (gpx == NULL) return "{}";

    return "placeholder";
}


/***A2 bonus functions***/


void addWaypoint(Route *rt, Waypoint *wpt) {
    return;
}

void addRoute(GPXdoc *doc, Route *rte) {
    return;
}

GPXdoc* JSONtoGPX(const char* gpxString) {
    return NULL;
}

Waypoint* JSONtoWaypoint(const char* gpxString) {
    return NULL;
}

Route *JSONtoRoute(const char *gpxString) {
    return NULL;
}


/**********A1 functions**********/


GPXdoc* createGPXdoc(char* fileName) {
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

    if (doc == NULL) return 0;

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

    if (data == NULL) return;

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
    char* string;
    char* buffer;
    TrackSegment* tempTrkSeg;
    int len;
    
    if (data == NULL) return NULL;
    
    string = (char*)calloc(2000, sizeof(char));
    tempTrkSeg = (TrackSegment*)data;

    sprintf(string, "\nTrack Segment:");

    //track points (waypoints)
    buffer = toString(tempTrkSeg->waypoints);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }

    len = strlen(string); //strlen() excludes NULL terminator
    string = (char*)realloc(string, len + 1);
        
    return string;
}


int getNumTracks(const GPXdoc* doc) {
    int numTracks;

    if (doc == NULL) return 0;

    numTracks = getLength(doc->tracks);
    if (numTracks == -1) {
        return 0;
    } else {
        return numTracks;
    }
}


void deleteTrack(void *data) {
    Track* tempTrk;

    if (data == NULL) return;

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
    char* string;
    char* buffer;
    Track* tempTrk;
    int len;
    
    if (data == NULL) return NULL;
    
    string = (char*)calloc(2000, sizeof(char));
    tempTrk = (Track*)data;

    sprintf(string, "\n*********************** \nTRACK:");

    //name
    strcat(string, "\nname: ");
    strcat(string, tempTrk->name);
    //other data
    buffer = toString(tempTrk->otherData);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }
    //track segments
    buffer = toString(tempTrk->segments);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }

    strcat(string, "\n***********************");

    len = strlen(string); //strlen() excludes NULL terminator
    string = (char*)realloc(string, len + 1);
        
    return string;
}


int getNumRoutes(const GPXdoc* doc) {
    int numRoutes;

    if (doc == NULL) return 0;

    numRoutes = getLength(doc->routes);
    if (numRoutes == -1) {
        return 0;
    } else {
        return numRoutes;
    }
}


void deleteRoute(void* data) {
    Route* tempRte;

    if (data == NULL) return;

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
    char* string;
    char* buffer = NULL;
    Route* tempRte;
    int len;
    
    if (data == NULL) return NULL;
    
    string = (char*)calloc(2000, sizeof(char));
    tempRte = (Route*)data;

    sprintf(string, "\n*********************** \nROUTE:");
    //name
    strcat(string, "\nname: ");
    strcat(string, tempRte->name);
    //other data
    buffer = toString(tempRte->otherData);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }
    // route points (waypoints)
    buffer = toString(tempRte->waypoints);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }
    strcat(string, "\n***********************");

    len = strlen(string); //strlen() excludes NULL terminator

    char* temp = (char*)realloc(string, len + 1);
    if (temp!= NULL) {
        string = temp;
    }

    return string;
}


Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    Waypoint* wptPtr;
    
    if(doc == NULL || name == NULL) return NULL;

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
    
    if(doc == NULL || name == NULL) return NULL;

    ListIterator iter = createIterator(doc->tracks);
    while ((trkPtr = nextElement(&iter)) != NULL) {
        if (strcmp(trkPtr->name, name) == 0) {
            return trkPtr;
        }
    }

    return NULL; //if track element with passed in name not found
}


Route* getRoute(const GPXdoc* doc, char* name) {
    Route* rtePtr;
    
    if(doc == NULL || name == NULL) return NULL;

    ListIterator iter = createIterator(doc->routes);
    while ((rtePtr = nextElement(&iter)) != NULL) {
        if (strcmp(rtePtr->name, name) == 0) {
            return rtePtr;
        }
    }

    return NULL; //if route element with passed in name not found
}


int getNumWaypoints(const GPXdoc* doc) {
    int numWaypoints;

    if (doc == NULL) return 0;

    numWaypoints = getLength(doc->waypoints);
    if (numWaypoints == -1) {
        return 0;
    } else {
        return numWaypoints;
    }
}


void deleteWaypoint(void* data) {
    Waypoint* tempWpt;

    if (data == NULL) return;

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
    char* string;
    Waypoint* tempWpt;
    int len;
    
    if (data == NULL) return NULL;
    
    string = (char*)calloc(2000, sizeof(char));
    tempWpt = (Waypoint*)data;

    //name, lat, lon
    sprintf(string, "\n*********************** \nWAYPOINT:"
                    "\nname: %s \nlat: %f \nlon: %f", tempWpt->name, tempWpt->latitude, tempWpt->longitude);
    //other data
    char* buffer = toString(tempWpt->otherData);
    if (buffer != NULL) {
        strcat(string, buffer);
        free(buffer);
    }
    strcat(string, "\n***********************");

    len = strlen(string); //strlen() excludes NULL terminator
    string = (char*)realloc(string, len + 1);

    return string;
}


int getNumGPXData(const GPXdoc* doc) {
    void* elem;
    TrackSegment* trksegPtr;
    Waypoint* rteptPtr, *trkptPtr;
    int numGpxData = 0;
    char buffer[100] = {'\0'};

    if (doc == NULL) return 0;

    if (getNumTracks(doc) != 0) {
        ListIterator iter = createIterator(doc->tracks);
        while ((elem = nextElement(&iter)) != NULL) {
            Track* trkPtr = (Track*)elem;
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
        while ((elem = nextElement(&iter)) != NULL) {
            Route* rtePtr = (Route*)elem;
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
        while ((elem = nextElement(&iter)) != NULL) {
            Waypoint* wptPtr = (Waypoint*)elem;
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

    if (data == NULL) return;

    tempOtherData = (GPXData*)data;
    free(tempOtherData);
}


char* gpxDataToString(void* data) {
    char* string;
    GPXData* tempOtherData;
    int len;
    
    if (data == NULL) return NULL;

    string = (char*)calloc(2000, sizeof(char));
    tempOtherData = (GPXData*)data;
    sprintf(string, "other data: \n-- name: %s \n-- value: %s", tempOtherData->name, tempOtherData->value);

    len = strlen(string); //strlen() excludes NULL terminator
    string = (char*)realloc(string, len + 1);

    return string;
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
