// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    //on page-load, add data to relevant components of GUI
    $.ajax({
        type: 'get',                        //Request type
        dataType: 'json',                   //Data type - we will use JSON for almost everything 
        url: '/getGPXFilesInUploadsDir',    //The server endpoint we are connecting to
        data: {},                           //Data we are sending to the server, currently an object with no instance vars
        success: function(data) { //the parameter "data" contains the data received from the server
            console.log("Page loaded successfully.");
            //load files on the server into File Log and dropdowns
            let GPXFileNamesArr = data.filenames;
            addUploadFilesToFileLog(GPXFileNamesArr);
        },
        fail: function(error) {
            console.log(error); 
        }
    });

    //clear all text boxes on page load
    $("input[type=text]").val('');

    //GPX View table should be empty on page load
    $('#GPXViewTable').append("<tr>" + "<td colspan='8'>No file selected</td>" + "</tr>");

    //create event listener for 'Create GPX' form submit button
    document.getElementById('createGPXButton').onclick = function(e) {
        e.preventDefault();
        let filename = $('#entryBoxGPXName').val();
        let creator = $('#entryBoxGPXCreator').val();
        createNewGPX(filename, creator);
    };

    //create event listener for 'Add Route' form submit button
    document.getElementById('addRouteButton').onclick = function(e) {
        e.preventDefault();
        let wpt1Lat = $('#entryBoxRtept1Lat').val();
        let wpt1Lon = $('#entryBoxRtept1Lon').val();
        let wpt2Lat = $('#entryBoxRtept2Lat').val();
        let wpt2Lon = $('#entryBoxRtept2Lon').val();
        addRouteToGPX(wpt1Lat, wpt1Lon, wpt2Lat, wpt2Lon);
    }

    //create event listener for 'Database Login' form submit button
    document.getElementById('loginButton').onclick = function(e) {
        e.preventDefault();
        let uname = $('#entryBoxDBUser').val();
        let pass = $('#entryBoxDBPass').val();
        let name = $('#entryBoxDBName').val();
        login(uname, pass, name);
    }

    //create event listener for 'Store All Files' button
    document.getElementById('storeAllFilesButton').onclick = function() {
        storeInDB();
    }

    //create event listener for 'Track Route Updates' button
    document.getElementById('trackRouteUpdatesButton').onclick = function() {
        trackRouteUpdates();
    }
    
    //create event listener for 'Clear Database' button
    document.getElementById('clearDataButton').onclick = function() {
        clearDB();
        setTimeout(function () { location.reload() }, 2000); //reload page (with 3s delay) to show logout console message
    }

    //create event listener for 'Display DB Status' button 
    document.getElementById('displayStatusButton').onclick = function() {
        displayDBStatus();
    }

    //create event listener for 'Logout' button
    document.getElementById('logoutButton').onclick = function() {
        logout();
        setTimeout(function () { location.reload() }, 2000); //reload page (with 3s delay) to show logout console message
    }

    //disable DB commands upon logout
    $('#clearDataButton').prop('disabled', true);
    $('#displayStatusButton').prop('disabled', true);
    $('#storeAllFilesButton').prop('disabled', true);
    $('#DBTrackRouteDropdown').prop('disabled', true);
    $('#trackRouteUpdatesButton').prop('disabled', true);
    $('#logoutButton').prop('disabled', true);
    //disable 'Execute Query' buttons
    $("#DBQueryDropdown").prop("disabled", true);
    
    //hide all query panels on page load
    $('#Q1Panel').hide();
    $('#Q2Panel').hide();
    $('#Q3Panel').hide();
    $('#Q4Panel').hide();
    $('#Q5Panel').hide();

    //create event listener for 'Execute' button for Query 1
    document.getElementById('executeQ1Button').onclick = function () {
        executeQuery1();
    }

    //create event listener for 'Execute' button for Query 2
    document.getElementById('executeQ2Button').onclick = function () {
        executeQuery2();
    }

    //create event listener for 'Execute' button for Query 3
    document.getElementById('executeQ3Button').onclick = function () {
        executeQuery3();
    }


});


