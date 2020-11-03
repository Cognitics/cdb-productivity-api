
cmake_minimum_required(VERSION 2.8)

project(cognitics)

if(${CMAKE_VERSION} VERSION_GREATER "3.9.0")
    include(CheckLanguage)
    check_language(CUDA)
endif()

if(CMAKE_CUDA_COMPILER)
    enable_language(CUDA)
endif(CMAKE_CUDA_COMPILER)


if(WIN32)
    if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "$(Configuration)")
    message("Defaulting to $(Configuration) build type macro for Visual Studio")
    endif(NOT DEFINED CMAKE_BUILD_TYPE)
endif(WIN32)

#cmake -G "Unix Makefiles" -DCOG_BUILD_GL_TOOLS=1 -DTHIRD_PARTY_DIR=/home/ubuntu/cognitics/ThirdParty -DCMAKE_INSTALL_PREFIX=/usr/local ..
#make DESTDIR=/home/ubuntu/cdbtools install

#cmake -G "Visual Studio 16 2019" -A x64 -T v141 -DCOG_BUILD_GL_TOOLS=1 ..

################################################################################

if(WIN32)
    SET(CMAKE_CONFIGURATION_TYPES "Release;Debug")
endif(WIN32)

if(UNIX)
    add_definitions(-DLINUX)
endif(UNIX)

if(NOT DEFINED THIRD_PARTY_DIR)
    if(WIN32)
        set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/ThirdParty.win32")
    endif(WIN32)
    if(UNIX)
        set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/ThirdParty.linux")
    endif(UNIX)
endif(NOT DEFINED THIRD_PARTY_DIR)

set(IPP_STATIC "ON")

if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++17 -g -o3")
    set(CMAKE_EXE_LINKER_FLAGS "-L/usr/lib/x86_64-linux-gnu -Wl,-rpath='/usr/local/lib',--start-group")
    link_libraries(stdc++fs)
endif(UNIX)

if(WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "/MP /Od /EHsc /RTC1 /MDd /W3 /nologo /c /Zi /TP /errorReport:prompt -DCOG_DEBUG -DDEBUG=1")
    set(CMAKE_CXX_FLAGS_RELEASE "/DEBUG /MP /O2 /FD /EHsc /MD /W3 /nologo /c /Zi /TP /errorReport:prompt -DCOG_RELEASE -DNDEBUG")
endif(WIN32)

message("THIRD_PARTY_DIR: ${THIRD_PARTY_DIR}")

################################################################################

string(TIMESTAMP BUILDTS "%Y%m%d-%H%M")

#add_custom_target(build_clear_cache COMMAND ${CMAKE_COMMAND} -U BUILDTS ${CMAKE_BINARY_DIR})

################################################################################

