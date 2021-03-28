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
    //return type of calling function first, argument list (what we're passing into function) second
    //for void input type, leave argument list empty
    //listing all wrappers
    
});


//send array of filenames in uploads directory
app.get('/getGPXFilesInUploadsDir', function (req, res) {
    const dir = path.join(__dirname, 'uploads'); //directory path

    //read directory
    fs.readdir(dir, function (error, files) {
        //handle error in reading
        if (error) return console.log('ERROR: could not read directory: ' + error);
        res.send({
            filenames: files
        });
    });
});




// //Sample endpoint
// app.get('/endpoint1', function(req , res){ //request (shit we're passing) and response (shit we're sending back)
//     let retStr = req.query.stuff + " " + req.query.junk;
//     res.send({
//         stuff: retStr
//     }); //getting JSON object
// });

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);