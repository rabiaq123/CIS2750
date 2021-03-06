'use strict'

// C library API - ffi-napi is a Node.js addon for loading and calling dynamic libraries
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
    res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
    //Feel free to change the contents of style.css to prettify your Web app
    res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
    fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
        const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
        res.contentType('application/javascript');
        res.send(minimizedContents._obfuscatedCode);
    });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
    if(!req.files) {
        return res.status(400).send('No files were uploaded.');
    }
    
    let uploadFile = req.files.uploadFile;
    
    // Use the mv() method to place the file somewhere on your server
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
        if(err) {
            return res.status(500).send(err);
        }
        res.redirect('/');
    });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
    fs.stat('uploads/' + req.params.name, function(err, stat) {
        if(err == null) {
            res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
        } else {
            console.log('Error in file downloading route: '+err);
            res.send('');
        }
    });
});

//******************** Your code goes here ******************** 

let GPXParserLib = ffi.Library('./libgpxparser', {
    //return type first, argument list second
    //if void input type, leave argument list empty
    //listing all wrappers
    'GPXFileToJSON': ['string', ['string']],
    'getAllRouteComponentsJSON': ['string', ['string']],
    'getAllTrackComponentsJSON': ['string', ['string']],
    'updateComponentName': ['bool', ['string', 'int', 'int', 'string']],
    'createNewGPX': ['bool', ['string', 'string', 'int']],
    'addRouteToGPXWrapper': ['bool', ['string', 'double', 'double', 'double', 'double']]
});

/*
Routes are individual functions that correspond to endpoints.
The below functions are routes.
In these callback functions, req is the data we're passing into this function and 
res is the data we're sending back.
*/


//send array of filenames in uploads directory
app.get('/getGPXFilesInUploadsDir', function (req, res) {
    const dir = path.join(__dirname, 'uploads'); //directory path

    //read directory
    fs.readdir(dir, function (error, files) {
        //handle error in reading
        if (error) return console.log('Error in reading directory: ' + error);
        res.send({
            filenames: files
        });
    });
});


//return JSON object of GPX file's contents
app.get('/GPXFileToJSON', function (req, res) {
    let file = req.query.fileDir; //grab data passed to server endpoint with 'request'
    let fileToJSON = GPXParserLib.GPXFileToJSON(file);

    //error-handing for invalid GPX file
    if (fileToJSON == null) {
        res.send({ //object being sent back with 'response'
            doc: null
        });
    } else {
        let GPXdoc = JSON.parse(fileToJSON); //convert JSON string to object
        res.send({
            doc: GPXdoc,
            filename: req.query.filename
        });
    }
});


//get components of file in JSON string format
app.get('/getGPXFileComponents', function (req, res) {
    let file = req.query.fileDir;
    let routesStringJSON = GPXParserLib.getAllRouteComponentsJSON(file);
    let tracksStringJSON = GPXParserLib.getAllTrackComponentsJSON(file);

    //error-handling for invalid GPX file (not a well-formed XML or does not follow GPX specifications)
    if (routesStringJSON == null || tracksStringJSON == null) {
        res.send({
            routesList: null,
            tracksList: null
        });
    } else {
        //convert JSON strings to objects
        let routesListJSON = JSON.parse(routesStringJSON);
        let tracksListJSON = JSON.parse(tracksStringJSON);    
        res.send({
            routesList: routesListJSON,
            tracksList: tracksListJSON,
        });
    }
});


//update route/track name in file and in GPX View table
app.get('/updateComponent', function (req, res) {
    let file = req.query.fileDir;
    let compFlag = req.query.componentFlag;
    let index = req.query.index;
    let newName = req.query.name;
    let nameChanged = GPXParserLib.updateComponentName(file, compFlag, index, newName);

    //error-handling for error in updating component name 
    if (nameChanged == false) {
        res.send({
            isUpdated: false
        });
    } else {
        res.send({
            isUpdated: true
        })
    }
});


//create GPX file
app.get('/createNewGPX', function (req, res) {
    let fileDir = req.query.fileDir;
    let creator = req.query.creator;
    let creatorLen = creator.length;
    let isCreated = GPXParserLib.createNewGPX(fileDir, creator, creatorLen);

    //return whether GPX file was successfully created
    res.send({
        isCreated: isCreated
    });
});


//add route to pre-existing GPX file
app.get('/addRouteToGPX', function (req, res) {
    let fileDir = req.query.fileDir;
    let wpt1Lat = req.query.wpt1Lat;
    let wpt1Lon = req.query.wpt1Lon;
    let wpt2Lat = req.query.wpt2Lat;
    let wpt2Lon = req.query.wpt2Lon;
    let isAdded = GPXParserLib.addRouteToGPXWrapper(fileDir, wpt1Lat, wpt1Lon, wpt2Lat, wpt2Lon);

    //return whether GPX file was successfully created
    res.send({
        isAdded: isAdded
    });

});


