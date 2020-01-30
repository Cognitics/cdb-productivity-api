# cdbmesh

This is the open-source version of Cognitics' libraries with emerging support for the OGC CDB, including mesh generation of CDB content. 

|Library|Description                              |
|-------|-----------------------------------------|
|cad|Cognitics Attribute Dictionary|
|ccl|Cognitics Core Library|
|ctl|Cognitics Triangulation Library|
|cts|Coordinate Transform|
|dbi|SQL binding library, inspired by PERL DBI|
|dbi_sqlite|dbi implemented for SQLite|
|dom|XML parser (not recommended for general purpose use)|
|elev|Elevation sampler|
|flt|OpenFlight library|
|ip|Image Processing Library|
|lmfit|Levenberg-Marquardt minimization algorithms used in a variety of least-squares problems|
|md5|md5 hash library|
|ogr|GDAL OGR library for the SFA library|
|scenegraph|Scenegraph library optimized for procedural generation (not runtime performance)|
|scenegraphflt|OpenFlight reader/writer for the scenegraph library|
|sfa|Cognitics implementation of the OGC SFA. Includes many geometric operations as well as a Feature object with attributes|
|sqlite|SQLite library|
|tg|Terrain Generation Library. Generates textured meshes from image and DEM.|


# Third Party Requirements

Currently, the solution is expecting the following libraries in this directory:

- boost_1_69_0
- gdal204
- IPP (2019 build)
- jpeg-8c
- lpng154

Binaries of all these (except boost) can be downloaded here:

Windows: [http://cdb.cognitics.net/ThirdParty.7z](http://cdb.cognitics.net/ThirdParty.7z)

Linux: [http://cdb.cognitics.net/ThirdPartyLinux.tgz](http://cdb.cognitics.net/ThirdPartyLinux.tgz)
