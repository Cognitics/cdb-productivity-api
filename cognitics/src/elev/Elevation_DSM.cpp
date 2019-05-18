/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#include <float.h>
#include "elev/Elevation_DSM.h"

#include <ccl/Profile.h>

using namespace ccl;
namespace elev
{
    namespace
    {
        size_t get_num_posts_for_strategy(elev::elevation_strategy strategy)
        {
            if(strategy == ELEVATION_NEAREST)
                return 1;
            if(strategy == ELEVATION_LINEAR)
                return 2;
            if(strategy == ELEVATION_TIN)
                return 3;
            if(strategy == ELEVATION_PLANAR)
                return 3;
            return 4;
        }

        std::vector<DataSource_Raster *> get_datasources_for_posts(const std::vector<DataPost *> &posts)
        {
            std::vector<DataSource_Raster *> result;
            for(size_t i = 0, c = posts.size(); i < c; ++i)
            {
                DataPost *post = posts.at(i);
                result.push_back(post->source);
            }
            std::sort(result.begin(), result.end());
            std::vector<DataSource_Raster *>::iterator end_it = std::unique(result.begin(), result.end());
            result.resize(std::distance(result.begin(), end_it));
            return result;
        }

        int get_highest_priority(const std::vector<DataSource_Raster *> &sources)
        {
            int result = INT_MIN;
            for(size_t i = 0, c = sources.size(); i < c; ++i)
                result = std::max<int>(result, sources[i]->priority);
            return result;
        }

        std::vector<DataSource_Raster *> filter_by_priority(const std::vector<DataSource_Raster *> &sources, int priority)
        {
            std::vector<DataSource_Raster *> result;
            for(size_t i = 0, c = sources.size(); i < c; ++i)
            {
                if(sources[i]->priority == priority)
                    result.push_back(sources[i]);
            }
            return result;
        }

        double get_lowest_post_spacing(const std::vector<DataSource_Raster *> &sources)
        {
            double result = DBL_MAX;
            for(size_t i = 0, c = sources.size(); i < c; ++i)
                result = std::min<double>(result, sources[i]->GetAveragePostSpacing());
            return result;
        }

        std::vector<DataSource_Raster *> filter_by_post_spacing(const std::vector<DataSource_Raster *> &sources, double post_spacing)
        {
            const double epsilon = 1e-8;
            std::vector<DataSource_Raster *> result;
            for(size_t i = 0, c = sources.size(); i < c; ++i)
            {
                if(sources[i]->GetAveragePostSpacing() <= post_spacing + epsilon)
                    result.push_back(sources[i]);
            }
            return result;
        }



        std::map<DataSource_Raster *, std::vector<DataPost *> > map_posts_to_datasources(const std::vector<DataPost *> &posts)
        {
            std::map<DataSource_Raster *, std::vector<DataPost *> > result;
            for(size_t i = 0, c = posts.size(); i < c; ++i)
                result[posts.at(i)->source].push_back(posts.at(i));
            return result;
        }

        DataSource_Raster *find_best_datasource_with_desired_posts(const std::map<DataSource_Raster *, std::vector<DataPost *> > &posts_by_datasource, size_t desired_posts)
        {
            DataSource_Raster *result = NULL;
            for(std::map<DataSource_Raster *, std::vector<DataPost *> >::const_iterator it = posts_by_datasource.begin(), end = posts_by_datasource.end(); it != end; ++it)
            {
                DataSource_Raster *ds = it->first;
                std::vector<DataPost *> posts = it->second;
                if(posts.size() < desired_posts)
                    continue;
                result = ds;
                break;
            }
            return result;
        }

    }

    Elevation_DSM::~Elevation_DSM()
    {
        for(size_t i = 0, c = debug_features.size(); i < c; ++i)
            delete debug_features.at(i);
    }
        
    bool Elevation_DSM::Load(sfa::Point *p)
    {
        double value;
        DataPost *dp = dynamic_cast<DataPost *>(p);

        if(dp && dp->GetValue(value))
        {
            dp->setZ(value);
            return true;
        }
        return false;
    }

    bool Elevation_DSM::Get(sfa::Point *p)
    {
        // Remove previous results.
        Clear();

        std::vector<DataPost *> posts;
        posts.reserve(16);

        if(dsm->GetPostsForPoint(p->X(), p->Y(), posts) == 0)
            return false;

        std::vector<DataSource_Raster *> post_sources = get_datasources_for_posts(posts);
        int highest_priority = get_highest_priority(post_sources);
        std::vector<DataSource_Raster *> highest_priority_sources = filter_by_priority(post_sources, highest_priority);
        double lowest_post_spacing = get_lowest_post_spacing(highest_priority_sources);
        std::vector<DataSource_Raster *> best_sources = filter_by_post_spacing(post_sources, lowest_post_spacing);

        std::map<DataSource_Raster *, std::vector<DataPost *> > posts_by_datasource = map_posts_to_datasources(posts);
        size_t desired_posts = get_num_posts_for_strategy(GetStrategy());
        DataSource_Raster *sole_source = find_best_datasource_with_desired_posts(posts_by_datasource, desired_posts);
        if(sole_source != NULL)
        {
            std::vector<DataPost *> sole_posts = posts_by_datasource[sole_source];
            for(size_t i = 0, c = posts.size(); i < c; ++i)
            {
                DataPost *post = posts.at(i);
                if(posts[i]->source == sole_source)
                    AddPoint(post);
                else
                    delete post;
            }
        }
        else
        {
            for(size_t i = 0, c = posts.size(); i < c; ++i)
                AddPoint(posts[i]);
        }

        bool result = Elevation::Get(p);

        if(dsm->generate_debug_features)
        {
            {
                sfa::Feature *feature = new sfa::Feature;
                feature->geometry = new sfa::Point(p);
                feature->attributes.setAttribute("result", result ? "true" : "false");
                feature->attributes.setAttribute("point", p->asText());
                for(size_t i = 0, c = points.size(); i < c; ++i)
                {
                    std::stringstream ss;
                    ss << "pt_" << i;
                    feature->attributes.setAttribute(ss.str(), points[i]->asText());
                }
                debug_features.push_back(feature);
            }

            for(size_t i = 0, c = posts.size(); i < c; ++i)
            {
                elev::DataPost *post = posts.at(i);
                if(post->source->debug_features.find(*post) != post->source->debug_features.end())
                    continue;
                sfa::Feature *feature = new sfa::Feature;
                feature->geometry = new sfa::Point(post);
                post->source->debug_features[post] = feature;
            }

        }

        return result;
    }

}

