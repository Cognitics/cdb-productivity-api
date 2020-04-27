# CDB Productivity API

This is the open-source version of the CDB Productivity API, with support for the OGC CDB, including mesh generation of CDB content. 

# CDB Productivity Suite User Manual

# Introduction

The CDB Productivity Suite combines several tools for the generation, validation, and visualization of CDB content. This manual describes each of these tools and presents example use cases to demonstrate the capabilities included.

More details for each tool can be found in the cognitics\docs directory.

# Components

*   **Data Injection (cdb-inject):** populates imagery and elevation from source data.
*   **Mesh Conversion (mesh2cdb):** Generates CDB content from OBJ meshes.
*   **Smart LOD Generator (cdb-lod):** creates lower level of detail imagery and elevation tiles for existing data.
*   **Validation (cdbinfo):** provides information on missing geotypical and geospecific models and textures.
*   **Attribute Translator:** converts feature attribution between different data dictionaries.





#Core Development Libraries
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

Binaries of all these can be downloaded here:

Windows: [http://cdb.cognitics.net/ThirdParty.7z](http://cdb.cognitics.net/ThirdParty.7z)

Linux: [http://cdb.cognitics.net/ThirdPartyLinux.tgz](http://cdb.cognitics.net/ThirdPartyLinux.tgz)