/* 
A4 DB Functionality
MYSQL Database Connection 
*/
const mysql = require('mysql2/promise');

//these variables will be used throughout the code requiring DB connection
var h = 'dursley.socs.uoguelph.ca';
var u;
var p;
var db;
var connection;


//login to database and save user's login info
app.get('/login', async function (req, res) {
    let uname = req.query.uname;
    let pass = req.query.pass;
    let name = req.query.name;
    let connected = true;
    let filledRows;

    try {
        /*
        'await' keyword ensures the next statement runs only after 
        createConnection() has returned the connection object
        */
        connection = await mysql.createConnection({
            host: h,
            user: uname,
            password: pass,
            database: name
        });
        //if login successful, save login info to use it throughout without prompting user for multiple logins
        u = uname;
        p = pass;
        db = name;

        /*
        create DB tables (if they do not already exist) when program executes 
        every 'execute' statement must be placed in a try block, as it may throw an error 
        */
        //create FILE, ROUTE, and POINT table on database
        await connection.execute("CREATE TABLE IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL, PRIMARY KEY(gpx_id) )");
        await connection.execute("CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, PRIMARY KEY(route_id), FOREIGN KEY(gpx_id) REFERENCES FILE(gpx_id) ON DELETE CASCADE )");
        await connection.execute("CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, PRIMARY KEY(point_id), FOREIGN KEY(route_id) REFERENCES ROUTE(route_id) ON DELETE CASCADE )");
        let [rows] = await connection.execute("SELECT * FROM FILE");
        filledRows = rows.length;
    } catch (e) { //error handling for creating database connection
        connected = false;
        console.log(e);
    } finally {
        if (connection && connection.end) connection.end();
    }

    //return whether login was successful
    res.send({
        loginStatus: connected,
        filledRows: filledRows
    });
});


//from server, get each file's version and creator, and store in database along with filename
app.get('/storeInDB', async function (req, res) {
    let fileDir, fileToJSON;
    let filenames = [];
    let routesToJSON, routesParsed;
    let GPXdoc;
    let gpxInsertID, routeInsertID;
    let record; //row to be added to table
    let filesStored = [], routesStored = [], pointsStored = [];
    let isStored = false;

    //get files from server
    const dir = path.join(__dirname, 'uploads'); //directory path
    fs.readdir(dir, async function (error, files) {
        if (error) throw (error);
        files.forEach(async function (file) {
            filenames.push(file);
        });
    });

    try {
        connection = await mysql.createConnection({
            host: h,
            user: u,
            password: p,
            database: db
        });

        let i = 0;
        for (let file of filenames) {
            if (i == 0) {
                await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 1");
                await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 1");
                await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 1");
            }
            i++;

            //check if file is already in FILE table 
            let [duplicatesFILE] = await connection.execute("SELECT * FROM FILE WHERE FILE.file_name='" + file + "'");
            if (duplicatesFILE && duplicatesFILE.length > 0) {
                console.log("File", file, "already exists in database");
                continue;
            }
            fileDir = "./uploads/" + file;

            //obtain version and creator of each file
            fileToJSON = GPXParserLib.GPXFileToJSON(fileDir);
            GPXdoc = JSON.parse(fileToJSON);
            if (GPXdoc == null) continue; //invalid file

            //insert file into FILE table - auto-incremented gpx_id can be given value of 'null'
            record = "INSERT INTO FILE VALUES (null,'" + file + "'," + GPXdoc.version + ",'" + GPXdoc.creator + "')";
            let [rowsFILE, fieldsFILE] = await connection.execute(record);
            gpxInsertID = rowsFILE.insertId; //id of last insert (value of gpx_id) in FILE table

            //obtain relevant information regarding each of file's routes and each route's waypoints
            routesToJSON = GPXParserLib.getAllRouteComponentsJSON(fileDir);
            routesParsed = JSON.parse(routesToJSON);
            for (let route of routesParsed) {
                //insert route into ROUTE table 
                if (route.name == "None") record = "INSERT INTO ROUTE VALUES (null, null," + route.len + "," + gpxInsertID + ")";
                else record = "INSERT INTO ROUTE VALUES (null,'" + route.name + "'," + route.len + "," + gpxInsertID + ")";
                let [rowsROUTE, fieldsROUTE] = await connection.execute(record);
                routeInsertID = rowsROUTE.insertId; //id of last insert (value of route_id) in ROUTE table

                //insert point into POINT table - auto-incremented route_id can be given value of 'null'
                for (let waypoint of route.waypoints) {
                    if (waypoint.name == "None") {
                        record = "INSERT INTO POINT VALUES (null," + waypoint.index + "," +
                            waypoint.latitude + "," + waypoint.longitude + ", null," + routeInsertID + ")";
                    } else {
                        record = "INSERT INTO POINT VALUES (null," + waypoint.index + "," +
                            waypoint.latitude + "," + waypoint.longitude + ",'" + waypoint.name + "'," + routeInsertID + ")";
                    }
                    let [rowsPOINT, fieldsPOINT] = await connection.execute(record);
                }
            }
        }
        let [allFILE] = await connection.execute("SELECT * FROM FILE");
        let [allROUTE] = await connection.execute("SELECT * FROM ROUTE");
        let [allPOINT] = await connection.execute("SELECT * FROM POINT");
        filesStored = allFILE;
        routesStored = allROUTE;
        pointsStored = allPOINT;
        if (filesStored.length > 0) isStored =  true;
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        isStored: isStored,
        filesStored: filesStored,
        routesStored: routesStored,
        pointsStored: pointsStored
    });
});