set(COGCORE_HEADERS
    ./include/Version.h
    ./include/ccl/ccl.h
    ./include/ccl/FileInfo.h
    ./include/ccl/Value.h
    ./include/ccl/AttributeContainer.h
    ./include/ccl/Key.h
    ./include/ccl/VariantException.h
    ./include/ccl/ScopedTimer.h
    ./include/ccl/ccltime.h
    ./include/ccl/cds.h
    ./include/ccl/LogStream.h
    ./include/ccl/Endian.h
    ./include/ccl/Log.h
    ./include/ccl/LittleEndian.h
    ./include/ccl/Number.h
    ./include/ccl/cds_sqlite.h
    ./include/ccl/LogBuffer.h
    ./include/ccl/attributes.h
    ./include/ccl/uri.h
    ./include/ccl/Expression.h
    ./include/ccl/sem.h
    ./include/ccl/LSBitField.h
    ./include/ccl/BigEndian.h
    ./include/ccl/inspection.h
    ./include/ccl/Op.h
    ./include/ccl/binary.h
    ./include/ccl/datetime.h
    ./include/ccl/BitField.h
    ./include/ccl/thread_local_ptr.h
    ./include/ccl/md5.h
    ./include/ccl/ProgressObserver.h
    ./include/ccl/Timer.h
    ./include/ccl/Profile.h
    ./include/ccl/date.h
    ./include/ccl/Variant.h
    ./include/ccl/MSBitField.h
    ./include/ccl/Action.h
    ./include/ccl/StringUtils.h
    ./include/ccl/mutex.h
    ./include/ccl/Runnable.h
    ./include/ccl/object.h
    ./include/ccl/JobManager.h
    ./include/ccl/BindStream.h
    ./include/ccl/primitive.h
    ./include/ccl/uuid.h
    ./include/ccl/cstdint.h
    ./include/ccl/ObjLog.h
    ./include/ccl/config.h
    ./include/ccl/miniz.h
    ./include/ccl/ArgumentParser.h
    ./include/ccl/gdal.h
    ./include/ccl/tinyxml2.h
    ./include/cad/AttributeDictionary.h
    ./include/scenegraphobj/scenegraphobj.h
    ./include/scenegraphobj/quickobj.h
    ./include/sfa_file_factory/sfa_file_factory.h
    ./include/gltf/GltfData.h
    ./include/gltf/Tileset.h
    ./include/gltf/GltfInfo.h
    ./include/gltf/GltfJson.h
    ./include/elev/SimpleDEMReader.h
    ./include/elev/DataSource_Raster.h
    ./include/elev/DataSource.h
    ./include/elev/CacheEntry.h
    ./include/elev/DataPost.h
    ./include/elev/Cache.h
    ./include/elev/Elevation_DSM.h
    ./include/elev/DataSource_Raster_GDAL.h
    ./include/elev/Line.h
    ./include/elev/DataSourceManager.h
    ./include/elev/Elevation.h
    ./include/scenegraphflt/scenegraphflt.h
    ./include/tg/TerrainGenerator.h
    ./include/tg/FbxTerrainGenerator.h
    ./include/tg/GltfTerrainGenerator.h
    ./include/tg/ObjTerrainGenerator.h
    ./include/tg/OpenFlightTerrainGenerator.h
    ./include/cts/ProjInfo.h
    ./include/cts/CS_CoordinateSystemFactory.h
    ./include/cts/CS_FittedCoordinateSystem.h
    ./include/cts/WGS84FromFlatEarthMathTransform.h
    ./include/cts/CS_CoordinateSystem.h
    ./include/cts/pt.h
    ./include/cts/CS_Datum.h
    ./include/cts/WGS84ToFlatEarthMathTransform.h
    ./include/cts/CS_VerticalDatum.h
    ./include/cts/WGS84Orthographic.h
    ./include/cts/CS_ProjectedCoordinateSystem.h
    ./include/cts/CS_VerticalCoordinateSystem.h
    ./include/cts/CS_Projection.h
    ./include/cts/CS_PrimeMeridian.h
    ./include/cts/CS_GeographicCoordinateSystem.h
    ./include/cts/CT_CoordinateTransformationAuthorityFactory.h
    ./include/cts/CT_CoordinateTransformationFactory.h
    ./include/cts/CT_MathTransformFactory.h
    ./include/cts/CS_CoordinateSystemAuthorityFactory.h
    ./include/cts/CS_CompoundCoordinateSystem.h
    ./include/cts/CS_Ellipsoid.h
    ./include/cts/WGS84ToOrthographicMathTransform.h
    ./include/cts/CS_Unit.h
    ./include/cts/CS_LocalCoordinateSystem.h
    ./include/cts/cts.h
    ./include/cts/FlatEarthProjection.h
    ./include/cts/CS_HorizontalDatum.h
    ./include/cts/CS_GeocentricCoordinateSystem.h
    ./include/cts/CS_LinearUnit.h
    ./include/cts/CS_AngularUnit.h
    ./include/cts/CS_LocalDatum.h
    ./include/cts/CS_Info.h
    ./include/cts/CS_HorizontalCoordinateSystem.h
    ./include/cts/TransformRegistry.h
    ./include/cts/WGS84FromOrthographicMathTransform.h
    ./include/cts/CT_CoordinateTransformation.h
    ./include/cts/CT_MathTransform.h
    ./include/scenegraph/ExtentsVisitor.h
    ./include/scenegraph/MappedTexture.h
    ./include/scenegraph/LineString.h
    ./include/scenegraph/Scene.h
    ./include/scenegraph/GroupingVisitor.h
    ./include/scenegraph/IntegrateExternalsVisitor.h
    ./include/scenegraph/Material.h
    ./include/scenegraph/DrapeVisitor.h
    ./include/scenegraph/CoordinateTransformVisitor.h
    ./include/scenegraph/TerrainCullingVisitor.h
    ./include/scenegraph/MappedTextureMatrix.h
    ./include/scenegraph/UserData.h
    ./include/scenegraph/FlattenVisitor.h
    ./include/scenegraph/scenegraph.h
    ./include/scenegraph/LightPoint.h
    ./include/scenegraph/Relationship.h
    ./include/scenegraph/LOD.h
    ./include/scenegraph/Octree.h
    ./include/scenegraph/SceneCropper.h
    ./include/scenegraph/Face.h
    ./include/scenegraph/Color.h
    ./include/scenegraph/SetTexturePathVisitor.h
    ./include/scenegraph/Node.h
    ./include/scenegraph/ExternalReference.h
    ./include/scenegraph/TransformVisitor.h
    ./include/scenegraph/Visitor.h
    ./include/rapidjson/prettywriter.h
    ./include/rapidjson/schema.h
    ./include/rapidjson/memorybuffer.h
    ./include/rapidjson/rapidjson.h
    ./include/rapidjson/filereadstream.h
    ./include/rapidjson/writer.h
    ./include/rapidjson/error/en.h
    ./include/rapidjson/error/error.h
    ./include/rapidjson/memorystream.h
    ./include/rapidjson/ostreamwrapper.h
    ./include/rapidjson/encodedstream.h
    ./include/rapidjson/reader.h
    ./include/rapidjson/internal/ieee754.h
    ./include/rapidjson/internal/meta.h
    ./include/rapidjson/internal/strtod.h
    ./include/rapidjson/internal/swap.h
    ./include/rapidjson/internal/strfunc.h
    ./include/rapidjson/internal/itoa.h
    ./include/rapidjson/internal/biginteger.h
    ./include/rapidjson/internal/dtoa.h
    ./include/rapidjson/internal/diyfp.h
    ./include/rapidjson/internal/pow10.h
    ./include/rapidjson/internal/regex.h
    ./include/rapidjson/internal/stack.h
    ./include/rapidjson/pointer.h
    ./include/rapidjson/stream.h
    ./include/rapidjson/document.h
    ./include/rapidjson/istreamwrapper.h
    ./include/rapidjson/allocators.h
    ./include/rapidjson/filewritestream.h
    ./include/rapidjson/stringbuffer.h
    ./include/rapidjson/encodings.h
    ./include/rapidjson/fwd.h
    ./include/rapidjson/msinttypes/inttypes.h
    ./include/rapidjson/msinttypes/stdint.h
    ./include/flt/ExtensionFieldBoolean.h
    ./include/flt/VertexWithColorUV.h
    ./include/flt/SoundPalette.h
    ./include/flt/ExtensionFieldXML.h
    ./include/flt/ColorPalette.h
    ./include/flt/InstanceDefinition.h
    ./include/flt/BoundingCylinder.h
    ./include/flt/OpenFlight.h
    ./include/flt/MultiTexture.h
    ./include/flt/MeshPrimitive.h
    ./include/flt/VertexList.h
    ./include/flt/Curve.h
    ./include/flt/BinarySeparatingPlane.h
    ./include/flt/ExtendedMaterialEmissive.h
    ./include/flt/RotateScaleToPoint.h
    ./include/flt/ExtendedMaterialAmbient.h
    ./include/flt/MaterialPalette.h
    ./include/flt/VertexPalette.h
    ./include/flt/ExtensionFieldInteger.h
    ./include/flt/LightPointAnimationPalette.h
    ./include/flt/BoundingHistogram.h
    ./include/flt/RoadPath.h
    ./include/flt/Replicate.h
    ./include/flt/PopExtension.h
    ./include/flt/ExtendedMaterialHeader.h
    ./include/flt/PushSubface.h
    ./include/flt/ExtensionFieldString.h
    ./include/flt/PushExtension.h
    ./include/flt/ExtendedMaterialDiffuse.h
    ./include/flt/Translate.h
    ./include/flt/flt.h
    ./include/flt/BoundingVolumeCenter.h
    ./include/flt/PopSubface.h
    ./include/flt/Matrix.h
    ./include/flt/Unknown.h
    ./include/flt/Record.h
    ./include/flt/ExtensionGUIDPalette.h
    ./include/flt/NameTable.h
    ./include/flt/Vector.h
    ./include/flt/UVList.h
    ./include/flt/Mesh.h
    ./include/flt/LightSource.h
    ./include/flt/LightSourcePalette.h
    ./include/flt/LightPointSystem.h
    ./include/flt/ShaderPalette.h
    ./include/flt/GeneralMatrix.h
    ./include/flt/Put.h
    ./include/flt/ClipRegion.h
    ./include/flt/Sound.h
    ./include/flt/ExtensionFieldDouble.h
    ./include/flt/PushAttribute.h
    ./include/flt/PushLevel.h
    ./include/flt/RoadConstruction.h
    ./include/flt/ExtendedMaterialBumpMap.h
    ./include/flt/Header.h
    ./include/flt/LevelOfDetail.h
    ./include/flt/Extension.h
    ./include/flt/LightPoint.h
    ./include/flt/RoadSegment.h
    ./include/flt/RotateAboutEdge.h
    ./include/flt/KeyTable.h
    ./include/flt/PopAttribute.h
    ./include/flt/BoundingVolumeOrientation.h
    ./include/flt/VertexWithColorNormal.h
    ./include/flt/ExtendedMaterialLightMap.h
    ./include/flt/ExtensionFieldFloat.h
    ./include/flt/MorphVertexList.h
    ./include/flt/Scale.h
    ./include/flt/CATData.h
    ./include/flt/BoundingBox.h
    ./include/flt/LongID.h
    ./include/flt/Face.h
    ./include/flt/CAT.h
    ./include/flt/ExtendedMaterialSpecular.h
    ./include/flt/Switch.h
    ./include/flt/ExtendedMaterialNormalMap.h
    ./include/flt/LocalVertexPool.h
    ./include/flt/EyepointTrackplanePalette.h
    ./include/flt/Group.h
    ./include/flt/TexturePalette.h
    ./include/flt/BoundingConvexHull.h
    ./include/flt/Text.h
    ./include/flt/PopLevel.h
    ./include/flt/RotateAboutPoint.h
    ./include/flt/DegreeOfFreedom.h
    ./include/flt/ExtendedMaterialShadowMap.h
    ./include/flt/LightPointAppearancePalette.h
    ./include/flt/IndexedString.h
    ./include/flt/TextureMappingPalette.h
    ./include/flt/Object.h
    ./include/flt/BoundingSphere.h
    ./include/flt/LineStylePalette.h
    ./include/flt/ExtendedMaterialAlpha.h
    ./include/flt/IndexedLightPoint.h
    ./include/flt/LinkagePalette.h
    ./include/flt/ExternalReference.h
    ./include/flt/Comment.h
    ./include/flt/RoadZone.h
    ./include/flt/VertexWithColorNormalUV.h
    ./include/flt/ExtendedMaterialReflectionMap.h
    ./include/flt/VertexWithColor.h
    ./include/flt/InstanceReference.h
    ./include/Queue.h
    ./include/sfa/Distance.h
    ./include/sfa/GeometryGraph.h
    ./include/sfa/PlaneFitter.h
    ./include/sfa/MultiSurface.h
    ./include/sfa/LineString.h
    ./include/sfa/Overlay3D.h
    ./include/sfa/EdgeGroupDivision.h
    ./include/sfa/PolyhedralSurface.h
    ./include/sfa/Curve.h
    ./include/sfa/PointNode.h
    ./include/sfa/Relate.h
    ./include/sfa/Surface.h
    ./include/sfa/Distance3D.h
    ./include/sfa/sfa.h
    ./include/sfa/EdgeIntersector.h
    ./include/sfa/Label.h
    ./include/sfa/MultiPoint.h
    ./include/sfa/LineMath.h
    ./include/sfa/Factory.h
    ./include/sfa/RingMath.h
    ./include/sfa/Projection2D.h
    ./include/sfa/Matrix.h
    ./include/sfa/File.h
    ./include/sfa/ConvexHull.h
    ./include/sfa/Quat.h
    ./include/sfa/GeometryCollection.h
    ./include/sfa/EdgeNodeEnd.h
    ./include/sfa/Buffer.h
    ./include/sfa/PointLineIntersector.h
    ./include/sfa/MultiPolygon.h
    ./include/sfa/Intersector.h
    ./include/sfa/GraphOperation.h
    ./include/sfa/ConvexHull3D.h
    ./include/sfa/EdgeGroup.h
    ./include/sfa/Feature.h
    ./include/sfa/RelateCompute3D.h
    ./include/sfa/GeometrySnapper.h
    ./include/sfa/PointExtractor.h
    ./include/sfa/BSP.h
    ./include/sfa/Layer.h
    ./include/sfa/SegmentIntersector.h
    ./include/sfa/Geometry.h
    ./include/sfa/MultiLineString.h
    ./include/sfa/PlanarGraph.h
    ./include/sfa/PolygonClipper.h
    ./include/sfa/Polygon.h
    ./include/sfa/PatchUnion.h
    ./include/sfa/EdgeGroupBuilder.h
    ./include/sfa/PlanarGraphOperation.h
    ./include/sfa/EnvelopeCheck.h
    ./include/sfa/EdgeNode.h
    ./include/sfa/Point.h
    ./include/sfa/Buffer3D.h
    ./include/sfa/PointMath.h
    ./include/sfa/RelateCompute.h
    ./include/sfa/RelateOperation.h
    ./include/sfa/PointLocator.h
    ./include/sfa/Overlay.h
    ./include/sfa/MultiCurve.h
    ./include/sfa/Intersector3D.h
    ./include/sfa/de9im.h
    ./include/ctl/BlockList.h
    ./include/ctl/Vector.h
    ./include/ctl/TIN.h
    ./include/ctl/DelaunayTriangulation.h
    ./include/ctl/QuadEdge.h
    ./include/ctl/Vertex.h
    ./include/ctl/ctl.h
    ./include/ctl/ConstraintMap.h
    ./include/ctl/Edge.h
    ./include/ctl/Subdivision.h
    ./include/ctl/QTriangulate.h
    ./include/ctl/CGrid.h
    ./include/ctl/ID.h
    ./include/ctl/LocationResult.h
    ./include/ctl/Util.h
    ./include/dom/CDATASection.h
    ./include/dom/Notation.h
    ./include/dom/Attr.h
    ./include/dom/DOMError.h
    ./include/dom/LSSerializer.h
    ./include/dom/dom.h
    ./include/dom/EntityReference.h
    ./include/dom/base.h
    ./include/dom/UserDataHandler.h
    ./include/dom/NodeIterator.h
    ./include/dom/DOMImplementation.h
    ./include/dom/DOMException.h
    ./include/dom/LSParser.h
    ./include/dom/LSException.h
    ./include/dom/DocumentType.h
    ./include/dom/LSOutput.h
    ./include/dom/LSInput.h
    ./include/dom/Entity.h
    ./include/dom/DOMErrorHandler.h
    ./include/dom/LSResourceResolver.h
    ./include/dom/Range.h
    ./include/dom/TypeInfo.h
    ./include/dom/Document.h
    ./include/dom/TreeWalker.h
    ./include/dom/ProcessingInstruction.h
    ./include/dom/DOMLocator.h
    ./include/dom/DOMConfiguration.h
    ./include/dom/NodeFilter.h
    ./include/dom/DocumentFragment.h
    ./include/dom/RangeException.h
    ./include/dom/LSParserFilter.h
    ./include/dom/LSSerializerFilter.h
    ./include/dom/Text.h
    ./include/dom/CharacterData.h
    ./include/dom/Element.h
    ./include/dom/Node.h
    ./include/dom/Comment.h
    ./include/dbi_sqlite/File.h
    ./include/dbi_sqlite/Statement.h
    ./include/dbi_sqlite/dbi_sqlite.h
    ./include/sqlite/sqlite3.h
    ./include/sqlite/sqlite3ext.h
    ./include/ip/GDALRasterSampler.h
    ./include/ip/imageinfo.h
    ./include/ip/ip.h
    ./include/ip/rasterPoly.h
    ./include/ip/attr.h
    ./include/ip/GDALRasterReader.h
    ./include/ip/pngwrapper.h
    ./include/ip/jpgwrapper.h
    ./include/ip/SkipList.h
    ./include/ip/rgb.h
    ./include/ogr/File.h
    ./include/ogr/Feature.h
    ./include/ogr/OGRLayer.h
    ./include/ogr/Field.h
    ./include/ogr/ogr.h
    ./include/scenegraphgltf/scenegraphgltf.h
    ./include/scenegraph_gl/scenegraph_gl.h
    ./include/lmfit/lmmin.h
    ./include/lmfit/lmstruct.h
    ./include/lmfit/lmcurve_tyd.h
    ./include/lmfit/lmcurve.h
    ./include/b64/base64.h
    ./meshgen/ws/WebServices.h
    ./include/features/FeatureInfo.h
    ./include/features/GMLParser.h
    ./include/features/GsBuildings.h

    ./include/cdb_tile/Coordinates.h
    ./include/cdb_tile/CoordinatesRange.h
    ./include/cdb_tile/Dataset.h
    ./include/cdb_tile/LOD.h
    ./include/cdb_tile/LODRange.h
    ./include/cdb_tile/Latitude.h
    ./include/cdb_tile/Longitude.h
    ./include/cdb_tile/Tile.h
    ./include/cdb_tile/TileCoordinates.h
    ./include/cdb_tile/TileLatitude.h
    ./include/cdb_tile/TileLongitude.h
    ./include/cdb_tile/util.h

	./include/dbflib/DBaseColDef.h
	./include/dbflib/DBaseField.h
	./include/dbflib/DBaseFieldProperty.h
	./include/dbflib/DBaseFile.h
	./include/dbflib/DBaseHeader.h
	./include/dbflib/DBaseRecord.h

	./include/cdb_util/cdb_util.h
	./include/cdb_util/FeatureDataDictionary.h
	./include/cdb_util/cdb_lod.h
	./include/cdb_util/cdb_inject.h
	./include/cdb_util/cdb_sample.h
	./include/cdb_util/cdb_service.h

	./include/civetweb/civetweb.h
	./include/civetweb/CivetServer.h

    ./include/ModelKit/Attachment.h
    ./include/ModelKit/AttachmentPoint.h
    ./include/ModelKit/AvoidArea.h
    ./include/ModelKit/AvoidClipper.h
    ./include/ModelKit/Connection.h
    ./include/ModelKit/CrossSection.h
    ./include/ModelKit/Edge.h
    ./include/ModelKit/Face.h
    ./include/ModelKit/FlightToCSM.h
    ./include/ModelKit/Material.h
    ./include/ModelKit/ModelGenerator.h
    ./include/ModelKit/ModelInstance.h
    ./include/ModelKit/ModelKit.h
    ./include/ModelKit/Reference.h
    ./include/ModelKit/RGBA.h
    ./include/ModelKit/SceneData.h
    ./include/ModelKit/TextureReference.h
    ./include/ModelKit/TunnelFeatureGenerator.h
)

