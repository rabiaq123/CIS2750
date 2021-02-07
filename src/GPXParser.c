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
    //printf("in createGPXdoc()");

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
    //get the root element node 'gpx'
    rootElement = xmlDocGetRootElement(file);

    GPXdoc* myGPXdoc = (GPXdoc*)calloc(1, sizeof(GPXdoc)); //calloc() initializes memory allocated to 0
    if (!traverseGPXtree(rootElement, myGPXdoc)) { //error-checking for incorrectly-formatted GPX file
        xmlFreeDoc(file);
        deleteGPXdoc(myGPXdoc);
        return NULL;
    }

    xmlFreeDoc(file);

    return myGPXdoc;
}


bool traverseGPXtree(xmlNode* node, GPXdoc* myGPXdoc) {
    //printf("in traverseGPXtree()\n");

    xmlNode* curNode = NULL;

    for (curNode = node; curNode != NULL; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) {

            //load XML tree into GPX struct
            if (strcmp((char*)curNode->name, "gpx") == 0) {
                //error-checking for non-compliance with GPXParser.h documentation
                if (!storeGPXattributes(curNode, myGPXdoc)) { //store root node's attributes
                    return false;
                }
            }
        }
        if (!traverseGPXtree(curNode->children, myGPXdoc)) {
            return false;
        }
    }

    return true;
}


bool storeGPXattributes(xmlNode* curNode, GPXdoc* myGPXdoc) {
    //printf("in storeGPXattributes()");

    xmlAttr* attr;

    //store the namespace
    if (!storeGPXnamespace(curNode, myGPXdoc)) { //empty or non-existent ns
        return false;
    }

    //NOTE that libxml2 documentation calls xmlAttr values “properties” instead of “attributes”
    for (attr = curNode->properties; attr != NULL; attr = attr->next) {
        xmlNode* value = attr->children; //value associated with attribute

        if (strcmp((char*)attr->name, "version") == 0) { //store the version
            if (!storeGPXversion(value, myGPXdoc)) {
                return false;
            }
        } else if (strcmp((char*)attr->name, "creator") == 0) { //store the creator
            if (!storeGPXcreator(value, myGPXdoc)) {
                return false;
            }
        }
    }
    
    return true;
}


bool storeGPXcreator(xmlNode* value, GPXdoc* myGPXdoc) {
    //printf("in storeCreator()");
    
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
    //printf("creator attribute is: %s, with strlen: %d (excluding '\0')\n", myGPXdoc->creator, len);

    return true;
}


bool storeGPXversion(xmlNode* value, GPXdoc* myGPXdoc) {
    //printf("in storeVersion()");

    double versionNum;
    char* ptr;
    char buffer[100] = {'\0'};

    //error-checking for incorrectly formatted version number
    if (value->content == NULL) {
        return false;
    }

    strcpy(buffer, (char*)value->content);
    if (buffer == '\0' || strcmp(buffer, "") == 0) { //empty string, GPX version uninitialized
        return false;
    }

    //store in GPX struct
    versionNum = strtod(buffer, &ptr); //convert char* to type double
    myGPXdoc->version = versionNum;
    //printf("version is: %f\n", myGPXdoc->version);

    return true;
}


bool storeGPXnamespace(xmlNode* rootNode, GPXdoc* myGPXdoc) {
    //printf("in storeGPXnamespace()");

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
    //printf("namespace is %s, with strlen: %d (excluding '\0')\n", myGPXdoc->namespace, strlen(myGPXdoc->namespace));

    return true;
}


void deleteGPXdoc(GPXdoc* doc) {

    if (doc != NULL) { //error-checking
        free(doc->creator);
        free(doc);
        doc = NULL;
    }

    xmlCleanupParser();
}
