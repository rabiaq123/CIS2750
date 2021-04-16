# CIS2750 - GPX Editor and Parser

## Assignment 4
Due Date: April 15, 2021

## Student Information
* Name: Rabia Qureshi
* Student ID: 1046427
* E-mail: rqureshi@uoguelph.ca

## Notes for Grader 
Professor said to add any necessary notes for the TA to help them navigate the web app easier and avoid losing marks for unexpected behaviour).

Thank you in advance for reading!
* 'Execute Query' dropdown only works as expected if user hits the "Store Files" button each time they login
* 'Clear Data', 'Add Route', and 'Rename Route' buttons will reload the page, which means you will have to login again if you wish to access the database
    * E.g. If you've added/renamed a route and would like to track the updates, you can simply hit 'Add/Rename Route', login again, and go to the "Track Updates" panel to update the DB.
* I've added a logout button at the bottom of the page for the rare occasion that database connection is lost while the app is running (this error can be found in the terminal). If you logout and log back in, the error will go away.