################################################################################

set(COGCORE_OS "Win32")
if(UNIX)
    set(COGCORE_OS "Unix")
endif(UNIX)

set(COGCORE_SOURCES
    ./src/cad/AttributeDictionary.cpp
    ./src/ccl/AttributeContainer.cpp
    ./src/ccl/time.cpp
    ./src/ccl/LogBuffer.cpp
    ./src/ccl/Action.cpp
    ./src/ccl/config.cpp
    ./src/ccl/Key.cpp
    ./src/ccl/Endian.cpp
    ./src/ccl/ScopedTimer.cpp
    ./src/ccl/Log.cpp
    ./src/ccl/Profile.cpp
    ./src/CoordinateSystems/EGM.cpp
    ./src/CoordinateSystems/EGM2008.cpp
    ./src/CoordinateSystems/EGMTransform.cpp
    ./src/CoordinateSystems/EllipsoidTangentPlane.cpp
    ./src/CoordinateSystems/GeoidTangentPlane.cpp
    ./src/CoordinateSystems/SphereTangentPlane.cpp
    ./src/CoordinateSystems/WGS84Transform.cpp
    ./src/ccl/${COGCORE_OS}/Runnable.cpp
    ./src/ccl/${COGCORE_OS}/mutex.cpp
    ./src/ccl/${COGCORE_OS}/sem.cpp
    ./src/ccl/${COGCORE_OS}/Timer.cpp
    ./src/ccl/ObjLog.cpp
    ./src/ccl/datetime.cpp
    ./src/ccl/Op.cpp
    ./src/ccl/uuid.cpp
    ./src/ccl/Value.cpp
    ./src/ccl/date.cpp
    ./src/ccl/LogStream.cpp
    ./src/ccl/binary.cpp
    ./src/ccl/md5.cpp
    ./src/ccl/object.cpp
    ./src/ccl/inspection.cpp
    ./src/ccl/FileInfo.cpp
    ./src/ccl/Expression.cpp
    ./src/ccl/JobManager.cpp
    ./src/ccl/Number.cpp
    ./src/ccl/Variant.cpp
    ./src/ccl/primitive.cpp
    ./src/ccl/ProgressObserver.cpp
    ./src/ccl/miniz.c
    ./src/ccl/ArgumentParser.cpp
    ./src/ccl/gdal.cpp
    ./src/ccl/tinyxml2.cpp
    ./src/sfa_file_factory/sfa_file_factory.cpp
    ./src/gltf/GltfJson.cpp
    ./src/gltf/GltfInfo.cpp
    ./src/gltf/GltfData.cpp
    ./src/gltf/Tileset.cpp
    ./src/elev/Elevation.cpp
    ./src/elev/Cache.cpp
    ./src/elev/CacheEntry.cpp
    ./src/elev/DataPost.cpp
    ./src/elev/Elevation_DSM.cpp
    ./src/elev/SimpleDEMReader.cpp
    ./src/elev/DataSourceManager.cpp
    ./src/elev/DataSource.cpp
    ./src/elev/DataSource_Raster_GDAL.cpp
    ./src/elev/DataSource_Raster.cpp
    ./src/scenegraphflt/buildSceneFromOpenFlight.cpp
    ./src/scenegraphflt/buildOpenFlightFromScene.cpp
    ./src/tg/TerrainGenerator.cpp
    ./src/tg/FbxTerrainGenerator.cpp
    ./src/tg/GltfTerrainGenerator.cpp
    ./src/tg/ObjTerrainGenerator.cpp
    ./src/tg/OpenFlightTerrainGenerator.cpp
    ./src/cts/WGS84ToFlatEarthMathTransform.cpp
    ./src/cts/CS_GeographicCoordinateSystem.cpp
    ./src/cts/ProjInfo.cpp
    ./src/cts/CS_FittedCoordinateSystem.cpp
    ./src/cts/CS_HorizontalDatum.cpp
    ./src/cts/WGS84FromOrthographicMathTransform.cpp
    ./src/cts/TransformRegistry.cpp
    ./src/cts/CS_Ellipsoid.cpp
    ./src/cts/CS_Projection.cpp
    ./src/cts/CS_LocalDatum.cpp
    ./src/cts/CS_PrimeMeridian.cpp
    ./src/cts/CS_CoordinateSystemFactory.cpp
    ./src/cts/CS_CoordinateSystem.cpp
    ./src/cts/CS_Info.cpp
    ./src/cts/CS_VerticalDatum.cpp
    ./src/cts/CS_Unit.cpp
    ./src/cts/CS_HorizontalCoordinateSystem.cpp
    ./src/cts/CT_CoordinateTransformationFactory.cpp
    ./src/cts/CS_LinearUnit.cpp
    ./src/cts/WGS84FromFlatEarthMathTransform.cpp
    ./src/cts/WGS84ToOrthographicMathTransform.cpp
    ./src/cts/FlatEarthProjection.cpp
    ./src/cts/CS_AngularUnit.cpp
    ./src/cts/CT_MathTransform.cpp
    ./src/cts/CS_Datum.cpp
    ./src/cts/CS_GeocentricCoordinateSystem.cpp
    ./src/cts/CT_CoordinateTransformation.cpp
    ./src/cts/CS_LocalCoordinateSystem.cpp
    ./src/cts/WGS84Orthographic.cpp
    ./src/cts/CS_VerticalCoordinateSystem.cpp
    ./src/cts/CT_MathTransformFactory.cpp
    ./src/cts/CS_CompoundCoordinateSystem.cpp
    ./src/cts/CS_ProjectedCoordinateSystem.cpp
    ./src/scenegraph/TransformVisitor.cpp
    ./src/scenegraph/Visitor.cpp
    ./src/scenegraph/Scene.cpp
    ./src/scenegraph/GroupingVisitor.cpp
    ./src/scenegraph/IntegrateExternalsVisitor.cpp
    ./src/scenegraph/buildSceneFromFeature.cpp
    ./src/scenegraph/MappedTexture.cpp
    ./src/scenegraph/SGLineString.cpp
    ./src/scenegraph/DrapeVisitor.cpp
    ./src/scenegraph/LOD.cpp
    ./src/scenegraph/CoordinateTransformVisitor.cpp
    ./src/scenegraph/ExternalReference.cpp
    ./src/scenegraph/SGNode.cpp
    ./src/scenegraph/LightPoint.cpp
    ./src/scenegraph/Octree.cpp
    ./src/scenegraph/ExtentsVisitor.cpp
    ./src/scenegraph/drapeFeature.cpp
    ./src/scenegraph/TerrainCullingVisitor.cpp
    ./src/scenegraph/Color.cpp
    ./src/scenegraph/Face.cpp
    ./src/scenegraph/FlattenVisitor.cpp
    ./src/scenegraph/SetTexturePathVisitor.cpp
    ./src/scenegraph/Material.cpp
    ./src/scenegraph/SceneCropper.cpp
    ./src/scenegraph/Relationship.cpp
    ./src/scenegraph/MappedTextureMatrix.cpp
    ./src/flt/PopExtension.cpp
    ./src/flt/FltText.cpp
    ./src/flt/MultiTexture.cpp
    ./src/flt/FltComment.cpp
    ./src/flt/Scale.cpp
    ./src/flt/ExtendedMaterialBumpMap.cpp
    ./src/flt/LocalVertexPool.cpp
    ./src/flt/LineStylePalette.cpp
    ./src/flt/PushExtension.cpp
    ./src/flt/CAT.cpp
    ./src/flt/PopSubface.cpp
    ./src/flt/BoundingSphere.cpp
    ./src/flt/LongID.cpp
    ./src/flt/PushLevel.cpp
    ./src/flt/ExtensionFieldDouble.cpp
    ./src/flt/ExtendedMaterialReflectionMap.cpp
    ./src/flt/ShaderPalette.cpp
    ./src/flt/VertexWithColorNormal.cpp
    ./src/flt/RoadConstruction.cpp
    ./src/flt/BoundingConvexHull.cpp
    ./src/flt/ExtendedMaterialNormalMap.cpp
    ./src/flt/PopLevel.cpp
    ./src/flt/RotateAboutEdge.cpp
    ./src/flt/KeyTable.cpp
    ./src/flt/ExtendedMaterialShadowMap.cpp
    ./src/flt/FltVector.cpp
    ./src/flt/CATData.cpp
    ./src/flt/ClipRegion.cpp
    ./src/flt/LightPointSystem.cpp
    ./src/flt/MorphVertexList.cpp
    ./src/flt/GeneralMatrix.cpp
    ./src/flt/NameTable.cpp
    ./src/flt/LightPointAnimationPalette.cpp
    ./src/flt/PushAttribute.cpp
    ./src/flt/PopAttribute.cpp
    ./src/flt/ExtendedMaterialDiffuse.cpp
    ./src/flt/ExtensionFieldInteger.cpp
    ./src/flt/TexturePalette.cpp
    ./src/flt/BoundingHistogram.cpp
    ./src/flt/Switch.cpp
    ./src/flt/LevelOfDetail.cpp
    ./src/flt/Record.cpp
    ./src/flt/RoadZone.cpp
    ./src/flt/ExtensionFieldString.cpp
    ./src/flt/VertexWithColorNormalUV.cpp
    ./src/flt/PushSubface.cpp
    ./src/flt/MaterialPalette.cpp
    ./src/flt/IndexedString.cpp
    ./src/flt/ExtendedMaterialAlpha.cpp
    ./src/flt/InstanceReference.cpp
    ./src/flt/Put.cpp
    ./src/flt/SoundPalette.cpp
    ./src/flt/RotateScaleToPoint.cpp
    ./src/flt/FltMesh.cpp
    ./src/flt/ExtensionFieldXML.cpp
    ./src/flt/DegreeOfFreedom.cpp
    ./src/flt/MeshPrimitive.cpp
    ./src/flt/Extension.cpp
    ./src/flt/ExtensionGUIDPalette.cpp
    ./src/flt/ExtendedMaterialAmbient.cpp
    ./src/flt/ExtendedMaterialHeader.cpp
    ./src/flt/FltObject.cpp
    ./src/flt/FltFace.cpp
    ./src/flt/BoundingCylinder.cpp
    ./src/flt/RoadPath.cpp
    ./src/flt/LightPointAppearancePalette.cpp
    ./src/flt/LightSourcePalette.cpp
    ./src/flt/BoundingVolumeCenter.cpp
    ./src/flt/FltMatrix.cpp
    ./src/flt/ExtendedMaterialSpecular.cpp
    ./src/flt/VertexList.cpp
    ./src/flt/OpenFlight.cpp
    ./src/flt/ColorPalette.cpp
    ./src/flt/FltLightPoint.cpp
    ./src/flt/TextureMappingPalette.cpp
    ./src/flt/VertexWithColorUV.cpp
    ./src/flt/Translate.cpp
    ./src/flt/Replicate.cpp
    ./src/flt/FltCurve.cpp
    ./src/flt/RotateAboutPoint.cpp
    ./src/flt/Unknown.cpp
    ./src/flt/RoadSegment.cpp
    ./src/flt/VertexWithColor.cpp
    ./src/flt/ExtendedMaterialEmissive.cpp
    ./src/flt/VertexPalette.cpp
    ./src/flt/UVList.cpp
    ./src/flt/ExtendedMaterialLightMap.cpp
    ./src/flt/LightSource.cpp
    ./src/flt/LinkagePalette.cpp
    ./src/flt/Sound.cpp
    ./src/flt/FltExternalReference.cpp
    ./src/flt/BoundingBox.cpp
    ./src/flt/BoundingVolumeOrientation.cpp
    ./src/flt/IndexedLightPoint.cpp
    ./src/flt/ExtensionFieldBoolean.cpp
    ./src/flt/ExtensionFieldFloat.cpp
    ./src/flt/EyepointTrackplanePalette.cpp
    ./src/flt/Header.cpp
    ./src/flt/InstanceDefinition.cpp
    ./src/flt/Group.cpp
    ./src/flt/BinarySeparatingPlane.cpp
    ./src/sfa/RingMath.cpp
    ./src/sfa/Point.cpp
    ./src/sfa/PatchUnion.cpp
    ./src/sfa/Intersector3D.cpp
    ./src/sfa/EdgeNodeEnd.cpp
    ./src/sfa/sfa_file.cpp
    ./src/sfa/GraphOperation.cpp
    ./src/sfa/RelateCompute3D.cpp
    ./src/sfa/PlanarGraphOperation.cpp
    ./src/sfa/PointLocator.cpp
    ./src/sfa/Feature.cpp
    ./src/sfa/EdgeGroupDivision.cpp
    ./src/sfa/EdgeNode.cpp
    ./src/sfa/PointNode.cpp
    ./src/sfa/RelateCompute.cpp
    ./src/sfa/PlaneFitter.cpp
    ./src/sfa/Curve.cpp
    ./src/sfa/Quat.cpp
    ./src/sfa/PointExtractor.cpp
    ./src/sfa/Relate.cpp
    ./src/sfa/EdgeIntersector.cpp
    ./src/sfa/Intersector.cpp
    ./src/sfa/Buffer3D.cpp
    ./src/sfa/RelateOperation.cpp
    ./src/sfa/Polygon.cpp
    ./src/sfa/Geometry.cpp
    ./src/sfa/Factory.cpp
    ./src/sfa/Distance.cpp
    ./src/sfa/Projection2D.cpp
    ./src/sfa/MultiLineString.cpp
    ./src/sfa/GeometrySnapper.cpp
    ./src/sfa/EdgeGroup.cpp
    ./src/sfa/PlanarGraph.cpp
    ./src/sfa/EnvelopeCheck.cpp
    ./src/sfa/de9im.cpp
    ./src/sfa/ConvexHull3D.cpp
    ./src/sfa/GeometryGraph.cpp
    ./src/sfa/MultiCurve.cpp
    ./src/sfa/LineString.cpp
    ./src/sfa/MultiPoint.cpp
    ./src/sfa/Matrix.cpp
    ./src/sfa/PointMath.cpp
    ./src/sfa/Overlay3D.cpp
    ./src/sfa/LineMath.cpp
    ./src/sfa/Overlay.cpp
    ./src/sfa/MultiPolygon.cpp
    ./src/sfa/Label.cpp
    ./src/sfa/SegmentIntersector.cpp
    ./src/sfa/ConvexHull.cpp
    ./src/sfa/PointLineIntersector.cpp
    ./src/sfa/Layer.cpp
    ./src/sfa/Distance3D.cpp
    ./src/sfa/PolyhedralSurface.cpp
    ./src/sfa/GeometryCollection.cpp
    ./src/sfa/EdgeGroupBuilder.cpp
    ./src/sfa/BSP.cpp
    ./src/sfa/MultiSurface.cpp
    ./src/sfa/Surface.cpp
    ./src/sfa/Buffer.cpp
    ./src/sfa/PolygonClipper.cpp
    ./src/ctl/LocationResult.cpp
    ./src/ctl/Edge.cpp
    ./src/ctl/QuadEdge.cpp
    ./src/ctl/TIN.cpp
    ./src/ctl/QTriangulate.cpp
    ./src/ctl/CGrid.cpp
    ./src/ctl/DelaunayTriangulation.cpp
    ./src/ctl/Subdivision.cpp
    ./src/ctl/Vertex.cpp
    ./src/ctl/Util.cpp
    ./src/ctl/ID.cpp
    ./src/ctl/Vector.cpp
    ./src/ctl/ConstraintMap.cpp
    ./src/dom/NodeIterator.cpp
    ./src/dom/DOMConfiguration.cpp
    ./src/dom/NodeFilter.cpp
    ./src/dom/Entity.cpp
    ./src/dom/DOMError.cpp
    ./src/dom/DOMImplementation.cpp
    ./src/dom/Element.cpp
    ./src/dom/Notation.cpp
    ./src/dom/Node.cpp
    ./src/dom/DocumentFragment.cpp
    ./src/dom/CharacterData.cpp
    ./src/dom/Document.cpp
    ./src/dom/LSOutput.cpp
    ./src/dom/Text.cpp
    ./src/dom/ProcessingInstruction.cpp
    ./src/dom/EntityReference.cpp
    ./src/dom/Comment.cpp
    ./src/dom/TypeInfo.cpp
    ./src/dom/DocumentType.cpp
    ./src/dom/LSSerializer.cpp
    ./src/dom/DOMLocator.cpp
    ./src/dom/Range.cpp
    ./src/dom/Attr.cpp
    ./src/dom/LSParser.cpp
    ./src/dom/LSInput.cpp
    ./src/dom/CDATASection.cpp
    ./src/dom/TreeWalker.cpp
    ./src/dbi_sqlite/Statement.cpp
    ./src/dbi_sqlite/dbi_sqlite_file.cpp
    ./src/ip/pngwrapper.cpp
    ./src/ip/GDALRasterReader.cpp
    ./src/ip/jpgwrapper.cpp
    ./src/ip/attr_file.cpp
    ./src/ip/rasterPoly.cpp
    ./src/ip/rgb.cpp
    ./src/ip/GDALRasterSampler.cpp
    ./src/ip/ip.cpp
    ./src/ogr/OGRLayer.cpp
    ./src/ogr/File.cpp
    ./src/ogr/Field.cpp
    ./src/meshgen/meshgen.cpp
    ./src/scenegraphgltf/buildGltfFromScene.cpp
    ./src/scenegraphfbx/buildFbxFromScene.cpp
    ./src/scenegraphobj/buildObjFromScene.cpp
    ./src/scenegraphobj/buildSceneFromOBJ.cpp
    ./src/scenegraphobj/quickobj.cpp
    ./src/b64/base64.cpp    
    ./meshgen/ws/WebServices.cpp
    ./include/sqlite/sqlite3.c
    ./src/lmfit/lmmin.c
    ./src/lmfit/lmcurve.c
    ./src/lmfit/lmcurve_tyd.c
    ./src/features/FeatureInfo.cpp
    ./src/features/GMLParser.cpp
    ./src/features/GsBuildings.cpp
    ./src/cdb_tile/CDBLOD.cpp
    ./src/cdb_tile/Coordinates.cpp
    ./src/cdb_tile/CoordinatesRange.cpp
    ./src/cdb_tile/Dataset.cpp
    ./src/cdb_tile/LODRange.cpp
    ./src/cdb_tile/Latitude.cpp
    ./src/cdb_tile/Longitude.cpp
    ./src/cdb_tile/Tile.cpp
    ./src/cdb_tile/TileCoordinates.cpp
    ./src/cdb_tile/TileLatitude.cpp
    ./src/cdb_tile/TileLongitude.cpp

	./src/dbflib/DBaseColDef.cpp
	./src/dbflib/DBaseField.cpp
	./src/dbflib/DBaseFieldProperty.cpp
	./src/dbflib/DBaseFile.cpp
	./src/dbflib/DBaseHeader.cpp
	./src/dbflib/DBaseRecord.cpp

	./src/cdb_util/cdb_util.cpp
	./src/cdb_util/FeatureDataDictionary.cpp
	./src/cdb_util/cdb_lod.cpp
	./src/cdb_util/cdb_inject.cpp
	./src/cdb_util/cdb_sample.cpp
	./src/cdb_util/cdb_service.cpp

	./src/civetweb/civetweb.c
	./src/civetweb/CivetServer.cpp
	./src/civetweb/handle_form.inl
	./src/civetweb/md5.inl
	./src/civetweb/sha1.inl
	./src/civetweb/timer.inl
    
    ./src/ModelKit/Attachment.cpp
    ./src/ModelKit/AttachmentPoint.cpp
    ./src/ModelKit/AvoidArea.cpp
    ./src/ModelKit/AvoidClipper.cpp
    ./src/ModelKit/Connection.cpp
    ./src/ModelKit/CrossSection.cpp
    ./src/ModelKit/Edge.cpp
    ./src/ModelKit/Face.cpp
    ./src/ModelKit/FlightToCSM.cpp
    ./src/ModelKit/Material.cpp
    ./src/ModelKit/ModelGenerator.cpp
    ./src/ModelKit/ModelInstance.cpp
    ./src/ModelKit/ModelKit.cpp
    ./src/ModelKit/Reference.cpp
    ./src/ModelKit/TextureReference.cpp
    ./src/ModelKit/TunnelFeatureGenerator.cpp
)

