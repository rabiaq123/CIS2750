/**
 * Name: Rabia Qureshi
 * Student ID: 1046427
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


static void printElementNames(xmlNode* node) {
    xmlNode *curNode = NULL;

    for (curNode = node; curNode != NULL; curNode = curNode->next) {
        if (curNode->type == XML_ELEMENT_NODE) { //if the XML element type is node
            printf("node type: Element, name: %s\n", curNode->name);
        }

        //uncomment the code below if you want to see the content of every node
        /*
        if (cur_node->content != NULL ){
            printf("  content: %s\n", cur_node->content);
        }
        */
        
        //iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = curNode->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        printElementNames(curNode->children);
    }
}

int parseXMLfile(char* filename) {
    /*
     * initializes the library and checks potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    xmlDoc *file = NULL;
    xmlNode *rootElement = NULL;

    //parse the XML file
    file = xmlReadFile(filename, NULL, 0);
    if (file == NULL) { //error-checking for invalid XML file
        return 1;
    }

    //get the root element node and print element names
    rootElement = xmlDocGetRootElement(file);
    printElementNames(rootElement);

    xmlFreeDoc(file);
    xmlCleanupParser(); //free the global variables that may have been allocated by the parser

    return 0;
}