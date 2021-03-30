/**
 * Name: Rabia Qureshi
 * StudentID: 1046427
 * Email: rqureshi@uoguelph.ca 
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"

/**********A3 helper functions**********/

/**
 * HELPER get number of trkpts in track (from all track segments)
 * @param trk track to find num trkpts for
 * @return num trkpts in track
 */
int getNumTrkptsInTrack(const Track *trk);

/**
 * HELPER get JSON string of one track including the number of points (trkpts) for the track
 * @param trk track to be parsed into JSON string
 * @return JSON string representing given track
 */
char* detailedTrackToJSON(const Track *trk);

/**
 * HELPER get JSON string of all tracks including the number of points (trkpts) for each track
 * @param list pointer to a List struct
 * @return JSON string representing all tracks in file
 */
char *detailedTrackListToJSON(const List *list);

/**
 * HELPER wrapper function to get components of each track in file, if file is a valid GPX
 * @param fileName name of file to create GPXdoc from
 * @return JSON array of objects, each object representing one track and its components
 */
char *getAllTrackComponentsJSON(char *fileName);

/**
 * HELPER wrapper function to get components of each route in file, if file is a valid GPX
 * @param fileName name of file to create GPXdoc from
 * @return JSON array of objects, each object representing one route and its components
 */
char *getAllRoutesComponentsJSON(char *fileName);

/**
 * HELPER convert file to GPX object and return a JSON string representation of it
 * @param filename
 * @return JSON string representation of file
 */
char *GPXFileToJSON(char *fileName);

/**********A2 helper functions**********/

/**
 * HELPER validate GPXdoc struct against GPXParser.h specifications
 * @param gpxDoc GPXDoc struct to be validated
 * @return boolean representing whether GPXdoc follows GPXParser.h specifications
 */
bool followsSpecifications(GPXdoc *gpxDoc);

/**
 * HELPER convert the GPXdoc struct to an xmlDoc (XML tree)
 * @param gpxDoc GPXDoc struct to be converted to XML ree
 * @return pointer to xmlDoc struct (XML tree) after conversion
 */
xmlDoc* convertToXMLDoc(GPXdoc* gpxDoc);

/**
 * HELPER validate the XML file against an XSD representing the GPX standard 
 * before proceeding to parse the file. 
 * @param file XMLDoc struct returned by xmlReadFile()
 * @param gpxSchemaFile schema filename
 * @return boolean representing whether XML file abides by GPX standards in XSD
 */
bool validateXML(xmlDoc* file, char* gpxSchemaFile);

/**
 * HELPER check Route constraints from GPXParser.h before proceeding to parse file. 
 * @param gpxDoc GPXdoc struct passed in by user from validateGPXDoc()
 * @return boolean representing whether Routes list and its elements abide by constraints from GPXParser.h
 */
bool validateRteGPXDoc(GPXdoc* gpxDoc);

/**
 * HELPER check Track constraints from GPXParser.h before proceeding to parse file. 
 * @param gpxDoc GPXdoc struct passed in by user from validateGPXDoc()
 * @return boolean representing whether Tracks list and its elements abide by constraints from GPXParser.h
 */
bool validateTrkGPXDoc(GPXdoc* gpxDoc);

/**
 * HELPER check Waypoint constraints from GPXParser.h before proceeding to parse file. 
 * @param gpxDoc GPXdoc struct passed in by user from validateGPXDoc()
 * @return boolean representing whether Waypoints list and its elements abide by constraints from GPXParser.h
 */
bool validateWptGPXDoc(GPXdoc* gpxDoc);

/**
 * HELPER add new Route XML node to parent node for XML tree
 * @param newRte Route struct to be parsed and added as child node of parent
 * @param pNode xmlNode representing parent node to add Route xmlNode to
 */
void createNewRte(Route* newRte, xmlNode* pNode);

/**
 * HELPER add new Waypoint XML node to parent node for XML tree
 * @param newWpt Waypoint struct to be parsed and added as child node of parent
 * @param pNode xmlNode representing parent node to add Waypoint xmlNode to
 * @param nodeName name of Waypoint node (can be 'wpt', 'rtept', or 'trkpt')
 */
void createNewWpt(Waypoint* newWpt, xmlNode* pNode, char* nodeName);

/**
 * HELPER add new Track XML node to parent node for XML tree
 * @param newTrk Track struct to be parsed and added as child node of parent
 * @param pNode xmlNode representing parent node to add Track xmlNode to
 */
void createNewTrk(Track* newTrk, xmlNode* pNode);

/**
 * HELPER calculate distance between 2 points using haversine formula
 * @param lat1 latitude of starting point
 * @param lat2 latitude of end point
 * @param lon1 longitude of starting point
 * @param lon2 longitude of end point
 * @return haversine distance
 */ 
double calcDistance(double lat1, double lat2, double lon1, double lon2);

/**
 * HELPER stub for ListAPI delete function, so the list is cleared but 
 * the nodes in the original GPXdoc are undamaged. 
 * @param data a pointer of any type to be deleted/freed
 */
void dummyDelete(void *data);

/**********A1 helper functions**********/

/**
 * HELPER traverse XML tree formed with file and parse contents into GPXdoc object
 * @param node pointer to next node to be traversed in XML tree. Will begin with root node
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether GPX struct could be created
 */
bool traverseGPXtree(xmlNode* node, GPXdoc* myGPXdoc);

/**
 * HELPER iterate through every attribute of the current node and store in GPX struct
 * @param curNode current node from which GPX file information is being parsed
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether all attributes could be parsed into GPX struct
 */