################################################################################

set(COGCUDA_HEADERS
    ./include/cogcuda/ElevationSampler.h
    ./include/cogcuda/Sampler.cuh
)

set(COGCUDA_SOURCES
    ./src/cogcuda/ElevationSampler.cpp
    ./src/cogcuda/Sampler.cu
)

################################################################################

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${THIRD_PARTY_DIR}/boost_1_69_0")
include_directories("${THIRD_PARTY_DIR}/fbxsdk/include")
if(WIN32)
    message("CONFIGURATION: ${CMAKE_BUILD_TYPE}")
    include_directories("${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/include")
    #include_directories("${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/include/osg")
    include_directories("${THIRD_PARTY_DIR}/ipp2019/include")
    include_directories("${THIRD_PARTY_DIR}/jpeg-8c/include")
    include_directories("${THIRD_PARTY_DIR}/lpng154/include")
endif(WIN32)
if(UNIX)
    include_directories("${THIRD_PARTY_DIR}/linux_x64/include")
    include_directories("${THIRD_PARTY_DIR}/ipp2019_linux_x64/include")
endif(UNIX)

add_library(cogcore ${COGCORE_SOURCES} ${COGCORE_HEADERS})
source_group("Header Files\\ccl" REGULAR_EXPRESSION "include/ccl/.*$")
source_group("Header Files\\cdb_tile" REGULAR_EXPRESSION "include/cdb_tile/.*$")
source_group("Header Files\\cdb_util" REGULAR_EXPRESSION "include/cdb_util/.*$")
source_group("Header Files\\civetweb" REGULAR_EXPRESSION "include/civetweb/.*$")
source_group("Header Files\\ctl" REGULAR_EXPRESSION "include/ctl/.*$")
source_group("Header Files\\cts" REGULAR_EXPRESSION "include/cts/.*$")
source_group("Header Files\\dbflib" REGULAR_EXPRESSION "include/dbflib/.*$")
source_group("Header Files\\dom" REGULAR_EXPRESSION "include/dom/.*$")
source_group("Header Files\\elev" REGULAR_EXPRESSION "include/elev/.*$")
source_group("Header Files\\flt" REGULAR_EXPRESSION "include/flt/.*$")
source_group("Header Files\\gltf" REGULAR_EXPRESSION "include/gltf/.*$")
source_group("Header Files\\ip" REGULAR_EXPRESSION "include/ip/.*$")
source_group("Header Files\\lmfit" REGULAR_EXPRESSION "include/lmfit/.*$")
source_group("Header Files\\rapidjson" REGULAR_EXPRESSION "include/rapidjson/.*$")
source_group("Header Files\\scenegraph" REGULAR_EXPRESSION "include/scenegraph/.*$")
source_group("Header Files\\sfa" REGULAR_EXPRESSION "include/sfa/.*$")
source_group("Header Files\\tg" REGULAR_EXPRESSION "include/tg/.*$")
source_group("Header Files\\ogr" REGULAR_EXPRESSION "include/ogr/.*$")
source_group("Source Files\\ccl" REGULAR_EXPRESSION "src/ccl/.*$")
source_group("Source Files\\cdb_tile" REGULAR_EXPRESSION "src/cdb_tile/.*$")
source_group("Source Files\\cdb_util" REGULAR_EXPRESSION "src/cdb_util/.*$")
source_group("Source Files\\civetweb" REGULAR_EXPRESSION "src/civetweb/.*$")
source_group("Source Files\\ctl" REGULAR_EXPRESSION "src/ctl/.*$")
source_group("Source Files\\cts" REGULAR_EXPRESSION "src/cts/.*$")
source_group("Source Files\\dbflib" REGULAR_EXPRESSION "src/dbflib/.*$")
source_group("Source Files\\dom" REGULAR_EXPRESSION "src/dom/.*$")
source_group("Source Files\\elev" REGULAR_EXPRESSION "src/elev/.*$")
source_group("Source Files\\flt" REGULAR_EXPRESSION "src/flt/.*$")
source_group("Source Files\\gltf" REGULAR_EXPRESSION "src/gltf/.*$")
source_group("Source Files\\ip" REGULAR_EXPRESSION "src/ip/.*$")
source_group("Source Files\\lmfit" REGULAR_EXPRESSION "src/lmfit/.*$")
source_group("Source Files\\rapidjson" REGULAR_EXPRESSION "src/rapidjson/.*$")
source_group("Source Files\\scenegraph" REGULAR_EXPRESSION "src/scenegraph/.*$")
source_group("Source Files\\sfa" REGULAR_EXPRESSION "src/sfa/.*$")
source_group("Source Files\\tg" REGULAR_EXPRESSION "src/tg/.*$")
source_group("Source Files\\ogr" REGULAR_EXPRESSION "src/ogr/.*$")
link_libraries(cogcore)


