// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',                        //Request type
        dataType: 'json',                   //Data type - we will use JSON for almost everything 
        url: '/getGPXFilesInUploadsDir',    //The server endpoint we are connecting to
        data: {},
        success: function(data) {
            /*
            Do something with returned object
            Note that what we get is an object, not a string, 
            so we do not need to parse it on the server.
            JavaScript really does handle JSONs seamlessly
            */
            //We write the object to the console to show that the request was successful
            console.log("Page loaded successfully.");
            let GPXFilenamesArr = data.filenames;
            console.log("Files in uploads/ directory: " + GPXFilenamesArr);
            addUploadFilesToFileLog(GPXFilenamesArr);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    // $('#someform').submit(function(event){ //event
    //     $('#blah').html("Form has data: "+$('#entryBox').val());
    //     event.preventDefault();
    //     //Pass data to the Ajax call, so it gets passed to the server
    //     $.ajax({
    //         type: 'get',
    //         dataType: 'json',
    //         url: '/endpoint1',
    //         data: {
    //             stuff: $('#entryBox').val(),
    //             junk: ":))"
    //         },
    //         success: function(shtuff) {
    //             console.log(shtuff);
    //         }
    //         //Create an object for connecting to another waypoint
    //     });
    // });
});

function AddDummyRow() {
    $('#GPXViewTable').append(
        "<tr><td>One</td><td>Two</td><td>Three</td><td>sknefjks</td><td>sdfjsdhfks/td></tr>"
    );
}