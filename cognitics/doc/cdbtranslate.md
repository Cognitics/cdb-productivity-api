

# Attribute Translator


# Overview

The attribute translator is a tool for converting feature attributes from a source dataset into the CDB data dictionary. It also supports translation to other data dictionaries.


# Usage

This tool is run from the command line via the _Command Prompt_ menu item in the _CDB Productivity Suite_ program group. The executable is named **AttributeTranslator.exe**. A GUI wrapper is also available.


```
AttributeTranslator <RuleSet.xml> <input> <output> [layer1] [layer2] ... [layerN]
```


Where:


<table>
  <tr>
   <td><code>&lt;RuleSet.xml&gt;</code>
   </td>
   <td>Path and filename to XML translation rules
   </td>
  </tr>
  <tr>
   <td><code>&lt;input&gt;</code>
   </td>
   <td>Input vector path and filename.
<p>
Any GDAL supported vector format (such as Shapefile, GeoPackage, ESRI File Geodatabase) is supported.
   </td>
  </tr>
  <tr>
   <td><code>&lt;output&gt;</code>
   </td>
   <td>Output vector path and filename.
<p>
Supported formats are: Shapefile, GeoPackage, ESRI File Geodatabase. If the file exists, features will be appended.
   </td>
  </tr>
  <tr>
   <td><code>[layer#]</code>
   </td>
   <td>Zero or more layers separated by spaces. If no layers are specified, all layers will be translated.
   </td>
  </tr>
</table>



# Rulesets


## Concepts

Rulesets are defined as a hierarchy of conditionals (**if** tags in the xml file), structured as a tree. Each feature is passed through the tree, descending further as long as each condition is evaluated as ‘true’. Each conditional may contain tags that set attributes on the features. Once a conditional evaluates to ‘true’, **all **set tags directly inside the scope of the ‘ **if **’ tag are executed. Each conditional at that level are next evaluated. Once a condition evaluates to false, none of the set or if commands below are processed.

It is important to be aware that tags in XML are not guaranteed to be executed in the same order.


## Layers

Features processed through the ruleset can be directed to a different layer in the output file by

setting the **output-layer** attribute. For example:


```
<set attr="output-layer" literal="translated_linear_road" type="string" />
```


The above will cause the feature to be placed in the translated_linear_road layer in the output

File.

All features will have the **source-layer** attribute indicating the layer name that the input feature

originated from.


## Enumerations

Enumerations provide the capability to associate a symbol to a different value. This functionality exists to simplify the process of creating rules, as well as making the rules more readable. Some data dictionaries use numeric values to represent different semantic values. For example, Concrete may be specified as the integer **1**, Dirt as **2**, etc. In this example, the following enumeration can be specified:


```
<enum name="surface_material" type="int">
<symbol name="Concrete" value="1"/>
<symbol name="Dirt" value="2"/>
<symbol name="Asphalt" value="3"/>
</enum>
```


Enumeration values are specified by using the literal value in a set tag to a value starting with the @ symbol, then the enum name, a colon, and then the symbolic name. Using the above enum example, if you wanted to set a Concrete value, you would use an **if **tag such as this:


```
<set attr="SMC" literal="@surface_material:Concrete" />
```


In the above case, the SMC attribute would be set to a value of ‘1’ in the output feature.


## Mapping Tables

Mapping tables provide a quick way to map a set of values to a different set of values. The same functionality could be processed with a series of **if **and **set **values, but using a map table simplifies this process.


```
<map-table name="yyy" default="1" type="int">
<lookup source-value="Concrete" map-to="@surface_material:Concrete" />
<lookup source-value="Asphalt" map-to="@surface_material:Asphalt"/>
<lookup source-value="Rock" map-to="999" />
</map-table>
```



## Conditionals

The type of evaluations used is specified with the **op **attribute in an **if **tag. Supported operations

currently include:



*   Always True: (always)
    *   Always is a convenience operation that allows the user to organize a block of conditions and **sets **that are always processed.
*   Equals: (eq)
*   Not Equals: (neq)
*   Less Than (lt)
*   Greater Than (gt)
*   In Set (in)
    *   When the in operation is used, a set of values are specified as child **<Value>** tags. If the source attribute matches any of the child **Value **tags, the condition is **true**.


```
<if attr="land_route_type" op="in">
<value>tank_trail</value>value>
<value>trail</value>value>
<set attr="FACC" literal="AP050"/>
</if>
```



## Setting Attributes

**‘set’ syntax:**


```
<set attr="LNAM" source-attr='name' type="string" />
```


**‘map’ syntax:**


```
<map source-attr="xyz" dest-attr="abc" table="yyy">
```