set (LM2OBJ_SOURCES ./lmbundle2obj/lmbundle2obj.cpp)
SET(OBJ_EXT_SOURCES ./obj-preprocess/obj-preprocess.cpp
                    ./obj-preprocess/obj-preprocess.h
		    ./obj-preprocess/meshrender.cpp
		    ./obj-preprocess/MeshRender.h)

add_executable(lmbundle2obj ${LM2OBJ_SOURCES})



if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
if(FALSE)
    target_link_libraries(obj-extract 
        debug "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgd.lib"
        optimized "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osg.lib")

    target_link_libraries(obj-extract 
        debug "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgGAd.lib"
        optimized "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgGA.lib")

    target_link_libraries(obj-extract 
        debug "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgDBd.lib"
        optimized "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgDB.lib")

    target_link_libraries(obj-extract 
        debug "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib//osgUtild.lib"
        optimized "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/osgUtil.lib")

    target_link_libraries(obj-extract 
        debug "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib//OpenThreadsd.lib"
        optimized "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/OpenThreads.lib")
endif(FALSE)
endif(WIN32)

add_executable(meshgen meshgen/meshgen.cpp)
if(WIN32)
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/libcurl.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/lpng154/lib/libpng15.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
    target_link_libraries(meshgen "${THIRD_PARTY_DIR}/fbxsdk/lib/vs2015/x64/${CMAKE_BUILD_TYPE}/libfbxsdk.lib")
