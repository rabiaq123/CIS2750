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
 * HELPER store longitude attribute in current Waypoint struct
 * @param curNode current node from which GPX file information is being parsed
 * @param newWpt pointer to Waypoint struct to store info in
 * @return boolean value representing whether longitude was parsed correctly - halt traversal process if false
 */
bool storeWptLongitude(xmlNode *curNode, Waypoint *newWpt);

/**
 * HELPER store latitude attribute in current Waypoint struct
 * @param curNode current node from which GPX file information is being parsed
 * @param newWpt pointer to Waypoint struct to store info in
 * @return boolean value representing whether latitude was parsed correctly - halt traversal process if false
 */
bool storeWptLatitude(xmlNode *curNode, Waypoint *newWpt);

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