bool storeGpxAttributes(xmlNode *curNode, GPXdoc *myGPXdoc);

/**
 * HELPER store the content of the 'namespace' attribute in GPX struct
 * @param rootNode level 0 node, 'gpx'
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'namespace' was found - halt traversal process if false
 */
bool storeGpxNamespace(xmlNode *rootNode, GPXdoc *myGPXdoc);

/**
 * HELPER store the content of the 'version' attribute in GPX struct
 * @param value value associated with the 'version' attribute
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'version' number was found - halt traversal process if false
 */
bool storeGpxVersion(xmlNode *value, GPXdoc *myGPXdoc);

/**
 * HELPER store the content of the 'creator' attribute in GPX struct
 * @param value value associated with the 'version' attribute
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'creator' value was found - halt traversal process if false
 */
bool storeGpxCreator(xmlNode* value, GPXdoc* myGPXdoc);

/**
 * HELPER initialize all lists in the GPXdoc struct
 * children requiring lists: wpt, rte, trk
 */
void initializeReqLists(GPXdoc* myGPXdoc);

/**
 * HELPER add one Waypoint struct to waypoints list of GPXDoc struct, Route struct, or TrackSegment struct 
 * depending on where the waypoint is found in the file. Only one of *myGPXdoc, *curRoute, and *curTackSeg 
 * will be initialized - the rest will be passed as NULL pointer arguments.
 * @param curNode current node from which GPX file information is being parsed
 * @param myGPXdoc pointer to GPX file struct in which to store waypoint info if 'wpt' child found in 'gpx' node
 * @param curRte pointer to Route struct in which to store waypoint info if 'rtept' child found in 'rte' node
 * @param curTrkSeg pointer to TrackSegment struct in which to store waypoint info if 'trkpt' child found in 'trk' node
 * @return boolean value representing whether Waypoint was parsed correctly - halt traversal process if false
 */
bool storeWpt(xmlNode *curNode, GPXdoc *myGPXdoc, Route* curRte, TrackSegment* curTrkSeg);

/**
 * HELPER store Waypoint name in current Waypoint struct
 * @param xmlWptChild current 'wpt' child node from which one Waypoint's information is being parsed
 * @param newWpt pointer to Waypoint struct to store name in
 */
void storeWptName(xmlNode *xmlWptChild, Waypoint *newWpt);

/**
 * HELPER store Waypoint's other data in current Waypoint struct
 * @param xmlWptChild current 'wpt' child node to parse into GPXData struct for otherData list in newWpt
 * @param newWpt pointer to Waypoint struct to store other wpt data in
 * @return boolean value representing whether Waypoint's other data was formatted properly in XML file - halt traversal if false
 */
bool storeWptOtherData(xmlNode* xmlWptChild, Waypoint* newWpt);

/**
 * HELPER store Waypoint's attributes in current Waypoint struct
 * @param attr current Waypoint attribute to parse into Waypoint struct. 
 * attr->children is the value associated with the attribute
 * @param newWpt pointer to Waypoint struct to store attribute in
 */
bool storeWptAttributes(xmlAttr* attr, Waypoint* newWpt);

/**
 * HELPER add one Route struct to routes list of GPXdoc struct
 * @param curNode current node from which GPX file information is being parsed
 * @param myGPXdoc pointer to GPX file struct in which to store route info if 'rte' child found in 'gpx' node
 * @return boolean value representing whether Route was parsed correctly - halt traversal process if false
 */
bool storeRte(xmlNode* curNode, GPXdoc* myGPXdoc);

/**
 * HELPER store Route name in current Route struct
 * @param xmlRteChild current 'rte' child node from which one Route's information is being parsed
 * @param newRte pointer to Route struct to store name in
 */
void storeRteName(xmlNode* xmlRteChild, Route* newRte);

/**
 * HELPER store Waypoint's other data in current Waypoint struct
 * @param xmlRteChild current 'rte' child node to parse into otherData list in newRte
 * @param newRte pointer to Route struct to store other rte data in
 * @return boolean value representing whether Route's other data was formatted properly in XML file - halt traversal if false
 */
bool storeRteOtherData(xmlNode* xmlRteChild, Route* newRte);

/**
 * HELPER add one Track struct to tracks list of GPXdoc struct
 * @param curNode current node from which GPX file information is being parsed
 * @param myGPXdoc pointer to GPX file struct in which to store track info if 'trk' child found in 'gpx' node
 * @return boolean value representing whether Track was parsed correctly - halt traversal process if false
 */
bool storeTrk(xmlNode* curNode, GPXdoc* myGPXdoc);

/**
 * HELPER store Track name in current Track struct
 * @param xmlTrkChild current 'trk' child node from which one Track's information is being parsed
 * @param newTrk pointer to Track struct to store name in
 */
void storeTrkName(xmlNode* xmlTrkChild, Track* newTrk);

/**
 * HELPER store Track's other data in current Track struct
 * @param xmlTrkChild current 'trk' child node to parse into otherData list in newTrk
 * @param newTrk pointer to Track struct to store other trk data in
 * @return boolean value representing whether Track's other data was formatted properly in XML file - halt traversal if false
 */
bool storeTrkOtherData(xmlNode* xmlTrkChild, Track* newTrk);

/**
 * HELPER add one TrackSegment struct to segments list of Track struct
 * @param xmlTrkChild current 'trk' child node to parse into segments list in newTrk
 * @param newTrk pointer to Track struct to store trkseg data in
 * @return boolean value representing whether TrackSeg was parsed correctly - halt traversal process if false
 */
bool storeTrkSeg(xmlNode* xmlTrkChild, Track* newTrk);
