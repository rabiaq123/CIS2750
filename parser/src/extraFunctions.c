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


// void iterateOverWptChildren(xmlNode* curNode, Waypoint *newWpt) {
//    xmlNode *wptChildren = curNode->children; //used to iterate through and parse children of wpt node
//    xmlNode* wptChild = NULL;
//    for (curNode = wptChildren; curNode != NULL; curNode = curNode->next) {
    // for (wptChild = curNode->children; wptChild != NULL; wptChild = wptChild->next) {
    //     if (wptChild->type == XML_ELEMENT_NODE) {

    //         //load wpt children from XML file into Waypoint struct
    //         if (strcmp((char*)wptChild->name, "name") == 0) { //store wpt's name (if present)
    //             // if (curNode == NULL) {
    //             //     printf("CURNODE HELLA NULL IN HERE\n");
    //             // }
    //             printf("WE HERE\n");
    //             if ((char*)wptChild->content == NULL) {
    //                 printf("BRUH ITS NULL\n");
    //             }
    //             storeWptName(wptChild, newWpt);
    //         } 
    //         // else {
    //         //     storeOtherWptChildren(curNode, newWpt);
    //         // }
    //     }
    // }
    // if (strcmp((char*)curNode->children->name, "name") == 0) {
    //     printf("there is a name: %s\n", curNode->children->content);
    //     if (curNode->children->content != NULL) {
    //         char buffer[100] = {'\0'};
    //         strcpy(buffer, (char*)curNode->children->content);
    //         printf("NAME OF WPT IS: %s\n", buffer);
    //     }
    // }
}






// void storeOtherWptChildren(xmlNode* wptChildNode, Waypoint* newWpt) {
//     //FOR A1: if valid XML file, assume it complies with GPX format
//     //content in GPXdata struct must not be an empty string

//     GPXData* wptChildData = NULL; //object to add to otherData list of Waypoint struct
//     char contentBuffer[1000] = {'\0'}, nameBuffer[1000] = {'\0'};
//     int contentLen, nameLen;

//     //error-checking for incorrectly formatted Waypoint child content
//     // if(wptChildNode->content == NULL) {
//     //     return;
//     // }

//     strcpy(nameBuffer, (char*)wptChildNode->name);
//     contentLen = strlen(nameBuffer); //strlen() excludes NULL terminator
//     if (nameBuffer == '\0' || strcmp(nameBuffer, "") == 0) { //empty string
//         return;
//     }

//     strcpy(contentBuffer, (char*)wptChildNode->content);
//     contentLen = strlen(contentBuffer); //strlen() excludes NULL terminator
//     if (contentBuffer == '\0' || strcmp(contentBuffer, "") == 0) { //empty string
//         return;
//     }


// //    wptChildData = 

//     //store child content in otherData list of Waypoint struct
// //    strcpy(newWpt->name, buffer);
// //    newWpt->name = realloc(newWpt->name, len + 1);
//     //printf("WAYPOINT NAME: %s, STRLEN: %d (excluding '\0')\n", newWpt->name, len);

// }





// bool storeTrk(xmlNode* curNode, GPXdoc* myGPXdoc) {
//     Track* newTrk;

//     newTrk = (Track*)calloc(1, sizeof(Track));

//     //must not be NULL, may be empty
//     newTrk->name = (char*)calloc(300, sizeof(char));
//     newTrk->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
//     newTrk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

//     //store content: name, trksegs (lists of waypoints), and other data
//     for (xmlNode* xmlTrkChild = curNode->children; xmlTrkChild != NULL; xmlTrkChild = xmlTrkChild->next) {
//         if (xmlTrkChild->type == XML_ELEMENT_NODE) {
//             if (strcmp((char*)xmlTrkChild->name, "name") == 0) { //do not store 'name' child content in otherData list
//                 storeRteName(xmlTrkChild, newTrk);
//             } else if (strcmp((char*)xmlTrkChild->name, "trseg") == 0) {
//                 if (!storeTrkSeg(xmlTrkChild, NULL, NULL, newTrk)) { //store 'trkpt' in waypoints list of TrackSegment struct
//                     deleteTrack(newTrk);
//                     return false;
//                 }
//             } else {
//                 if (!storeRteOtherData(xmlTrkChild, newTrk)) { //other data's name and content must not be empty strings
//                     deleteTrack(newTrk);
//                     return false;
//                 }
//             }
//         }
//     }

//     // newRte->name = (char*)calloc(300, sizeof(char));
//     // newRte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
//     // newRte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

