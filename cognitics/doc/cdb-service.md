# WMS Server for CDB

# Introduction
cdb-service is a very lightweight WMS server, intended for a single user. It can be used through your localhost network to view the imagery layer in a CDB repository. 

It doesn't take any parameters, after you start it up, go to this URL in your browser:

http://localhost:8080/

It will prompt you to enter the path for a CDB. Once you enter it, it will take you to a browser-based WMS client.

If you want to use the WMS service in an application like QGIS, use a URL like this:

http://localhost:8080/wms

If you are building this yourself, make sure that the htdocs directory is placed as a subdirectory of the working directory when you launch cdb-service. You can find the htdocs directory in this repository under /cognitics/htdocs.