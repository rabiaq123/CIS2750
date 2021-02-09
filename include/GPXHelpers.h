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
 * HELPER add one Waypoint struct to Waypoint list from respective struct. 
 * NOTE the struct will be either of GPXdoc type, Route type, TrackSegment type.
 * @param curNode current node from which GPX file information is being parsed
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether Waypoint was parsed correctly - halt traversal process if false
 */
bool storeWpt(xmlNode *curNode, GPXdoc *myGPXdoc);

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
 * HELPER store Waypoint name to current Waypoint struct
 * @param curNode current 'wpt' child node from which one Waypoint's information is being parsed
 * @param newWpt pointer to Waypoint struct to store name in
 * @return boolean value representing whether latitude was parsed correctly - halt traversal process if false
 */
void storeWptName(xmlNode *curNode, Waypoint *newWpt);
