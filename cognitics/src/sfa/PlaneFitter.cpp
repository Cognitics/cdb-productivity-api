/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
//#pragma optimize( "", off )
#include <boost/foreach.hpp>
#include "sfa/Point.h"
#include "lmfit/lmmin.h"
#include "sfa/PlaneFitter.h"

//#define PLANEFIT_DEBUG_OUTPUT

namespace sfa
{
    struct PlaneFitter::PlaneFitterData
    {
        lm_control_struct   control_struct;
        lm_status_struct    status_struct;
        Point plane[3];
        PointList points;
        std::vector<bool> constrainedIndices;
        bool forceBelow;
        typedef std::pair<int, int> intpair_t;
        //Pairs of indices into the point list
        //each pair should be the same Z (to keep roads flat for instance)
        std::vector<intpair_t> samePoints;

        //Interpolate Z from a plane
        bool interpolatePointInPlane(sfa::Point *pt, double &z)
        {
            plane[0].setZ(plane[0].Z() + plane[0].M());
            plane[1].setZ(plane[1].Z() + plane[1].M());
            plane[2].setZ(plane[2].Z() + plane[2].M());
            sfa::Point P = *pt - plane[0];
            sfa::Point U = plane[1] - plane[0];
            sfa::Point V = plane[2] - plane[0];
            double denom = V.X() * U.Y() - V.Y() * U.X();
            double v = (P.X() * U.Y() - P.Y() * U.X()) / denom;
            double u = (P.Y() * V.X() - P.X() * V.Y()) / denom;
            z = (plane[0].Z() + (u * U.Z()) + (v * V.Z()));
            if (z != z)
                return false;
            return true;

        }
    };

    void PlaneFitter::constrainPair(int a, int b)
    {
        intpair_t ipair;
        ipair.first = a;
        ipair.second = b;
        data->samePoints.push_back(ipair);
    }
    void PlaneFitter::constrainPoint(size_t idx)
    {
        data->constrainedIndices[idx] = true;
    }

    PlaneFitter::~PlaneFitter()
    {
        delete data;
    }

    PlaneFitter::PlaneFitter(const sfa::PointList points, bool forceBelowPoints)
    {
        log.init("PlaneFitter", this);
        validFit = false;
        data = new PlaneFitterData;
        data->control_struct = lm_control_double;
        //data->control_struct.printflags = 3;
        data->points = points;
        data->forceBelow = forceBelowPoints;
        data->constrainedIndices.resize(points.size());
        for (size_t i = 0, ic = points.size(); i < ic; i++)
        {
            data->constrainedIndices[i] = false;
        }
        if (points.size() > 0)
        {
            //Use the first point as the origin, and two other points to form a right triangle as the plane's basis
            data->plane[0] = *points.at(0);
            data->plane[0].clearM();
            data->plane[1] = data->plane[0];
            data->plane[1].setX(data->plane[1].X() + 10);
            data->plane[2] = data->plane[0];
            data->plane[2].setY(data->plane[0].Y() + 10);
        }
    }

    void PlaneFitter::setInitialBasis(sfa::Point pt, int idx)
    {
        if (idx < 0 || idx > 2)
            return;
        data->plane[idx] = pt;
    }

