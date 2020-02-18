# CDB Smart LOD Generator

# Overview

The CDB Smart LOD Generator is a tool for creating lower level of detail elevation and imagery based on existing content. Rather than simply downsampling existing data, it finds the highest detail data and selectively injects it into lower levels of detail based on file timestamps. If no lower level exists, it is created as would be expected in typical downsampling.


# Usage

This tool is run from the command line via the _Command Prompt_ menu item in the _CDB Productivity Suite_ program group. The executable is named **cdb-lod.exe**.


```
cdb-lod [options] <CDB>
```


Where `<CDB>` is the path to an existing CDB.

Options:


<table>
  <tr>
   <td><code>-logfile &lt;filename&gt;</code>
   </td>
   <td>Filename for log output
   </td>
  </tr>
  <tr>
   <td><code>-workers &lt;N&gt;</code>
   </td>
   <td>Number of worker threads (default: 8)
   </td>
  </tr>
</table>



# Example

Suppose there are two source imagery files that have been used to generate a new CDB (D:\MyCDB). One is a lower resolution (LOD 2 equivalent) and one is high resolution (LOD 9 equivalent) occupying a subset of the lower resolution area. The high resolution data is from 2019 and the low resolution data is from 2020.

To run the smart lod generator, run cdb-lod from the command line as follows:

```
cdb-lod D:\MyCDB 
```

In the target CDB, tiles will be generated for LODs 3-8 for the bounds of the high resolution data. However, since the lower resolution data is newer, LODs 2 and lower will not be overwritten.
