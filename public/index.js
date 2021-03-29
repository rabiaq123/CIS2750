// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    $.ajax({
        type: 'get',                        //Request type
        dataType: 'json',                   //Data type - we will use JSON for almost everything 
        url: '/getGPXFilesInUploadsDir',    //The server endpoint we are connecting to
        data: {},                           //Data we are sending to the server, currently an object with no instance vars
        success: function(data) { //the parameter "data" contains the data received from the server
            //write to console to show successful page load
            console.log("Page loaded successfully.");
            //load files on the server into GPX File Log
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

//convert files in to JSON objects and add contents of each file to their appropriate place on app
function convertFileToJSON(filename) {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/GPXFileToJSON',
        data: {
            filename: "./uploads/" + filename,
        },
        success: function (data) {
            //only display file in File Log and dropdowns if valid GPX
            if (data.doc != null) {
                addFileToFileLog(data);
                //addFileToGPXViewDropdown(data);
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
    console.log("GPX file added to File Log panel successfully: " + GPXobj.filename);

    let file = GPXobj.filename.split("./uploads/");
    $('#FileLogTable').append(
        "<tr>" + 
            "<td> <a href=\"" + GPXobj.filename + "\" download>" + file + "</a>" +
            "<td>" + GPXobj.doc.version + "</td>" +
            "<td>" + GPXobj.doc.creator + "</td>" +
            "<td>" + GPXobj.doc.numWaypoints + "</td>" +
            "<td>" + GPXobj.doc.numRoutes + "</td>" +
            "<td>" + GPXobj.doc.numTracks + "</td>" +
        "<tr>"
    );
}


/*
add every valid GPX file's info to the File Log table AND 
the GPX View Panel's dropdown for file selection
*/




function AddDummyRow() {
    $('#GPXViewTable').append(
        "<tr><td>One</td><td>Two</td><td>Three</td><td>Four</td><td>sdfjsdhfks/td></tr>"
    );
}