//add files in upload directory to File Log table
function addUploadFilesToFileLog(GPXFileNamesArr) {
    //if no files in uploads/ directory
    if (GPXFileNamesArr.length < 1) {
        $('#FileLogTable').append("<tr>" + "<td colspan='6'>No files</td>" + "</tr>"); //add row to File Log table
    } else {
        for (let filename of GPXFileNamesArr) {
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
                //add file to dropdowns
                $('#GPXViewDropdown').append(new Option(data.filename, data.filename));
                $('#AddRouteDropdown').append(new Option(data.filename, data.filename));
                $('#DBTrackRouteDropdown').append(new Option(data.filename, data.filename));
            } else {
                console.log("Error in loading file: " + filename);
            }
        },
        fail: function (error) {
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
                console.log("Components added successfully to GPX View Panel for file: " + chosenFile);
                addAllComponentsToGPXViewTable(data);
            } else {
                if (chosenFile != "") console.log("Error in loading file into GPX View table: " + chosenFile);
            }
        },
        fail: function (error) {
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

    if (GPXobj.routesList.length == 0 && GPXobj.tracksList.length == 0) {
        $('#GPXViewTable').append("<tr>" + "<td colspan='8'>No routes/tracks to be displayed</td>" + "</tr>");
        return;
    }

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
                    "<form ref='renameForm' id='renameForm' onsubmit='return false'>" +
                        "<div class='form-group'>" +
                            "<input type='submit' class='btn btn-secondary' value='Submit' " +
                                "onclick='updateComponent(" + 1 + ", " + counter + ")'>" +
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


//connect to server endpoint to update component (route/track) name
//flag is component type (1 = Route), counter is component number, and name is the new component name
function updateComponent(flag, counter) {
    let name;
    if (flag == 1) name = $('#entryBox' + 'R' + counter).val();
    else name = $('#entryBox' + 'T' + counter).val();
    let chosenFile = $('#GPXViewDropdown option:selected').val();
    let index = counter - 1;

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
                console.log("Successfully updated component name and saved to disk.");
                setTimeout(function() { location.reload() }, 2000); //reload page (with 3s delay) to show changes in file contents
            } else {
                console.log("Error occurred while attempting to rename component to: " + name);
            }
        },
        fail: function (error) {
            // Non-200 return, do something with error
            console.log(error);
        }
    });
}


