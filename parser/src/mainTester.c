#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "LinkedListAPI.h"
#include <string.h>

#include "GPXParser.h"
#include "GPXHelpers.h"

int main(int argc, char **argv)
{

    Route *routePtrFirst = NULL, *routePtrLast = NULL;
    Track *trackPtrFirst = NULL, *trackPtrLast = NULL;

    printf("\n1. Testing 'createValidGPXdoc' with valid arguments...\n");
    GPXdoc *doc = createValidGPXdoc(argv[1], "./testFiles/gpx.xsd");
    if (doc)
        printf("\tCreated valid GPXdoc.\n");
    else
    {
        printf("\tCreated invalid GPXdoc... exiting program.\n");
        return 0;
    }

    routePtrFirst = getFromFront(doc->routes);
    routePtrLast = getFromBack(doc->routes);

    trackPtrFirst = getFromFront(doc->tracks);
    trackPtrLast = getFromBack(doc->tracks);

    printf("\n2. Testing 'validateGPXDoc' with valid arguments...\n");
    bool isValid = validateGPXDoc(doc, "./testFiles/gpx.xsd");
    if (isValid)
        printf("\tValid GPXdoc.\n");
    else
        printf("\tInvalid GPXdoc.\n");

    printf("\n3. Testing 'writeGPXdoc' with valid arguments...\n");
    bool didWrite = writeGPXdoc(doc, "output.gpx");
    if (didWrite)
        printf("\tSuccesfully wrote GPXdoc to fileName output.gpx.\n");
    else
        printf("\tCouldn't write GPXdoc succesfully.\n");

    printf("\n4. Testing 'getRouteLen' with '%s'...\n", routePtrFirst->name);
    float routeLen = getRouteLen(routePtrFirst);
    printf("\t'%s' route len is %f, round10 is %f.\n", routePtrFirst->name, routeLen, round10(routeLen));

    printf("\n5. Testing getTrackLen with %s...\n", trackPtrFirst->name);
    float trackLen = getTrackLen(trackPtrFirst);
    printf("\t'%s' track len is %f. round10 is %f.\n", trackPtrFirst->name, trackLen, round10(trackLen));

    float delta = 11.0;
    printf("\n6. Testing numRoutesWithLength with '%f' and delta '%f'...\n",
           routeLen + delta, delta);
    int numRoutes = numRoutesWithLength(doc, routeLen + delta, delta);
    printf("\t%d routes with length %f +- %f.\n", numRoutes, routeLen, delta);

    delta = 10.0;
    printf("\n7. Testing numRoutesWithLength with '%f' and delta '%f'...\n", routeLen - delta - 1, 10.0);
    numRoutes = numRoutesWithLength(doc, routeLen - delta - 1, delta);
    printf("\t%d routes with length %f +- %f.\n", numRoutes, routeLen, delta + 1);

    delta = 8.0;
    printf("\n8. Testing numTracksWithLength with '%f' and delta '%f'...\n", trackLen + delta, delta);
    int numTracks = numTracksWithLength(doc, trackLen + delta, delta);
    printf("\t%d routes with length %f +- %f.\n", numTracks, trackLen, delta);

    delta = 7.0;
    printf("\n9. Testing numTracksWithLength with '%f' and delta '%f'...\n",
           trackLen - delta - 1, delta);
    numTracks = numTracksWithLength(doc, trackLen - delta - 1, delta);
    printf("\t%d routes with length %f +- %f.\n", numTracks, trackLen, delta);

    printf("\n10. Testing isLoopRoute with '%s' and delta 10...\n", routePtrFirst->name);
    isLoopRoute(routePtrFirst, 10.0)
        ? printf("\t'%s' is loop route.\n", routePtrFirst->name)
        : printf("\t '%s' isn't loop route.\n", routePtrFirst->name);

    printf("\n10.1 Testing isLoopRoute with '%s' and delta 10...\n",
           routePtrLast->name);
    isLoopRoute(routePtrLast, 10.0)
        ? printf("\t'%s' is loop route.\n", routePtrLast->name)
        : printf("\t '%s' isn't loop route.\n", routePtrLast->name);

    printf("\n11. Testing isLoopRoute with '%s' and delta 10000...\n", routePtrFirst->name);
    isLoopRoute(routePtrFirst, 10000.0)
        ? printf("\t'%s' is loop route.\n", routePtrFirst->name)
        : printf("\t '%s' isn't loop route.\n", routePtrFirst->name);

    printf("\n12. Testing isLoopTrack with '%s' and delta 10...\n",
           trackPtrFirst->name);
    isLoopTrack(trackPtrFirst, 10.0)
        ? printf("\t'%s' is loop track.\n", trackPtrFirst->name)
        : printf("\t '%s' isn't loop track.\n", trackPtrFirst->name);

    printf("\n12.1 Testing isLoopTrack with '%s' and delta 10...\n",
           trackPtrLast->name);
    isLoopTrack(trackPtrLast, 10.0)
        ? printf("\t'%s' is loop track.\n", trackPtrLast->name)
        : printf("\t '%s' isn't loop track.\n", trackPtrLast->name);

    printf("\n13. Testing isLoopTrack with '%s' and delta 10000...\n",
           trackPtrFirst->name);
    isLoopTrack(trackPtrFirst, 10000.0)
        ? printf("\t'%s' is loop track.\n", trackPtrFirst->name)
        : printf("\t '%s' isn't loop track.\n", trackPtrFirst->name);

    Waypoint *firstWp = getFromFront(routePtrFirst->waypoints);
    Waypoint *secondWp = getFromBack(routePtrLast->waypoints);

    printf(
        "\n14. Testing getRoutesBetween with sourceLat=%f, sourceLon=%f, "
        "destLat=%f, destLong=%f and delta=%f\n",
        firstWp->latitude, firstWp->longitude, secondWp->latitude,
        secondWp->longitude, 5.0);
    List *routesBetween =
        getRoutesBetween(doc, firstWp->latitude, firstWp->longitude,
                         secondWp->latitude, secondWp->longitude, 5.0);
    routesBetween
        ? printf("\tLength of routes between is %d.\n", routesBetween->length)
        : printf("\tList was NULL.\n");

    printf(
        "\n15. Testing getRoutesBetween with sourceLat=%f, sourceLon=%f, "
        "destLat=%f, destLong=%f and delta=%f\n",
        firstWp->latitude, firstWp->longitude, 44.596100, 123.620570, 5.0);
    List *routesBetween2 = getRoutesBetween(
        doc, firstWp->latitude, firstWp->longitude, 44.596100, -123.620570, 5.0);
    routesBetween2
        ? printf("\tLength of routes between is %d.\n", routesBetween2->length)
        : printf("\tList was NULL.\n");

    TrackSegment *firstTrackSeg = getFromFront(trackPtrFirst->segments);
    TrackSegment *lastTrackSeg = getFromBack(trackPtrLast->segments);

    firstWp = getFromFront(firstTrackSeg->waypoints);
    secondWp = getFromBack(lastTrackSeg->waypoints);

    printf(
        "\n16. Testing getTracksBetween with sourceLat=%f, sourceLon=%f, "
        "destLat=%f, destLong=%f and delta=%f\n",
        firstWp->latitude, firstWp->longitude, secondWp->latitude,
        secondWp->longitude, 5.0);
    List *tracksBetween =
        getTracksBetween(doc, firstWp->latitude, firstWp->longitude,
                         secondWp->latitude, secondWp->longitude, 5.0);
    tracksBetween
        ? printf("\tLength of tracks between is %d.\n", tracksBetween->length)
        : printf("\tList was NULL.\n");

    printf(
        "\n17. Testing getTracksBetween with sourceLat=%f, sourceLon=%f, "
        "destLat=%f, destLong=%f and delta=%f\n",
        firstWp->latitude, firstWp->longitude, 49.598070, -123.620460, 5.0);
    List *tracksBetween2 = getTracksBetween(doc, firstWp->latitude, firstWp->longitude,
                                            49.598070, -123.620460, 5.0);
    tracksBetween2
        ? printf("\tLength of tracks between is %d.\n", tracksBetween2->length)
        : printf("\tList was NULL.\n");

    char *t1ToJson = trackToJSON(trackPtrFirst);
    char *t2ToJson = trackToJSON(trackPtrLast);
    char *r1ToJson = routeToJSON(routePtrFirst);
    char *r2ToJson = routeToJSON(routePtrLast);
    char *rlToJson = routeListToJSON(doc->routes);
    char *tlToJson = trackListToJSON(doc->tracks);
    char *docToJson = GPXtoJSON(doc);

    printf("\n18. Testing trackToJson with '%s'...\n", trackPtrFirst->name);
    printf("\t'%s' is output string.\n", t1ToJson);
    printf("\n19. Testing trackToJson with '%s'...\n", trackPtrLast->name);
    printf("\t'%s' is output string.\n", t2ToJson);
    printf("\n20. Testing trackToJson with '%s'...\n", routePtrFirst->name);
    printf("\t'%s' is output string.\n", r1ToJson);
    printf("\n21. Testing trackToJson with '%s'...\n", routePtrLast->name);
    printf("\t'%s' is output string.\n", r2ToJson);
    printf("\n22. Testing routeListToJson...\n");
    printf("\t'%s' is output string.\n", rlToJson);
    printf("\n23. Testing trackListToJson...\n");
    printf("\t'%s' is output string.\n", tlToJson);
    printf("\n24. Testing docToJson...\n");
    printf("\t'%s' is output string.\n", docToJson);

    free(t1ToJson);
    free(t2ToJson);
    free(r1ToJson);
    free(r2ToJson);
    free(rlToJson);
    free(tlToJson);
    free(docToJson);

    freeList(routesBetween);
    freeList(tracksBetween);
    freeList(routesBetween2);
    freeList(tracksBetween2);

    deleteGPXdoc(doc);

    printf("\n\n");

    return 0;
}