//display count of files, routes, and points in their respective DB tables
app.get('/displayDBStatus', async function (req,res) {
    let numRowsFILE, numRowsROUTE, numRowsPOINT;
    let status = 0;

    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });
        //count num rows in each table
        let [fRows] = await connection.execute("SELECT * FROM FILE");
        numRowsFILE = fRows.length;
        let [rRows] = await connection.execute("SELECT * FROM ROUTE");
        numRowsROUTE = rRows.length;
        let [pRows] = await connection.execute("SELECT * FROM POINT");
        numRowsPOINT = pRows.length;
    } catch (err) {
        console.log(err);
        status = -1;
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        status: status,
        numFiles: numRowsFILE,
        numRoutes: numRowsROUTE,
        numPoints: numRowsPOINT
    });
});


//clear database (delete all rows from every table without deleting table)
app.post('/clearDB', async function (req, res) {
    let isCleared = true;

    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });
        await connection.execute("DELETE FROM FILE");
        await connection.execute("DELETE FROM ROUTE");
        await connection.execute("DELETE FROM POINT");
    } catch (err) {
        console.log(err);
        isCleared = false;
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        isCleared: isCleared,
    });
});


//update database when file gets changed on server
app.get('/updateDB', async function (req, res) {
    let fileDir, fileToJSON;
    let filenames = [];
    let routesToJSON, routesParsed;
    let GPXdoc;
    let gpxInsertID, routeInsertID;
    let record; //row to be added to table
    let filesStored = [], routesStored = [], pointsStored = [];
    let isStored = false;

    //get files from server
    const dir = path.join(__dirname, 'uploads'); //directory path
    fs.readdir(dir, async function (error, files) {
        if (error) throw (error);
        files.forEach(async function (file) {
            filenames.push(file);
        });
    });

    try {
        connection = await mysql.createConnection({
            host: h,
            user: u,
            password: p,
            database: db
        });

        await connection.execute("DELETE FROM FILE");
        await connection.execute("DELETE FROM ROUTE");
        await connection.execute("DELETE FROM POINT");

        let i = 0;
        for (let file of filenames) {
            if (i == 0) {
                await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 1");
                await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 1");
                await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 1");
            }
            i++;

            //check if file is already in FILE table
            let [duplicatesFILE] = await connection.execute("SELECT * FROM FILE WHERE FILE.file_name='" + file + "'");
            if (duplicatesFILE && duplicatesFILE.length > 0) {
                console.log("File", file, "already exists in database");
                continue;
            }
            fileDir = "./uploads/" + file;

            //obtain version and creator of each file
            fileToJSON = GPXParserLib.GPXFileToJSON(fileDir);
            GPXdoc = JSON.parse(fileToJSON);
            if (GPXdoc == null) continue; //invalid file

            //insert file into FILE table - auto-incremented gpx_id can be given value of 'null'
            record = "INSERT INTO FILE VALUES (null,'" + file + "'," + GPXdoc.version + ",'" + GPXdoc.creator + "')";
            let [rowsFILE, fieldsFILE] = await connection.execute(record);
            gpxInsertID = rowsFILE.insertId; //id of last insert (value of gpx_id) in FILE table

            //obtain relevant information regarding each of file's routes and each route's waypoints
            routesToJSON = GPXParserLib.getAllRouteComponentsJSON(fileDir);
            routesParsed = JSON.parse(routesToJSON);
            for (let route of routesParsed) {
                //insert route into ROUTE table 
                if (route.name == "None") record = "INSERT INTO ROUTE VALUES (null, null," + route.len + "," + gpxInsertID + ")";
                else record = "INSERT INTO ROUTE VALUES (null,'" + route.name + "'," + route.len + "," + gpxInsertID + ")";
                let [rowsROUTE, fieldsROUTE] = await connection.execute(record);
                routeInsertID = rowsROUTE.insertId; //id of last insert (value of route_id) in ROUTE table

                //insert point into POINT table - auto-incremented route_id can be given value of 'null'
                for (let waypoint of route.waypoints) {
                    if (waypoint.name == "None") {
                        record = "INSERT INTO POINT VALUES (null," + waypoint.index + "," +
                            waypoint.latitude + "," + waypoint.longitude + ", null," + routeInsertID + ")";
                    } else {
                        record = "INSERT INTO POINT VALUES (null," + waypoint.index + "," +
                            waypoint.latitude + "," + waypoint.longitude + ",'" + waypoint.name + "'," + routeInsertID + ")";
                    }
                    let [rowsPOINT, fieldsPOINT] = await connection.execute(record);
                }
            }
        }
        let [allFILE] = await connection.execute("SELECT * FROM FILE");
        let [allROUTE] = await connection.execute("SELECT * FROM ROUTE");
        let [allPOINT] = await connection.execute("SELECT * FROM POINT");
        filesStored = allFILE;
        routesStored = allROUTE;
        pointsStored = allPOINT;
        if (filesStored.length > 0) isStored = true;
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        isStored: isStored,
        filesStored: filesStored,
        routesStored: routesStored,
        pointsStored: pointsStored
    });
});