endif(WIN32)
if(UNIX)
    target_link_libraries(meshgen ${CMAKE_DL_LIBS})
    target_link_libraries(meshgen "pthread")
    target_link_libraries(meshgen "libgdal.so")
    target_link_libraries(meshgen "libcurl.so")
    target_link_libraries(meshgen "libpng15.so")
    target_link_libraries(meshgen "libjpeg.so")
    target_link_libraries(meshgen "libippi.a")
    target_link_libraries(meshgen "libipps.a")
    target_link_libraries(meshgen "libippcore.a")
    target_link_libraries(meshgen "libfbxsdk.a")
endif(UNIX)

add_executable(cdb-service cdb-service/cdb-service.cpp)

if(WIN32)
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/lpng154/lib/libpng15.lib")
    target_link_libraries(cdb-service "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
endif(WIN32)
if(UNIX)
    target_link_libraries(cdb-service ${CMAKE_DL_LIBS})
    target_link_libraries(cdb-service "pthread")
    target_link_libraries(cdb-service "libgdal.so")
    target_link_libraries(cdb-service "libippi.a")
    target_link_libraries(cdb-service "libipps.a")
    target_link_libraries(cdb-service "libippcore.a")
    target_link_libraries(cdb-service "libpng15.so")
    target_link_libraries(cdb-service "libjpeg.so")
endif(UNIX)

add_executable(cdb cdb/cdb.cpp)
target_compile_definitions(cdb PRIVATE -DBUILDTS="${BUILDTS}")
if(WIN32)
    target_link_libraries(cdb "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    target_link_libraries(cdb "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
    target_link_libraries(cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    target_link_libraries(cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    target_link_libraries(cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
endif(WIN32)
if(UNIX)
    target_link_libraries(cdb ${CMAKE_DL_LIBS})
    target_link_libraries(cdb "pthread")
    target_link_libraries(cdb "libgdal.so")
    target_link_libraries(cdb "libjpeg.so")
#    target_link_libraries(cdb "libippi.a")
#    target_link_libraries(cdb "libipps.a")
        target_link_libraries(cdb "libcurl.so")
        target_link_libraries(cdb "libgdal.so")
        target_link_libraries(cdb "libpng15.so")
        target_link_libraries(cdb "libjpeg.so")
        target_link_libraries(cdb "libippcore.so")
       target_link_libraries(cdb "libippvm.so")
       target_link_libraries(cdb "libipps.so")
       target_link_libraries(cdb "libippi.so")
endif(UNIX)


################################################################################


if(COG_BUILD_GL_TOOLS)
    add_definitions("-DCOG_USE_GL")
    if(WIN32)
        include_directories("${THIRD_PARTY_DIR}/glew/${CMAKE_BUILD_TYPE}/include")
        include_directories("${THIRD_PARTY_DIR}/glut/include")
    endif(WIN32)
    if(UNIX)
        include_directories("${THIRD_PARTY_DIR}/glew/linux_x64/include")
        include_directories("${THIRD_PARTY_DIR}/glut/linux_x64/include")
    endif(UNIX)

SET(OBJ_EXT_SOURCES ./obj-extract/obj-extract.cpp
                    ./obj-extract/obj-extract.h)

add_executable(obj-extract ${OBJ_EXT_SOURCES})
if(WIN32)
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/lpng154/lib/libpng15.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/glew/release/lib/glew32.lib")
    target_link_libraries(obj-extract "${THIRD_PARTY_DIR}/glut/lib/freeglut.lib")
    target_link_libraries(obj-extract "opengl32.lib")
    
endif(WIN32)
if(UNIX)
    target_link_libraries(obj-extract ${CMAKE_DL_LIBS})
    target_link_libraries(obj-extract "pthread")
    target_link_libraries(obj-extract "libgdal.so")
    target_link_libraries(obj-extract "libippi.a")
    target_link_libraries(obj-extract "libipps.a")
    target_link_libraries(obj-extract "libippcore.a")
    target_link_libraries(obj-extract "libpng15.so")
    target_link_libraries(obj-extract "libjpeg.so")
    target_link_libraries(obj-extract "libGLEW.a")
    target_link_libraries(obj-extract "libglut.so")

endif(UNIX)

    add_executable(mesh2cdb
#		mesh2cdb/quickobj.h
#		mesh2cdb/quickobj_lmb.cpp
        mesh2cdb/mesh2cdb.cpp
        mesh2cdb/mesh2cdb.h
        mesh2cdb/meshconvert.cpp
        mesh2cdb/MeshRender.h
        mesh2cdb/meshrender.cpp
        src/scenegraph_gl/scenegraph_gl.cpp
        )
    if(WIN32)
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/libcurl.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/zlib128.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/lpng154/lib/libpng15_static.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippvm.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/fbxsdk/lib/vs2015/x64/${CMAKE_BUILD_TYPE}/libfbxsdk.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/glew/release/lib/glew32.lib")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/glut/lib/freeglut.lib")
    endif(WIN32)
    if(UNIX)
        target_link_libraries(mesh2cdb ${CMAKE_DL_LIBS})
        target_link_libraries(mesh2cdb "pthread")
        target_link_libraries(mesh2cdb "libEGL.so")
        target_link_libraries(mesh2cdb "libGLX.so")
        target_link_libraries(mesh2cdb "OpenGL.so")
        target_link_libraries(mesh2cdb "GLU")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/fbxsdk/lib/gcc4/x64/release/libfbxsdk.a")
        target_link_libraries(mesh2cdb "libcurl.so")
        target_link_libraries(mesh2cdb "libgdal.so")
        target_link_libraries(mesh2cdb "libpng15.so")
        target_link_libraries(mesh2cdb "libjpeg.so")
        target_link_libraries(mesh2cdb "libippcore.so")
        target_link_libraries(mesh2cdb "libippvm.so")
        target_link_libraries(mesh2cdb "libipps.so")
        target_link_libraries(mesh2cdb "libippi.so")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/glew/linux_x64/lib/libGLEW.a")
        target_link_libraries(mesh2cdb "${THIRD_PARTY_DIR}/glut/linux_x64/lib/libglut.so")
        #install(TARGETS mesh2cdb DESTINATION bin)
    endif(UNIX)

    add_executable(mesh2cdb-rest
        mesh2cdb/mesh2cdb-rest.cpp
        mesh2cdb/mesh2cdb.h
        mesh2cdb/meshconvert.cpp
        mesh2cdb/MeshRender.h
        mesh2cdb/meshrender.cpp
        src/scenegraph_gl/scenegraph_gl.cpp
        )
    if(WIN32)
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/libcurl.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/zlib128.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/lpng154/lib/libpng15_static.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/jpeg-8c/lib/jpg8-c.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippvm.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/fbxsdk/lib/vs2015/x64/${CMAKE_BUILD_TYPE}/libfbxsdk.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/glew/release/lib/glew32.lib")
        target_link_libraries(mesh2cdb-rest "${THIRD_PARTY_DIR}/glut/lib/freeglut.lib")
    endif(WIN32)
    if(UNIX)
        target_link_libraries(mesh2cdb-rest ${CMAKE_DL_LIBS})
        target_link_libraries(mesh2cdb-rest "pthread")
        target_link_libraries(mesh2cdb-rest "libEGL.so")
        target_link_libraries(mesh2cdb-rest "libGLX.so")
        target_link_libraries(mesh2cdb-rest "OpenGL.so")
        target_link_libraries(mesh2cdb-rest "GLU")
        target_link_libraries(mesh2cdb-rest "libfbxsdk.a")
        target_link_libraries(mesh2cdb-rest "libcurl.so")
        target_link_libraries(mesh2cdb-rest "libgdal.so")
        target_link_libraries(mesh2cdb-rest "libpng15.so")
        target_link_libraries(mesh2cdb-rest "libjpeg.so")
        target_link_libraries(mesh2cdb-rest "libippcore.so")
        target_link_libraries(mesh2cdb-rest "libippvm.so")
        target_link_libraries(mesh2cdb-rest "libipps.so")
        target_link_libraries(mesh2cdb-rest "libippi.so")
        target_link_libraries(mesh2cdb-rest "libGLEW.a")
        target_link_libraries(mesh2cdb-rest "libglut.so")
        #install(TARGETS mesh2cdb-rest DESTINATION bin)
    endif(UNIX)

endif(COG_BUILD_GL_TOOLS)

if(CMAKE_CUDA_COMPILER)
    add_library(cogcuda ${COGCUDA_SOURCES} ${COGCUDA_HEADERS})
    set_target_properties(cogcuda PROPERTIES CUDA_SEPERABLE_COMPILATION ON)
    #add_executable(cdb-elev cdb-elev/cdb-elev.cpp)
    #target_link_libraries(cdb-elev cogcuda)
    #if(WIN32)
    #    target_link_libraries(cdb-elev "${THIRD_PARTY_DIR}/gdal-cdb/${CMAKE_BUILD_TYPE}/lib/gdal_i.lib")
    #    target_link_libraries(cdb-elev "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippi.lib")
    #    target_link_libraries(cdb-elev "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ipps.lib")
    #    target_link_libraries(cdb-elev "${THIRD_PARTY_DIR}/ipp2019/lib/intel64_win/ippcore.lib")
    #endif(WIN32)
    #if(UNIX)
    #    target_link_libraries(cdb-elev ${CMAKE_DL_LIBS})
    #    target_link_libraries(cdb-elev "pthread")
    #    target_link_libraries(cdb-elev "libgdal.so")
    #    target_link_libraries(cdb-elev "libippi.a")
    #    target_link_libraries(cdb-elev "libipps.a")
    #    target_link_libraries(cdb-elev "libippcore.a")
    #endif(UNIX)
endif(CMAKE_CUDA_COMPILER)


if(UNIX)
    install(TARGETS meshgen DESTINATION bin)
    install(TARGETS cdb DESTINATION bin)
    install(TARGETS cdb-service  DESTINATION bin)
  
    #install(DIRECTORY ${THIRD_PARTY_DIR}/ipp2019_linux_x64/lib/intel64_lin/ DESTINATION lib USE_SOURCE_PERMISSIONS FILES_MATCHING PATTERN "*.so" PATTERN "*.a")
    #install(DIRECTORY ${THIRD_PARTY_DIR}/linux_x64/ DESTINATION . USE_SOURCE_PERMISSIONS)
    #install(DIRECTORY ${THIRD_PARTY_DIR}/glew/linux_x64/lib/ DESTINATION lib USE_SOURCE_PERMISSIONS FILES_MATCHING PATTERN "*.so" PATTERN "*.a")
endif(UNIX)


