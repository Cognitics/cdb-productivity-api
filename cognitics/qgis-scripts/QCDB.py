import math
from PyQt5.QtCore import QCoreApplication
from processing.algs.qgis.QgisAlgorithm import QgisAlgorithm
from qgis.PyQt.QtGui import QImage, QPainter, QColor
from qgis.PyQt.QtCore import QSize
from qgis.core import ( QgsProcessing,
                        QgsFeatureSink,
                        QgsProcessingException,
                        QgsProcessingAlgorithm,
                        QgsRectangle,
                        QgsMapSettings,
                        QgsMapRendererCustomPainterJob,
                        QgsMapRendererSequentialJob,
                        QgsProcessingParameterFeatureSource,
                        QgsProcessingParameterFeatureSink,
                        QgsProcessingParameterEnum,
                        QgsProcessingParameterExtent,
                        QgsProcessingParameterMultipleLayers,
                        QgsProcessingParameterString,
                        QgsProcessingParameterNumber,
                        QgsProcessingParameterBoolean,
                        QgsProcessingParameterMapLayer,
                        QgsProcessingParameterRasterDestination,
                        QgsRasterFileWriter,
                        QgsProject,
                        QgsRasterLayer,
                        QgsMapLayer,
                        QgsLayerTreeNode,
                       )
import processing
import qgis
import osgeo.gdal
import os
import tempfile
import subprocess
import select


class QCDBGenerator(QgsProcessingAlgorithm):

    OUTPUTDIR = "OUTPUTDIR"
    TOOL_PATHFILE = "TOOL_PATHFILE"
    IMGLOD = "IMGLOD"
    ELEVLOD = "ELEVLOD"
    ATTRDM = "ATTRDM"

    def tr(self, string):
        """
        Returns a translatable string with the self.tr() function.
        """
        return QCoreApplication.translate('Processing', string)

    def createInstance(self):
        return QCDBGenerator()

    def name(self):
        """
        Returns the algorithm name, used for identifying the algorithm. This
        string should be fixed for the algorithm, and must not be localised.
        The name should be unique within each provider. Names should contain
        lowercase alphanumeric characters only and no spaces or other
        formatting characters.
        """
        return 'QCDBGenerator'

    def displayName(self):
        """
        Returns the translated algorithm name, which should be used for any
        user-visible display of the algorithm name.
        """
        return self.tr('QGIS CDB Generator')

    def group(self):
        """
        Returns the name of the group this algorithm belongs to. This string
        should be localised.
        """
        return self.tr('CDB')

    def groupId(self):
        """
        Returns the unique ID of the group this algorithm belongs to. This
        string should be fixed for the algorithm, and must not be localised.
        The group id should be unique within each provider. Group id should
        contain lowercase alphanumeric characters only and no spaces or other
        formatting characters.
        """
        return 'cdb_generator'

    def shortHelpString(self):
        """
        Returns a localised short helper string for the algorithm. This string
        should provide a basic description about what the algorithm does and the
        parameters and outputs associated with it..
        """
        return self.tr("Adds the layers in this project to a new or existing CDB datastore")

    def initAlgorithm(self, config=None):
        """
        Here we define the inputs and output of the algorithm, along
        with some other properties.
        """
        # List of names in order.
        
        self.addParameter(
            QgsProcessingParameterString (
                name = self.OUTPUTDIR,
                description = self.tr('CDB Output Directory'),
                defaultValue=r'j:\output\qgis_cdb',
                optional=False))

        self.availableTranslators = ["GGDM", "OpenStreetMaps", "SE Core"]
        self.attributeTranslatorParameters = QgsProcessingParameterEnum(
            name = self.ATTRDM,
            description = self.tr("Attribute Translation Ruleset"),
            options = self.availableTranslators,
            defaultValue = 0,
            allowMultiple = False,
            optional=False)

        self.addParameter(self.attributeTranslatorParameters)

        self.imgLODList = [];
        for i in range(-10,20):
            self.imgLODList.append(str(i))

        self.imageryLODParameters = QgsProcessingParameterEnum(
            name = self.IMGLOD,
            description = self.tr("Imagery LOD"),
            options = self.imgLODList,
            defaultValue = 16,
            allowMultiple = False,
            optional=False)
        self.addParameter(self.imageryLODParameters)

        self.elevationLODParameters = QgsProcessingParameterEnum(
            name = self.ELEVLOD,
            description = self.tr("Elevation LOD"),
            options = self.imgLODList,# Share the LOD list with imagery
            defaultValue = 16,
            allowMultiple = False,
            optional=False)
        self.addParameter(self.elevationLODParameters)

        self.addParameter(
            QgsProcessingParameterString (
                name = self.TOOL_PATHFILE,
                description = self.tr('CDB Tool application directory'),
                defaultValue=r'C:\Program Files\CDB Productivity Suite\bin',
                optional=False))


    def chomp(self, x):
        if x.endswith("\r\n"): 
            return x[:-2]
        if x.endswith("\n") or x.endswith("\r"): 
            return x[:-1]
        return x

    def processAlgorithm(self, parameters, context, feedback):
        toolsPath = self.parameterAsString(
            parameters,
            self.TOOL_PATHFILE,
            context
        )

        cdbPath = self.parameterAsString(
            parameters,
            self.OUTPUTDIR,
            context
        )

        cdbInsertExePath = os.path.join(toolsPath,"cdb-inject.exe")
        gdaladdoPath = os.path.join(toolsPath,"gdaladdo.exe")
        cdbImageryLayerURI = "CDB:" + cdbPath + ":Imagery_Yearly"

        feedback.setProgressText("Using " + cdbInsertExePath)

        layers = QgsProject.instance().mapLayers()
        for layerId,layer in layers.items():
            layerName = layer.name()
            layerFilename = layer.source()
            if(not os.path.exists(layerFilename)):
                feedback.setProgressText("Skipping layer " + layerName + " because file " + layerFilename + " does not exist.")
                continue
            
            feedback.setProgressText("Processing " + layerFilename)
            
            if(layer.type()==QgsMapLayer.RasterLayer):
                if(layer.bandCount()==1):
                    feedback.setProgressText("Processing elevation file " + layerFilename)
                    #elevation                    
                elif(layer.bandCount()==3 or layer.bandCount()==4):
                    #imagery
                    feedback.setProgressText("Processing imagery file " + layerFilename)
                    
                    args = [cdbInsertExePath,"-skip-overviews", "-imagery", layerFilename, cdbPath ]
                    #p = subprocess.Popen(args,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
                    try:
                        #p = subprocess.check_output(args,stderr=subprocess.PIPE)
                        p = subprocess.Popen(args,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
                        for line in p.stdout:
                            line = self.chomp(str(line, "windows-1252"))
                            if(len(line)==0):
                                continue
                            else:
                                feedback.setProgressText(line)
                    except subprocess.CalledProcessError as error:
                        feedback.setProgressText(str(error.output))
                        feedback.setProgressText(str(error.returncode))
                        feedback.setProgressText(str(error.cmd))


        #build overviews
        feedback.setProgressText("Building LODs...")
        args = [gdaladdoPath,"--config", "LODMIN", "0", "--config", "LODMAX", "8", cdbImageryLayerURI]
        p = subprocess.Popen(args,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
        for line in p.stdout:
            line = self.chomp(str(line, "windows-1252"))
            if(len(line)==0):
                continue
            else:
                feedback.setProgressText(line)
        return {}