//send user input for new GPX file to server
function createNewGPX(filename, creator) {
    $.ajax({
        type: 'get',                    //Request type
        dataType: 'json',               //Data type - we will use JSON for almost everything 
        url: '/createNewGPX',           //The server endpoint we are connecting to
        data: {                         //Data we are sending to the server, currently an object with no instance vars
            fileDir: "./uploads/" + filename,
            creator: creator
        },
        success: function (data) { //the parameter "data" contains the data received from the server
            if (data.isCreated == true) {
                console.log("Successfully created GPX file '" + filename + "' and saved to disk.");
                setTimeout(function() { location.reload() }, 2000); //reload page (with 3s delay) to show changes in file contents
            } else {
                if (filename.length != 0 && creator.length != 0) {
                    alert("Error occurred during GPX file creation.\n" +
                    "File '" + filename + "' may already exist, or it may not match GPX specifications.");
                } else {
                    alert("Please fill in both fields.");
                }
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//add route to GPX file
function addRouteToGPX(wpt1Lat, wpt1Lon, wpt2Lat, wpt2Lon) {
    if (wpt1Lat < -90 || wpt1Lat > 90 || wpt2Lat < -90 || wpt2Lat > 90 ||
        wpt1Lon < -180 || wpt1Lon >  180 || wpt2Lon < -180 || wpt2Lon > 180) {
        alert("Invalid Input: Latitude must be within -90 to 90 and\nlongitude must be within -180 to 180.");
        return;
    }

    let chosenFile = $('#AddRouteDropdown option:selected').val();
    $.ajax({
        type: 'get',                    //Request type
        dataType: 'json',               //Data type - we will use JSON for almost everything 
        url: '/addRouteToGPX',          //The server endpoint we are connecting to
        data: {                         //Data we are sending to the server, currently an object with no instance vars
            fileDir: "./uploads/" + chosenFile,
            wpt1Lat: wpt1Lat,
            wpt1Lon: wpt1Lon,
            wpt2Lat: wpt2Lat,
            wpt2Lon: wpt2Lon
        },
        success: function (data) { //the parameter "data" contains the data received from the server
            if (data.isAdded == 1) {
                console.log("Route successfully added to GPX file: " + chosenFile);
                setTimeout(function() { location.reload() }, 2000); //reload page (with 3s delay) to show changes in file contents
            } else {
                console.log("Error in adding new route to GPX file: " + chosenFile);
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });

    //if user selects default (no file) option again, display No file selected message
    if (chosenFile == "") {
        $("#GPXViewTable tbody tr").remove(); //clear <tbody/> before adding new rows for new file
        $('#GPXViewTable').append("<tr>" + "<td colspan='8'>No file selected</td>" + "</tr>");
    }
}


/*
A4 DB Functionality
MYSQL Database Connection
*/


//display database status 
function displayDBStatus() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/displayDBStatus',
        data: {},
        success: function (data) {
            if (data.status == 0) alert("Database has " + data.numFiles + " files, " + data.numRoutes + " routes, and " + data.numPoints + " points.");
            else alert("Could not display database status.");
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//allow user to login to database
function login(uname, pass, name) {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/login',
        data: {
            uname: uname,
            pass: pass,
            name: name
        },
        success: function (data) {
            //ensures invalid files do not get displayed
            if (data.loginStatus == true) {
                console.log("Successfully created connection to database:", name);
                displayDBStatus();
                //make clickable upon login, as DB tables are also created by then
                $('#clearDataButton').prop('disabled', false);
                $('#displayStatusButton').prop('disabled', false);
                $('#logoutButton').prop('disabled', false);
                //enable UI elements for updating DB, if there are files on the server
                let numFiles = $('#GPXViewDropdown').children('option').length;
                if (numFiles > 0) { //this functionality needs files to be on the server
                    $('#storeAllFilesButton').prop('disabled', false);
                    $("#DBTrackRouteDropdown").prop("disabled", false);
                }
                //allow execute query commands if DB is already filled
                if (data.filledRows > 0) $("#DBQueryDropdown").prop("disabled", false);
            } else {
                alert("Invalid credentials. Please try again.");
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//store all files in DB
/*
get all files on the server (in the uploads/ directory)
for each filename in the array of filenames, create a GPXdoc struct 
insert the filename and its corresponding GPXdoc struct's related info (filename, version, creator) into table
*/
function storeInDB() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/storeInDB',
        data: {},
        success: function (data) {
            if (data.isStored) {
                console.log("Successfully stored all files in database!");
                displayDBStatus();
                $("#DBQueryDropdown").prop("disabled", false);
                //add files and routes in server to respective dropdowns
                for (let file of data.filesStored) $('#Q2FileDropdown').append(new Option(file.file_name, file.file_name));
                for (let route of data.routesStored) {
                    let routeName = route.route_name;
                    if (!routeName) routeName = "[no named route]";
                    $('#Q3RouteDropdown').append(new Option(routeName, route.route_id));
                }
            }
            else console.log("Error in storing files - there may be no files on the server.");
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//clear database
function clearDB() {
    $.ajax({
        type: 'post',
        dataType: 'json',
        url: '/clearDB',
        data: {},
        success: function (data) {
            if (data.isCleared == true) {
                console.log("Successfully cleared database!");
                displayDBStatus();
                $('#trackRouteUpdatesButton').prop('disabled', true);
            }
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//enable track route updates button
function enableTrackRouteUpdates() {
    if ($('#DBTrackRouteDropdown option:selected').val() == "") {
        $('#trackRouteUpdatesButton').prop('disabled', true);
    }
    else {
        //enable 'Track Route Updates' button if DB is storing files
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/displayDBStatus',
            data: {},
            success: function (data) {
                if (data.numFiles > 0) $('#trackRouteUpdatesButton').prop('disabled', false);
            },
            fail: function (error) {
                console.log(error);
            }
        });
    }
}


//track route updates
function trackRouteUpdates() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/updateDB',
        data: {},
        success: function (data) {
            if (data.isStored) {
                console.log("Successfully updated database!");
                displayDBStatus();
                $("#DBQueryDropdown").prop("disabled", false);
                //add files and routes in server to respective dropdowns
                let i = 0;
                for (let file of data.filesStored) $('#Q2FileDropdown').append(new Option(file.file_name, file.file_name));
                for (let route of data.routesStored) {
                    let routeName = route.route_name;
                    if (!routeName) routeName = "[no named route # " + i + "]";
                    $('#Q3RouteDropdown').append(new Option(routeName, route.route_id));
                    i++;
                }
            }
            else console.log("Error in storing files - there may be no files on the server.");
        },
        fail: function (error) {
            console.log(error);
        }
    });
}


//show query panel of query selected from Query Panel dropdown 
function showQuery() {
    let chosenQuery = $('#DBQueryDropdown option:selected').val();
    
    $('#Q1Panel').hide();
    $('#Q2Panel').hide();
    $('#Q3Panel').hide();
    $('#Q4Panel').hide();
    $('#Q5Panel').hide();
    $("#Q1Table tbody tr").remove();
    $("#Q2Table tbody tr").remove();
    $("#Q3Table tbody tr").remove();
    $("#Q4Table tbody tr").remove();
    $("#Q5Table tbody tr").remove();
    
    if (chosenQuery == "Q1Option") {
        $('#Q1Panel').show();
        enableQuery('#executeQ1Button', '#Q1Table');
    }
    else if (chosenQuery == "Q2Option") {
        $('#Q2Panel').show();
        enableQuery('#executeQ2Button', '#Q2Table');
    }
    else if (chosenQuery == "Q3Option") {
        $('#Q3Panel').show();
        enableQuery('#executeQ3Button', '#Q3Table');
    }
    else if (chosenQuery == "Q4Option") {
        $('#Q4Panel').show();
        enableQuery('#executeQ4Button', '#Q4Table');
    }
    else if (chosenQuery == "Q5Option") {
        $('#Q5Panel').show();
        enableQuery('#executeQ5Button', '#Q5Table');
    } 
}


//enable selected query - show panel, enable 'Execute' button if server has files
function enableQuery(queryButton, queryTable) {
    $(queryButton).prop('disabled', true);

    let numDBFiles = $('#Q2FileDropdown').children('option').length;
    if (numDBFiles > 0) $(queryButton).prop('disabled', false);
    $(queryTable).append("<tr>" + "<td colspan='6'>Nothing to display</td>" + "</tr>"); //until 'Execute' button is clicked
}


//execute Query 1
function executeQuery1() {
    let sortChoice = 0; //1->name, 2->length, 0->neither

    if ($('#Q1NameOption').is(':checked')) sortChoice = 1;
    else if ($('#Q1LengthOption').is(':checked')) sortChoice = 2;

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/query1',
        data: {
            sort: sortChoice
        },
        success: function (data) {           
            //clear (all table rows) within <tbody/> before adding new rows for new Query
            $("#Q1Table tbody tr").remove();

            //display sorted results in table
            let routeName;
            for (let row of data.sortedRoutes) {
                routeName = row.route_name;
                if (!routeName) routeName = "[no name for route " + row.route_id + "]";
                $('#Q1Table').append("<tr>" + 
                    "<td>" + row.route_id + "</td>" +
                    "<td>" + routeName + "</td>" +
                    "<td>" + row.route_len + "</td>" +
                    "</tr>");
            }
            console.log("Successfully executed Query 1!");
        },
        fail: function (error) {
            console.log(error);
        }
    })
}


//execute Query 2
function executeQuery2() {
    let sortChoice = 0; //1->name, 2->length, 0->neither
    let filename = $('#Q2FileDropdown option:selected').val();

    if ($('#Q2NameOption').is(':checked')) sortChoice = 1;
    else if ($('#Q2LengthOption').is(':checked')) sortChoice = 2;

    if (filename != "") {
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/query2',
            data: {
                sort: sortChoice,
                file: filename
            },
            success: function (data) {
                //clear (all table rows) within <tbody/> before adding new rows for new Query
                $("#Q2Table tbody tr").remove();

                //display sorted routes in table
                let routeName;
                for (let row of data.sortedRoutes) {
                    routeName = row.route_name;
                    if (!routeName) routeName = "[no name for route " + row.route_id + "]";
                    $('#Q2Table').append("<tr>" +
                        "<td>" + row.route_id + "</td>" +
                        "<td>" + routeName + "</td>" +
                        "<td>" + row.route_len + "</td>" +
                        "</tr>");
                }
                if (data.sortedRoutes.length == 0) $('#Q2Table').append("<tr>" + "<td colspan='6'>No routes</td>" + "</tr>");
                console.log("Successfully executed Query 2!");
            },
            fail: function (error) {
                console.log(error);
            }
        })
    }
}


//execute Query 3
function executeQuery3() {
    let routeID = $('#Q3RouteDropdown option:selected').val();

    //clear (all table rows) within <tbody/> before adding new rows for new Query
    $("#Q3Table tbody tr").remove();

    if (routeID == "") {
        $('#Q3Table').append("<tr>" + "<td colspan='6'>Nothing to display</td>" + "</tr>");
    } else {
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/query3',
            data: {
                route: routeID
            },
            success: function (data) {    
                //display sorted points in table
                let pointName;
                for (let row of data.sortedPoints) {
                    pointName = row.point_name;
                    if (!pointName) pointName = "[no name for point " + row.point_id + "]";
                    $('#Q3Table').append("<tr>" +
                        "<td>" + row.point_id + "</td>" +
                        "<td>" + row.point_index + "</td>" +
                        "<td>" + pointName + "</td>" +
                        "<td>" + row.latitude + "</td>" +
                        "<td>" + row.longitude + "</td>" +
                        "</tr>");
                }
                if (data.sortedPoints.length == 0) $('#Q3Table').append("<tr>" + "<td colspan='6'>No points</td>" + "</tr>");
    
                console.log("Successfully executed Query 3!");
            },
            fail: function (error) {
                console.log(error);
            }
        })
    }
}


//execute Query 4
function executeQuery4() {
}


//execute Query 5
function executeQuery5() {
}


//log user out of database
function logout() {
    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/logout',
        data: {},
        success: function (data) {
            if (data.isLoggedOut == true) console.log("Successfully logged out of database!");
        },
        fail: function (error) {
            console.log(error);
        }
    });
}