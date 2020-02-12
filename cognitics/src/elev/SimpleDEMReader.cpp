
#include "elev/SimpleDEMReader.h"
#include "gdalwarper.h"

namespace elev {
    SimpleDEMReader::~SimpleDEMReader()
    {
        if(gdal_dataset)
            GDALClose((GDALDatasetH)gdal_dataset);
    }
    
    SimpleDEMReader::SimpleDEMReader(const std::string &filename, 
        OGRSpatialReference destinationSRS, int windowTop, int windowBottom, int windowRight, int windowLeft, float scale)
    {
        //Bear in mind that the windowTop=0 is row 0 in the raster, not the top row geographically
        //since the DEM is inverted spatially from the row
        windowedMode = true;
        this->windowTop = windowTop;
        this->windowBottom = windowBottom;
        this->windowRight = windowRight;
        this->windowLeft = windowLeft;
        this->scaleFactor = scale;
        height = 0;
        width = 0;
        depth = 0;
        log.init("SimpleDEMReader", this);
        this->filename = filename;
        app_srs = NULL;
        file_srs = NULL;
    }

    SimpleDEMReader::SimpleDEMReader(const std::string &filename, OGRSpatialReference destinationSRS, float scale)
    {
        windowedMode = false;
        height = 0;
        width = 0;
        depth = 0;
        log.init("SimpleDEMReader", this);
        this->filename = filename;
        this->scaleFactor = scale;
        app_srs = NULL;
        file_srs = NULL;
}


#if 0
    bool SimpleDEMReader::Open()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "Open()" << log.endl;
#endif
        gdal_dataset = (GDALDataset *)GDALOpen(this->filename.c_str(), GA_ReadOnly);
        if (!gdal_dataset)
            return false;

