
# CDB Data Injection


# Overview

The CDB Data Injection tool populates CDB imagery and elevation from source data. By default, it selected level of detail based on the resolution of the source rasters. If the CDB does not exist, a new one is created with the new data. If the CDB does exist, source data is injected into existing tiles, overwriting pixels in the existing content only where source data overlap.


# Usage

This tool is run from the command line via the _Command Prompt_ menu item in the _CDB Productivity Suite_ program group. The executable is named **cdb-inject.exe**.


```
cdb-inject [options] <CDB>
```


Where `<CDB>` is the path to an existing CDB.

Options:


<table>
  <tr>
   <td><code>-logfile &lt;filename></code>
   </td>
   <td>Filename for log output
   </td>
  </tr>
  <tr>
   <td><code>-workers &lt;N></code>
   </td>
   <td>Number of worker threads (default: 8)
   </td>
  </tr>
  <tr>
   <td><code>-bounds &lt;s> &lt;w> &lt;n> &lt;e></code>
   </td>
   <td>Bounds for area of interest
   </td>
  </tr>
  <tr>
   <td><code>-elevation &lt;file/path></code>
   </td>
   <td>Source elevation filename or path
   </td>
  </tr>
  <tr>
   <td><code>-imagery &lt;file/path></code>
   </td>
   <td>Source imagery filename or path
   </td>
  </tr>
  <tr>
   <td><code>-build-overviews</code>
   </td>
   <td>Perform LOD downsampling
   </td>
  </tr>
  <tr>
   <td><code>-lod &lt;lod></code>
   </td>
   <td>Target LOD
   </td>
  </tr>
</table>



# Example

Suppose there is a set of imagery files in a source directory (D:\imagery) and a separate elevation file (D:\elevation\elev.tif) for the same area. To create a CDB (D:\MyCDB) for this source data:


```
cdb-inject -elevation D:\elevation\elev.tif -imagery D:\imagery D:\MyCDB
```


This will create D:\MyCDB with imagery and elevation tiles at LODs matching the source raster resolution.

A new higher level imagery subset becomes available (D:\new_imagery.tif). This file overlaps a part of the first imagery and matches the LOD. To add that to an existing CDB:


```
cdb-inject -imagery D:\new_imagery.tif D:\MyCDB
```


This automatically detects the target LOD and injects it into the imagery already in the CDB. Only pixels that are covered by the new data are overwritten while existing pixels are preserved.

