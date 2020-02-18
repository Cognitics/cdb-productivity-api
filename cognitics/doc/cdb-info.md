# CDB Validation


# Overview

The CDB Validation tool provides information on missing geotypical and geospecific models and textures. It searches the GTFeature and GSFeature datasets to identify matching models and reports if the associated model files are missing from the CDB. If the model exists, it inventories the textures referenced by the model and reports if the associated texture files are missing from the CDB.


# Usage

This tool is run from the command line via the _Command Prompt_ menu item in the _CDB Productivity Suite_ program group. The executable is named **cdbinfo.exe**.


```
cdbinfo [options] <CDB>
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
   <td><code>-bounds &lt;s> &lt;w> &lt;n> &lt;e></code>
   </td>
   <td>Bounds for area of interest
   </td>
  </tr>
  <tr>
   <td><code>-gsfeatures</code>
   </td>
   <td>Test GSFeature dataset
   </td>
  </tr>
  <tr>
   <td><code>-gtfeatures</code>
   </td>
   <td>Test GTFeature dataset
   </td>
  </tr>
</table>


