// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    //on page-load, add data to relevant components of GUI
    $.ajax({
        type: 'get',                        //Request type
        dataType: 'json',                   //Data type - we will use JSON for almost everything 
        url: '/getGPXFilesInUploadsDir',    //The server endpoint we are connecting to
        data: {},                           //Data we are sending to the server, currently an object with no instance vars
        success: function(data) { //the parameter "data" contains the data received from the server
            //write to console to show successful page load
            console.log("Page loaded successfully.");
            //load files on the server into File Log and dropdowns
            let GPXFileNamesArr = data.filenames;
            console.log("Files in uploads/ directory: " + GPXFileNamesArr);
            addUploadFilesToFileLog(GPXFileNamesArr);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });
});

//add files in upload directory to File Log table
function addUploadFilesToFileLog(GPXFileNamesArr) {
    //if no files in uploads/ directory
    if (GPXFileNamesArr.length < 1) {
        //add row to File Log table
        $('#FileLogTable').append(
            "<tr>" + "<td colspan='6'>No files</td>" + "</tr>" //this row's column will span the width of the table
        );
    } else {
        for (filename of GPXFileNamesArr) {
            convertFileToJSON(filename);
        }
    }
}

//convert files to JSON objects and add contents of each file to their appropriate place on app
function convertFileToJSON(filename) {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/GPXFileToJSON',
        data: {
            filename: filename,
            fileDir: "./uploads/" + filename,
        },
        success: function (data) {
            //ensures invalid files do not get displayed
            if (data.doc != null) {
                console.log("GPX file loaded in successfully: " + data.filename);
                addFileToFileLog(data); //add file to File Log Panel table
                $('#GPXViewDropdown').append(new Option(data.filename, data.filename)); //add file to GPX View dropdown
            } else {
                console.log("Error in loading file: " + filename);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
}

//add one file to a row in the File Log table
function addFileToFileLog(GPXobj) { //data passed in is object with instance vars of filename and doc object
    $('#FileLogTable').append(
        "<tr>" + 
            "<td> <a href=\"" + GPXobj.filename + "\" download>" + GPXobj.filename + "</a>" +
            "<td>" + GPXobj.doc.version + "</td>" +
            "<td>" + GPXobj.doc.creator + "</td>" +
            "<td>" + GPXobj.doc.numWaypoints + "</td>" +
            "<td>" + GPXobj.doc.numRoutes + "</td>" +
            "<td>" + GPXobj.doc.numTracks + "</td>" +
        "<tr>"
    );
}

//display GPX file info in GPX View Panel
function displayGPXFileInfo() {
    let chosenFile = $('#GPXViewDropdown option:selected').val();
    $.ajax({
        type: 'get',                    //Request type
        dataType: 'json',               //Data type - we will use JSON for almost everything 
        url: '/getGPXFileComponents',   //The server endpoint we are connecting to
        data: {                         //Data we are sending to the server, currently an object with no instance vars
            fileDir: "./uploads/" + chosenFile,
        },
        success: function (data) { //the parameter "data" contains the data received from the server
            if (data.routesList != null && data.tracksList != null) {
                //write to console to show successful loading of components into table
                console.log("Components added successfully to GPX View Panel for file: " + chosenFile);
                addAllComponentsToGPXViewTable(data);
            } else {
                console.log("Error in loading file into GPX View table: " + chosenFile);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
}

//add all components (routes and tracks) in selected file to GPX View table 
function addAllComponentsToGPXViewTable(GPXobj) {
    //display all route components
    let counter = 1;
    for (let routeJSON of GPXobj.routesList) {
        if (routeJSON.name == "None") routeJSON.name = " ";
        $('#GPXViewTable').append(
            "<tr>" +
                "<td>Route " + counter + "</td>" +          //route number
                "<td>" + routeJSON.name + "</td>" +         //name of route
                "<td>" + routeJSON.numPoints + "</td>" +    //number of points
                "<td>" + routeJSON.len + "m </td>" +        //route length
                "<td>" + routeJSON.loop + "</td>" +         //is loop (true/false)
            "</tr>"
        );
        counter++;
    }
    //display all track components
    counter = 1;
    for (let trackJSON of GPXobj.tracksList) {
        if (trackJSON.name == "None") trackJSON.name = " ";
        $('#GPXViewTable').append(
            "<tr>" +
                "<td>Track " + counter + "</td>" +          //track number
                "<td>" + trackJSON.name + "</td>" +         //name of track
                "<td>" + trackJSON.numPoints + "</td>" +    //number of points
                "<td>" + trackJSON.len + "m </td>" +        //track length
                "<td>" + trackJSON.loop + "</td>" +         //is loop (true/false)
            "</tr>"
        );
        counter++;
    }
}

function AddDummyRow() {
    $('#GPXViewTable').append(
        "<tr>" +
            "<td>Route 1</td>" +
            "<td>Name of Route</td>" +
            "<td>3</td>" +
            "<td>4</td>" +
            "<td>FALSE</td>" +
        "</tr>"
    );
}