    bool PlaneFitter::Fit()
    {
        double par[3];
        if (data->points.size() < 3)
            return true;        

        // Use the first point's Z as the initial guess of a flat plane
        par[0] = data->plane[0].Z();
        par[1] = data->plane[1].Z();
        par[2] = data->plane[2].Z();

        data->status_struct.outcome = 0;
        data->control_struct.patience = 100;
        
        //the three parameters are our three Z values for the plane
        lmmin(3, par, data->points.size(), (void*)data, PlaneFitter::Callback, &data->control_struct, &data->status_struct);
        
        //If we ran out of patience, use the average of the constrained points.
        if (data->status_struct.outcome == 5)
        {
            int num_constrained = 0;
            double constrained_sum = 0;
            for (size_t i = 0, ic = data->points.size(); i < ic; i++)
            {
                if (data->constrainedIndices[i])
                {
                    num_constrained++;
                    constrained_sum += data->points[i]->Z();
                }
            }
            if (num_constrained)
            {
                double constrained_avg = constrained_sum/num_constrained;

#ifdef PLANEFIT_DEBUG_OUTPUT
                log << ccl::LERR << "Too many constraints (" << num_constrained << "), ran out of patience. Using average of (" << constrained_avg << ") contraints Z to create a flat plane" << log.endl;
                for (size_t i = 0, ic = data->points.size(); i < ic; i++)
                {
                    bool constrained = data->constrainedIndices.at(i);
                    
                    log << ccl::LERR << "\tPoint: " << data->points[i]->X() << " , " << data->points[i]->Y() << " , " << data->points[i]->Z() << " , " << data->points[i]->M();
                    if (constrained)
                        log << " (C)";
                    log << log.endl;
                }
                log << "\tPlane" << log.endl;
                for (size_t i = 0, ic = 3; i < ic; i++)
                {
                    log << ccl::LERR << "\t\tPoint: " << data->plane[i].X() << " , " << data->plane[i].Y() << " , " << data->plane[i].Z() << " , " << data->plane[i].M() << log.endl;
                }
#endif
                data->plane[0].setZ(constrained_avg);
                data->plane[1].setZ(constrained_avg);
                data->plane[2].setZ(constrained_avg);
                validFit = true;
                return true;
            }
            else
            {
#ifdef PLANEFIT_DEBUG_OUTPUT
                log << ccl::LERR << "Ran out of patience fitting a plane for " << log.endl;
                for (size_t i = 0, ic = data->points.size(); i < ic; i++)
                {
                    log << ccl::LERR << "\tPoint: " << data->points[i]->X() << " , " << data->points[i]->Y() << " , " << data->points[i]->Z() << " , " << data->points[i]->M() << log.endl;
                }
                log << "\tPlane" << log.endl;
                for (size_t i = 0, ic = 3; i < ic; i++)
                {
                    log << ccl::LERR << "\t\tPoint: " << data->plane[i].X() << " , " << data->plane[i].Y() << " , " << data->plane[i].Z() << " , " << data->plane[i].M() << log.endl;
                }
#endif
            }

        }

        if (data->status_struct.outcome!=5  && data->status_struct.outcome > 3)
        {
            validFit = false;
            if (data->status_struct.outcome > 12 || data->status_struct.outcome < 0)
                log << ccl::LERR << "Unable to fit plane. lmmin returns an invalid info message" << log.endl;
            else
                log << ccl::LERR << "Unable to fit plane. lmmin returns " << lm_infmsg[data->status_struct.outcome] << log.endl;
            return false;
        }
        validFit = true;
        return true;
    }
       

    void PlaneFitter::Callback(const double *par, int m_dat, const void *data, double *fvec, int *info)
    {
        PlaneFitterData* mfd = (PlaneFitterData*)(data);
        mfd->plane[0].setZ(par[0]);
        mfd->plane[1].setZ(par[1]);
        mfd->plane[2].setZ(par[2]);
        double *deltas = new double[mfd->points.size()];
        size_t num_points = mfd->points.size();
        for (size_t i = 0; i < num_points; i++)
        {
            sfa::Point *pt = mfd->points.at(i);
            double z = 0;
            if (mfd->interpolatePointInPlane(pt, z))
            {
                //printf("idx=%d error=%f\n", i, pt->Z() - z);
                double delta = abs(pt->Z() - z);
                deltas[i] = delta;
                fvec[i] = exp(delta);
                if (mfd->forceBelow && (z > pt->Z()))
                    fvec[i] = fvec[i] * mfd->points.size();
                if (mfd->constrainedIndices[i])
                {
                    fvec[i] = fvec[i] * mfd->points.size();//Constraints scale proportionally to the number of points
                }
            }            
        }
        for (int j = 0, jc = mfd->samePoints.size(); j < jc; j++)
        {
            int idxa = mfd->samePoints[j].first;
            int idxb = mfd->samePoints[j].second;
            if (idxa < mfd->points.size())
            {
                if (idxb < mfd->points.size())
                {
                    double pairdelta = abs(deltas[idxa] - deltas[idxb]);
                    fvec[idxa] += ((1 + pairdelta) * (1 + pairdelta))* mfd->points.size();
                    fvec[idxb] += ((1 + pairdelta) * (1 + pairdelta))* mfd->points.size();
                }
            }
        }

        delete[] deltas;
        

    }

    bool PlaneFitter::getPlane(sfa::Point &p1, sfa::Point &p2, sfa::Point &p3)
    {
        PlaneFitterData* mfd = (PlaneFitterData*)(data);
        p1 = mfd->plane[0];
        p2 = mfd->plane[1];
        p3 = mfd->plane[2];
        if (!validFit)
        {
#ifdef PLANEFIT_DEBUG_OUTPUT
            log << ccl::LERR << "Unable to fit plane, using average Z to form a flat plane." << log.endl;
#endif
            double sum = 0;
            PointList::iterator iter = data->points.begin();
            while (iter != data->points.end())
            {
                sfa::Point *pt = *iter++;
                sum += pt->Z();
            }
            double flatz = sum / data->points.size();
            p1.setZ(flatz);
            p2.setZ(flatz);
            p3.setZ(flatz);
        }
        return true;
    }


}