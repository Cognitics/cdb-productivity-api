# Mesh to CDB Converter (mesh2cdb)

# Introduction
Mesh2CDB reads textured meshes, and populates a CDB datastore with imagery and elevation.

Imagery is accomplished by rendering OBJ files with OpenGL to a texture, using an orthographic projection. Each texture is aligned exactly with a CDB tile, and is written to a JPEG 2000 file in the output CDB store. After the rendering, the depth buffer is used to calculate a digital terrain model (DTM), which is stored in the output CDB as a GeoTIFF file. Note that the CDB should have Digital Surface Models (DSM), so technically, this is not compliant with the CDB standard. In the future, buildings and other cultural data will be removed from the DTM to make a DSM.

There are some expectations of the OBJ content for this to work. 

1. The projection for the OBJ content must be a known Well-Known-Text (WKT) string, or the projection must be East-North-UP (ENU).
2. Textures must be PNG or JPEG (dds support is not stable at the moment)

To start the conversion, you need an XML file to configure the process. An example is below:

```xml
<?xml version="1.0" encoding="utf-8"?>

<mesh2cdb maxlod="12" outputdir="D:\output_cdb_directory" highest_lod_only="false">
    <origin x="-117.2543578" y="32.79946724"/>
    <offset x="0" y="0" z="0"/>
    <wkt>ENU</wkt>
    <sources texturepath="" objdir="D:\obj\area_export_base_color" recurse="false">
    </sources>
</mesh2cdb>
```

If the OBJ is projected with UTM, your xml file would lok something like this:

```xml
<?xml version="1.0" encoding="utf-8"?>

<mesh2cdb maxlod="12" outputdir="D:\output_cdb_directory" highest_lod_only="false">
    <origin x="0" y="0"/>
    <offset x="205366.64844575466" y="1234567.189701308" z="65.70969764043629"/>
    <wkt>PROJCS["WGS 84 / UTM zone 18N",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]],PROJECTION["Transverse_Mercator"],PARAMETER["latitude_of_origin",0],PARAMETER["central_meridian",-75],PARAMETER["scale_factor",0.9996],PARAMETER["false_easting",500000],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AUTHORITY["EPSG","32618"]]</wkt>
    <sources texturepath="" objdir="D:\obj\area_export_base_color" recurse="false">
    </sources>
</mesh2cdb>
```

Notice that if you are using a WKT projection, you usually won't have an origin, but if you are using ENU, you must have one. Offsets are optional.

To start the conversion process, the command line will look something like this:

mesh2cdb -config c:\path\to\config.xml