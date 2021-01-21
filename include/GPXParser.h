#ifndef GPX_PARSER_H
#define GPX_PARSER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"

//M_PI is not declared in the C standard, so we declare it manually
//We will need it for A2
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

//Represents a generic GPX element/XML node - i.e. some sort of an additinal piece of data, 
// e.g. comment, elevation, desciption, etc..
typedef struct  {
    //GPXData name.  Must not be an empty string.
	char 	name[256];

    //GPXData value.  We use a C99 flexible array member, which we will discuss in class.
	//Must not be an empty string
	char	value[]; 
} GPXData;

typedef struct {
    //Waypoint name.  Must not be NULL.  May be an empty string.
    char* name;

    //Waypoint longitude.  Must be initialized.
    double longitude;

    //Waypoint latitude.  Must be initialized.
    double latitude;

    //Additional waypoint data - i.e. children of the GPX waypoint other than <name>.  
    //We will assume that all waypoint children have no children of their own
    //This can be elevation, time, etc.. Note that while the element <name> can be a child of the waypoint node,
    //the name already has its own dedicated filed in the Waypoint sruct - so do not place the name in this list
    //All objects in the list will be of type GPXData.  It must not be NULL.  It may be empty.
    List* otherData;
} Waypoint;

typedef struct {
    //Route name.  Must not be NULL.  May be an empty string.
    char* name;

    //Waypoints that make up the route
    //All objects in the list will be of type Waypoint.  It must not be NULL.  It may be empty.
    List* waypoints;

    //Additional route data - i.e. children of the GPX route other than <name>.  
    //We will assume that all waypoint children have no children of their own
    //This can be comment, description, etc.. Note that while the element <name> can be a child of the route node,
    //the name already has its own dedicated filed in the Waypoint sruct - so do not place the name in this list
    //All objects in the list will be of type GPXData.  It must not be NULL.  It may be empty.
    List* otherData;
} Route;

typedef struct {
    //Waypoints that make up the track segment
    //All objects in the list will be of type Waypoint.  It must not be NULL.  It may be empty.
    List* waypoints;
} TrackSegment;

typedef struct {
    //Track name.  Must not be NULL.  May be an empty string.
    char* name;

    //Segments that make up the track
    //All objects in the list will be of type TrackSegment.  It must not be NULL.  It may be empty.
    List* segments;

    //Additional track data - i.e. children of the GPX track other than <name>.  
    //We will assume that all waypoint children have no children of their own
    //This can be comment, description, etc.. Note that while the element <name> can be a child of the track node,
    //the name already has its own dedicated filed in the Waypoint sruct - so do not place the name in this list
    //All objects in the list will be of type GPXData.  It must not be NULL.  It may be empty.
    List* otherData;
} Track;


typedef struct {
    
    //Namespace associated with our GPX doc.  Must not be an empty string. While a real GPX doc might have
    //multiple namespaces associated with it, we will assume there is only one
    char namespace[256];
    
    //GPX version.  Must be initialized.  Will usually be 1.1
    double version;
    
    //GPX creator. Must not be NULL. Must not be an empty string.
    char* creator;
    
    //Waypoints in the GPX file
    //All objects in the list will be of type Waypoint.  It must not be NULL.  It may be empty.
    List* waypoints;

    //Routes in the GPX file
    //All objects in the list will be of type Route.  It must not be NULL.  It may be empty.
    List* routes;
    
    //Tracks in the GPX file
    //All objects in the list will be of type Track.  It must not be NULL.  It may be empty.
    List* tracks;
} GPXdoc;



//A1

/* Public API - main */

/** Function to create an GPX object based on the contents of an GPX file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createGPXdoc(char* fileName);

/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc);

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/
void deleteGPXdoc(GPXdoc* doc);

/* For the five "get..." functions below, return the count of specified entities from the file.  
They all share the same format and only differ in what they have to count.
 
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
 */


//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc);

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc);

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc);

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc);

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc);

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name);
// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name);
// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name);


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteGpxData( void* data);
char* gpxDataToString( void* data);
int compareGpxData(const void *first, const void *second);

void deleteWaypoint(void* data);
char* waypointToString( void* data);
int compareWaypoints(const void *first, const void *second);

void deleteRoute(void* data);
char* routeToString(void* data);
int compareRoutes(const void *first, const void *second);

void deleteTrackSegment(void* data);
char* trackSegmentToString(void* data);
int compareTrackSegments(const void *first, const void *second);

void deleteTrack(void* data);
char* trackToString(void* data);
int compareTracks(const void *first, const void *second);


#endif