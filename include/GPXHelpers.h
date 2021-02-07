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
 * traverse XML tree formed with file and parse contents into GPXdoc object
 * @param node pointer to next node to be traversed in XML tree. Will begin with root node
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether GPX struct could be created
 */
bool traverseGPXtree(xmlNode* node, GPXdoc* myGPXdoc);

/**
 * iterate through every attribute of the current node and store in GPX struct
 * @param curNode current node being updated to store GPX file information
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether all attributes could be parsed into GPX struct
 */
bool storeGPXattributes(xmlNode *curNode, GPXdoc *myGPXdoc);

/**
 * store the 'namespace' content in GPX struct
 * @param rootNode level 0 node, 'gpx'
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'namespace' was found - halt traversal process if false
 */
bool storeGPXnamespace(xmlNode *rootNode, GPXdoc *myGPXdoc);

/**
 * store the content of the 'version' attribute in GPX struct
 * @param value value associated with the 'version' attribute
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'version' number was found - halt traversal process if false
 */
bool storeGPXversion(xmlNode *value, GPXdoc *myGPXdoc);

/**
 * store the content of the 'creator' attribute in GPX struct
 * @param value value associated with the 'version' attribute
 * @param myGPXdoc pointer to GPX struct in which to store entire GPX file
 * @return boolean value representing whether valid 'creator' value was found - halt traversal process if false
 */
bool storeGPXcreator(xmlNode* value, GPXdoc* myGPXdoc);

/**
 * temporarily store string content in buffer
 * @param value current node
 * @param len string length of content
 * @return buffer containing content
 */
char *tempStoreInBuffer(xmlNode* value, int len);