        if (gdal_dataset->GetProjectionRef() == NULL)
        {
            log << ccl::LERR << "Open(): GetProjectionRef() failed." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        width = gdal_dataset->GetRasterXSize();
        height = gdal_dataset->GetRasterYSize();
        depth = gdal_dataset->GetRasterCount();
#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): width = " << width << log.endl;
        log << ccl::LDEBUG << "Open(): height = " << height << log.endl;
        log << ccl::LDEBUG << "Open(): depth = " << depth << log.endl;
#endif

        char *wkt = new char[8192];
        strncpy(wkt, gdal_dataset->GetProjectionRef(), 8192);
        wkt[8191] = 0;
        if (!strlen(wkt))
        {
            std::string prj_filename = filename.substr(0, filename.size() - 3) + "prj";
            FILE *PRJ = fopen(prj_filename.c_str(), "r");
            if (PRJ)
            {
                fgets(wkt, 8192, PRJ);
                fclose(PRJ);
            }
        }
        wkt[8191] = 0;

        if (!strlen(wkt))
        {
            OGRSpatialReference oSRS;
            oSRS.SetWellKnownGeogCS("WGS84");
            char *wktp = NULL;
            oSRS.exportToWkt(&wktp);
            strncpy(wkt, wktp, 8191);
            CPLFree(wktp);
            log << ccl::LERR << "Open(): missing projection in " << filename << " assuming geographic." << log.endl;

        }
        if (!strlen(wkt))
        {
            log << ccl::LERR << "Open(): missing projection wkt." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            delete[] wkt;
            return false;
        }

        char *pwkt = wkt;
        file_srs = new OGRSpatialReference;
        OGRErr err = file_srs->importFromWkt((char **)&wkt);
        delete[] pwkt;
        if (err != OGRERR_NONE)
        {
            log << ccl::LERR << "Open(): unable to parse projection information." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        double geotransform[6];
        if (gdal_dataset->GetGeoTransform(geotransform) != CE_None)
        {
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        this->BuildCoordinateTransformations();

        spacing_x = geotransform[1];
        bound_x_low = geotransform[0];
        rotation_x = geotransform[2];

        spacing_y = geotransform[5];
        bound_y_low = geotransform[3];
        rotation_y = geotransform[4];

        // a post is in the middle of the space, regardless of area_or_point
        bound_x_low = bound_x_low + (spacing_x / 2);
        bound_y_low = bound_y_low + (spacing_y / 2);
        bound_x_high = bound_x_low + (geotransform[1] * (width - 1));
        bound_y_high = bound_y_low + (geotransform[5] * (height - 1));

        // make geographic versions of the bounds
        geo_bound_x_low = bound_x_low;
        geo_bound_y_low = bound_y_low;
        geo_bound_x_high = bound_x_high;
        geo_bound_y_high = bound_y_high;
        if (app_ct)
        {
            app_ct->Transform(1, &geo_bound_x_low, &geo_bound_y_low);
            app_ct->Transform(1, &geo_bound_x_high, &geo_bound_y_high);
        }

#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): geotransform = [" << geotransform[0] << ", "
            << geotransform[1] << ", " << geotransform[2] << ", "
            << geotransform[3] << ", " << geotransform[4] << ", "
            << geotransform[5] << ", " << "]" << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_x = " << spacing_x << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_y = " << spacing_y << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_low = " << bound_x_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_high = " << bound_x_high << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_low = " << bound_y_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_high = " << bound_y_high << log.endl;
#endif

        return true;
    }
#endif


    bool SimpleDEMReader::Open()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "Open()" << log.endl;
#endif
        gdal_dataset = (GDALDataset *)GDALOpen(this->filename.c_str(), GA_ReadOnly);
        if (!gdal_dataset)
            return false;

        if (gdal_dataset->GetProjectionRef() == NULL)
        {
            log << ccl::LERR << "Open(): GetProjectionRef() failed." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        width = gdal_dataset->GetRasterXSize();
        height = gdal_dataset->GetRasterYSize();
        if (windowedMode)
        {
            windowHeight = windowBottom - windowTop;
            windowWidth = windowRight - windowLeft;
#ifdef DEBUG
            log << ccl::LDEBUG << "Open(): window width = " << windowWidth << log.endl;
            log << ccl::LDEBUG << "Open(): window height = " << windowHeight << log.endl;
#endif
        }
        else
        {
#ifdef DEBUG
            log << ccl::LDEBUG << "Open(): width = " << width << log.endl;
            log << ccl::LDEBUG << "Open(): height = " << height << log.endl;
#endif
        }
        depth = gdal_dataset->GetRasterCount();
#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): depth = " << depth << log.endl;
#endif

        char *wkt = new char[8192];
        strncpy(wkt, gdal_dataset->GetProjectionRef(), 8192);
        wkt[8191] = 0;
        if (!strlen(wkt))
        {
            std::string prj_filename = filename.substr(0, filename.size() - 3) + "prj";
            FILE *PRJ = fopen(prj_filename.c_str(), "r");
            if (PRJ)
            {
                fgets(wkt, 8192, PRJ);
                fclose(PRJ);
            }
        }
        wkt[8191] = 0;

        if (!strlen(wkt))
        {
            OGRSpatialReference oSRS;
            oSRS.SetWellKnownGeogCS("WGS84");
            char *wktp = NULL;
            oSRS.exportToWkt(&wktp);
            strncpy(wkt, wktp, 8191);
            CPLFree(wktp);
            log << ccl::LERR << "Open(): missing projection in " << filename << " assuming geographic." << log.endl;

        }
        if (!strlen(wkt))
        {
            log << ccl::LERR << "Open(): missing projection wkt." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            delete[] wkt;
            return false;
        }

        char *pwkt = wkt;
        file_srs = new OGRSpatialReference;
        OGRErr err = file_srs->importFromWkt((char **)&wkt);
        delete[] pwkt;
        if (err != OGRERR_NONE)
        {
            log << ccl::LERR << "Open(): unable to parse projection information." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        double geotransform[6];
        if (gdal_dataset->GetGeoTransform(geotransform) != CE_None)
        {
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        this->BuildCoordinateTransformations();

		//skarasevich - if image is rotated, spacing will be 0 resulting in bounds generating incorrectly
		//https://gis.stackexchange.com/questions/281132/why-doesnt-gdalinfo-report-pixel-size
		if (geotransform[1] == 0.0 && geotransform[5] == 0.0)
		{
			geotransform[1] = sqrt(geotransform[1] * geotransform[1] + geotransform[4] * geotransform[4]);
			geotransform[5] = sqrt(geotransform[2] * geotransform[2] + geotransform[5] * geotransform[5]);
		}

        spacing_x = geotransform[1];
        bound_x_low = geotransform[0];
        rotation_x = geotransform[2];

        spacing_y = geotransform[5];
        bound_y_low = geotransform[3];
        rotation_y = geotransform[4];


        // a post is in the middle of the space, regardless of area_or_point
        bound_x_low = bound_x_low + (spacing_x / 2);
        bound_y_low = bound_y_low + (spacing_y / 2);
        bound_x_high = bound_x_low + (geotransform[1] * (width - 1));
        bound_y_high = bound_y_low + (geotransform[5] * (height - 1));

        // make geographic versions of the bounds
        geo_bound_x_low = bound_x_low;
        geo_bound_y_low = bound_y_low;
        geo_bound_x_high = bound_x_high;
        geo_bound_y_high = bound_y_high;
        if (app_ct)
        {
            app_ct->Transform(1, &geo_bound_x_low, &geo_bound_y_low);
            app_ct->Transform(1, &geo_bound_x_high, &geo_bound_y_high);
        }

#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): geotransform = [" << geotransform[0] << ", "
            << geotransform[1] << ", " << geotransform[2] << ", "
            << geotransform[3] << ", " << geotransform[4] << ", "
            << geotransform[5] << ", " << "]" << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_x = " << spacing_x << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_y = " << spacing_y << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_low = " << bound_x_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_high = " << bound_x_high << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_low = " << bound_y_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_high = " << bound_y_high << log.endl;
#endif

        return true;
    }



    int SimpleDEMReader::getWidth() const
    {
        if (windowedMode)
            return windowWidth;
        return width;
    }

    int SimpleDEMReader::getHeight() const
    {
        if (windowedMode)
            return windowHeight;
        return height;
    }
    int SimpleDEMReader::getScaledWidth() const
    {
        return ceil(getHeight() * scaleFactor);
    }
    int SimpleDEMReader::getScaledHeight() const
    {
        return ceil(getHeight() * scaleFactor);
    }
    void SimpleDEMReader::getMBR(double &north, double &south, double &east, double &west)
    {
        if (windowedMode)
        {
            west = geo_bound_x_low + (windowLeft * spacing_x);
            east = geo_bound_x_low + (windowRight * spacing_x);
            north = geo_bound_y_low + (windowTop * spacing_y);
            south = geo_bound_y_low + (windowBottom * spacing_y);
        }
        else
        {
            north = geo_bound_y_high;
            south = geo_bound_y_low;
            east = geo_bound_x_high;
            west = geo_bound_x_low;
        }
        return;
    }
    // Convert each post to a double value and return it as an array
    bool SimpleDEMReader::getGrid(std::vector<double> &grid)
    {
        int offsetX = 0;
        int offsetY = 0;
        int readWidth = width;
        int readHeight = height;
        if (windowedMode)
        {
            offsetX = windowLeft;
            offsetY = windowTop;
            readWidth = windowRight - windowLeft;
            readHeight = windowBottom - windowTop;
        }

        int scaledReadWidth = getScaledWidth();
        int scaledReadHeight = getScaledHeight();
        int len = scaledReadWidth * scaledReadHeight;
        double *fgrid = new double[len];
        GDALRasterBand *poBand = gdal_dataset->GetRasterBand(1);

        CPLErr error = poBand->RasterIO(GF_Read, offsetX, offsetY, readWidth, readHeight,
            fgrid, scaledReadWidth, scaledReadHeight, GDT_Float64,
            0, 0);
        if (error != CPLErr::CE_None)
        {
            std::cout << "RasterIO Error" << std::endl;
        }

        grid.resize(len);
        for (int i = 0; i < len; i++)
        {
            grid[i] = fgrid[i];
        }
        delete fgrid;
        return true;
    }

    //!< Needed because of https://github.com/OSGeo/proj.4/issues/226 (deadlock/crash because of setlocal not being threadsafe)
    static ccl::mutex transformMutex;
    void SimpleDEMReader::BuildCoordinateTransformations()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "BuildCoordinateTransformations()" << log.endl;
#endif
        file_ct = NULL;
        app_ct = NULL;

        // Using the C handle functions to avoid DLL hell with release/debug versions
        if (app_srs)
            OSRDestroySpatialReference(OGRSpatialReferenceH(app_srs));
        app_srs = (OGRSpatialReference *)OSRNewSpatialReference(NULL);;
        OGRErr err = app_srs->SetFromUserInput("WGS84");
        if (err)
            throw std::runtime_error("invalid reference type");
        if (!file_srs)
        {
            file_srs = app_srs; //throw std::runtime_error("missing file spacial reference");
            return;
        }

        // we don't need transforms if the file is geographic
        if (file_srs->IsGeographic())
            return;

        transformMutex.lock();
        file_ct = OGRCreateCoordinateTransformation(app_srs, file_srs);
        app_ct = OGRCreateCoordinateTransformation(file_srs, app_srs);
        transformMutex.unlock();
    }

}