//     // //store content: name, rtepts (waypoints), and other data
//     // for (xmlNode* xmlRteChild = curNode->children; xmlRteChild != NULL; xmlRteChild = xmlRteChild->next) {
//     //     if (xmlRteChild->type == XML_ELEMENT_NODE) {
//     //         if (strcmp((char*)xmlRteChild->name, "name") == 0) { //do not store 'name' child content in otherData list
//     //             storeRteName(xmlRteChild, newRte);
//     //         } else if (strcmp((char*)xmlRteChild->name, "rtept") == 0) {
//     //             if (!storeWpt(xmlRteChild, NULL, newRte, NULL)) { //store 'rtept' in waypoints list of Route struct
//     //                 deleteRoute(newRte);
//     //                 return false;
//     //             }
//     //         } else {
//     //             if (!storeRteOtherData(xmlRteChild, newRte)) { //other data's name and content must not be empty strings
//     //                 deleteRoute(newRte);
//     //                 return false;
//     //             }
//     //         }
//     //     }
//     // }

//     // insertBack(myGPXdoc->routes, newRte);

//     return true;
// }





// char *GPXdocToString(GPXdoc *doc)
// {
//     char *result = (char *)calloc(10000, sizeof(char));
//     char *buffer = NULL;
//     int len;
//     // ListIterator itrWpt, itrOtherData;
//     // void* tempWpt;
//     // void* tempOtherData;

//     //GPX (ROOT NODE) ATTRIBUTES
//     sprintf(result, "\n-------------------GPX DOC OBJECT TO STRING:-------------------"
//                     "\n*********************** \nGPX NODE:"
//                     "\nnamespace: %s \nversion: %f \ncreator: %s",
//             doc->namespace, doc->version, doc->creator);
//     strcat(result, " \n***********************");

//     //WAYPOINTS
//     strcat(result, "\n\n! DISPLAYING ALL WPTS !");
//     buffer = toString(doc->waypoints);
//     strcat(result, buffer);
//     strcat(result, "\n");
//     free(buffer);

//     //ROUTES
//     strcat(result, "\n! DISPLAYING ALL ROUTES !");
//     buffer = toString(doc->routes);
//     strcat(result, buffer);
//     strcat(result, "\n");
//     free(buffer);

//     //other data (for waypoints)
//     // itrWpt = createIterator(doc->waypoints);
//     // while ((tempWpt = nextElement(&itrWpt)) != NULL) { //iterate Waypoints list
//     //     Waypoint* curWpt = (Waypoint*)tempWpt;
//     //     //iterate otherData list in each Waypoint struct
//     //     itrOtherData = createIterator(curWpt->otherData);
//     //     while((tempOtherData = nextElement(&itrOtherData)) != NULL) {
//     //         GPXData* curGPXData = (GPXData*)tempOtherData;
//     //         buffer = gpxDataToString(curGPXData);
//     //         strcat(result, buffer);
//     //         strcat(result, "\n");
//     //         if (buffer != NULL) {
//     //             free(buffer);
//     //         }
//     //     }
//     // }

//     strcat(result, "------------------END OF DOC OBJECT TO STRING------------------\n");

//     len = strlen(result); //strlen() excludes NULL terminator
//     result = realloc(result, len + 1);

//     return result;
// }




// //store track point
// for (xmlNode* xmlTrkptElem = xmlTrkChild->children; xmlTrkptElem != NULL; xmlTrkptElem = xmlTrkptElem->next) {
//     if (xmlTrkptElem->type == XML_ELEMENT_NODE) {
//         if (strcmp((char*)xmlTrkptElem->name, "trkpt") == 0) {
//             if (!storeWpt(xmlTrkChild, NULL, NULL, newTrkSeg)) { //store 'trkpt' in waypoints list of TrackSegment struct
//                 return false;
//             }
//         }
//     }
// }




    // //create a new node which is "attached" as child node of rootNode
    // xmlNewChild(rootNode, NULL, BAD_CAST "node1", BAD_CAST "content of node 1");

    // //the same as above, but the new child node does not have any content
    // xmlNewChild(rootNode, NULL, BAD_CAST "node2", NULL);

    // //xmlNewProp() creates attributes which are "attached" to a node.
    // node = xmlNewChild(rootNode, NULL, BAD_CAST "node3", BAD_CAST "this node has attributes");
    // xmlNewProp(node, BAD_CAST "attribute", BAD_CAST "yes");
    // xmlNewProp(node, BAD_CAST "foo", BAD_CAST "bar");

    // //a simple loop that automates node creation
    // for (i = 5; i < 7; i++) {
    //     sprintf(nodeName, "node%d", i);
    //     node = xmlNewChild(rootNode, NULL, BAD_CAST nodeName, NULL);
    //     for (j = 1; j < 4; j++) {
    //         sprintf(nodeName, "node%d%d", i, j);
    //         node1 = xmlNewChild(node, NULL, BAD_CAST nodeName, NULL);
    //         xmlNewProp(node1, BAD_CAST "odd", BAD_CAST((j % 2) ? "no" : "yes"));
    //     }
    // }