//execute Query 1
app.get('/query1', async function(req, res) {
    let sortChoice = req.query.sort; //1->name, 2->length
    let result = [];
    
    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });

        if (sortChoice == 1) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE ORDER BY route_name");
            result = rowsSortedROUTE;
        } else if (sortChoice == 2) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE ORDER BY route_len");
            result = rowsSortedROUTE;
        }
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        sortedRoutes: result
    });
});


//execute Query 2
app.get('/query2', async function(req,res) {
    let sortChoice = req.query.sort; //1->name, 2->length
    let filename = req.query.file;
    let sortedResult = [];

    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });

        //get GPX ID of filename
        let [result] = await connection.execute("SELECT * FROM FILE WHERE file_name='" + filename + "'");
        let gpxID = result[0].gpx_id;

        if (sortChoice == 1) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id=" + gpxID + " ORDER BY route_name");
            sortedResult = rowsSortedROUTE;
        } else if (sortChoice == 2) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id=" + gpxID + " ORDER BY route_len");
            sortedResult = rowsSortedROUTE;
        }
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        sortedRoutes: sortedResult
    });
});


//execute Query 3
app.get('/query3', async function (req, res) {
    let routeID = req.query.route;
    let sortedResult = [];

    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });

        //get all points with the same route ID
        let [rowsSortedPOINT] = await connection.execute("SELECT * FROM POINT WHERE route_id=" + routeID + " ORDER BY point_index");
        sortedResult = rowsSortedPOINT;
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        sortedPoints: sortedResult
    });
});


//execute Query 4
app.get('/query4', async function (req, res) {
    let sortChoice = req.query.sort; //1->name, 2->length
    let filename = req.query.file;
    let sortedRoutes = [], sortedPoints = [];

    try {
        connection = await mysql.createConnection({ //wait for this to happen before moving on
            host: h,
            user: u,
            password: p,
            database: db
        });

        //get GPX ID of filename
        let [result] = await connection.execute("SELECT * FROM FILE WHERE file_name='" + filename + "'");
        let gpxID = result[0].gpx_id;

        if (sortChoice == 1) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id=" + gpxID + " ORDER BY route_name");
            sortedRoutes = rowsSortedROUTE;            
        } else if (sortChoice == 2) {
            let [rowsSortedROUTE] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id=" + gpxID + " ORDER BY route_len");
            sortedRoutes = rowsSortedROUTE;
        }
        for (let i = 0; i < sortedRoutes.length; i++) {
            let routeID = sortedRoutes[i].route_id
            let [rowsSortedPOINT] = await connection.execute("SELECT * FROM POINT WHERE route_id=" + routeID + " ORDER BY point_index");
            sortedPoints.push(...rowsSortedPOINT); //append to list of sortedPoints
        }
    } catch (err) {
        console.log(err);
    } finally {
        if (connection && connection.end) connection.end();
    }

    res.send({
        sortedRoutes: sortedRoutes,
        sortedPoints: sortedPoints
    });
});


//log user out of database of connection still present
app.get('/logout', async function (req, res) {
    if (connection && connection.end) connection.end();
    res.send({
        isLoggedOut: true
    });
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);