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
            //console.log("Files in uploads/ directory: " + GPXFileNamesArr);
            addUploadFilesToFileLog(GPXFileNamesArr);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    //GPX View table should be empty on page load
    $('#GPXViewTable').append("<tr>" + "<td colspan='8'>No file selected</td>" + "</tr>");
});

//add files in upload directory to File Log table
function addUploadFilesToFileLog(GPXFileNamesArr) {
    //if no files in uploads/ directory
    if (GPXFileNamesArr.length < 1) {
        $('#FileLogTable').append("<tr>" + "<td colspan='6'>No files</td>" + "</tr>"); //add row to File Log table
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
                if (chosenFile != "") console.log("Error in loading file into GPX View table: " + chosenFile);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });

    //if user selects default (no file) option again, display No file selected message
    if (chosenFile == "") {
        $("#GPXViewTable tbody tr").remove(); //clear <tbody/> before adding new rows for new file
        $('#GPXViewTable').append("<tr>" + "<td colspan='8'>No file selected</td>" + "</tr>");
    }
}

//add all components (routes and tracks) in selected file to GPX View table 
function addAllComponentsToGPXViewTable(GPXobj) {
    //clear (all table rows) within <tbody/> before adding new rows for new file
    $("#GPXViewTable tbody tr").remove();

    //display all route components
    let counter = 1;
    for (let route of GPXobj.routesList) {
        if (route.name == "None") route.name = "";
        let otherDataRte = JSON.stringify(route.otherData);
        //add row to table
        $('#GPXViewTable').append(
            "<tr>" +
                "<td>Route " + counter + "</td>" +      //route number
                "<td>" + route.name + "</td>" +         //name of route
                "<td>" + route.numPoints + "</td>" +    //number of points
                "<td>" + route.len + "m </td>" +        //route length
                "<td>" + route.loop + "</td>" +         //is loop (true/false)
                "<td>" +                                //rename component form
                    "<form ref='renameForm' id='renameForm'>" +
                        "<div class='form-group'>" +
                            "<input type='text' class='form-control' id='entryBoxR" + counter + "' value='" + route.name + "' " +
                              "placeholder='" + "Rename component here..." + "'>" +
                        "</div>" +
                    "</form>" +
                "</td>" +
                "<td>" +
                    "<form ref='renameForm' id='renameForm'>" +
                        "<div class='form-group'>" +
                        "<input type='submit' class='btn btn-secondary' value='Submit'>" +
                        "</div>" +
                    "</form>" +
                "</td>" +
                "<td>" +
                    //view other data radio button and display alert if selected
                    "<input type='radio' name='optInfo' onclick='displayAlert(" + otherDataRte + ")'>" +
                "</td>" +
            "</tr>"
        );
        console.log("Route: ", route);
        counter++;
    }

    //display all track components
    counter = 1;
    for (let track of GPXobj.tracksList) {
        if (track.name == "None") track.name = "";
        let otherDataTrk = JSON.stringify(track.otherData);
        //add row to table
        $('#GPXViewTable').append(
            "<tr>" +
                "<td>Track " + counter + "</td>" +      //track number
                "<td>" + track.name + "</td>" +         //name of track
                "<td>" + track.numPoints + "</td>" +    //number of points
                "<td>" + track.len + "m </td>" +        //track length
                "<td>" + track.loop + "</td>" +         //is loop (true/false)
                "<td>" +                                //rename component form
                    "<form ref='renameForm' id='renameForm'>" +
                        "<div class='form-group'>" +
                            "<input type='text' class='form-control' id='entryBoxT" + counter + "' value='" + track.name + "' " +
                              "placeholder='" + "Rename component here..." + "'>" +
                        "</div>" +
                    "</form>" +
                "</td>" +
                "<td>" +
                   "<form ref='renameForm' id='renameForm' onsubmit='return false'>" +
                       "<div class='form-group'>" +
                           "<input type='submit' class='btn btn-secondary' value='Submit' " +
                             "onclick='updateComponent(" + 2 + ", " + counter + ")'>" +
                       "</div>" +
                   "</form>" +
                "</td>" +
                "<td>" +
                    //view other data radio button and display alert if selected
                    "<input type='radio' name='optInfo' onclick='displayAlert(" + otherDataTrk + ")'>" +
                "</td>" +
            "</tr>"
        );
        console.log("Track: ", track);
        counter++;
    }
}

//connect to server endpoint to update component (route/track) name
//flag is component type (1 = Route), counter is component number, and name is the new component name
function updateComponent(flag, counter) {
    let name;
    if (flag == 1) name = $('#entryBox' + 'R' + counter).val();
    else name = $('#entryBox' + 'T' + counter).val();
    let chosenFile = $('#GPXViewDropdown option:selected').val();
    let index = counter - 1;
    console.log("UPDATE Component", flag, " ", counter, " ", name, " ");

    $.ajax({
        type: 'get',                   //Request type
        dataType: 'json',               //Data type - we will use JSON for almost everything 
        url: '/updateComponent',        //The server endpoint we are connecting to
        data: {                         //Data we are sending to the server, currently an object with no instance vars
            fileDir: "./uploads/" + chosenFile,
            componentFlag: flag,
            index: index,
            name: name
        },
        success: function (data) { //the parameter "data" contains the data received from the server
            if (data.isUpdated == true) {
                //reload page to show changes in file contents
//                location.reload();
                console.log("Successfully updated component name and saved to disk.");
            } else {
                console.log("Error updating component name to: " + chosenFile);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
}

//display alert message containing all objects in JSON array (other data objects in route/track)
function displayAlert(alertMessageObject) {
    let displayString = "";

    //append name and value attributes for each otherData item to display string
    let counter = 1;
    if (alertMessageObject.length != 0) { //check for empty otherData list for route/track
        for (let alert of alertMessageObject) {
            if (alert.name != undefined && alert.value != undefined) { //key was not assigned a value
                displayString = displayString + "Component detail element " + counter + ":\n";
                displayString = displayString + "name: " + alert.name + "\nvalue: " + alert.value + "\n";
                if (counter < alertMessageObject.length) displayString = displayString + "\n"; //don't add \n for last object
            }
            counter++;
        }
    } else {
        displayString = "Component contains no other data."
    }

    alert(displayString);
}
