/*
 * =====================================================================================
 *
 *       Filename:  ContextFeature.cpp
 *
 *    Description:  features for rule context
 *
 *        Version:  1.0
 *        Created:  03/08/2013 05:05:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang
 *   Organization:  ICT.CAS
 *
 * =====================================================================================
 */
#include "ContextFeature.h"

namespace Rabbit{
    PhraseBound::PhraseBound(int sf,int ef,int se,int ee):startF(sf),endF(ef),startE(se),endE(ee){}
    ContextFeature::~ContextFeature(){

    }
    bool ContextFeature::isContext(const PhraseBound& prev, const PhraseBound& next)
    {
        return false;
    }
    bool InFeature::isContext(const PhraseBound& prev, const PhraseBound& next)
    {
        bool flag = false;
        if(
                (
                 (next.startE == prev.startE && next.endE<= prev.endE)||//目标端左覆盖
                 (next.startE >= prev.startE && next.endE == prev.endE)//目标端右覆盖
                )
                &&
                (
                 (next.startF == prev.startF && next.endF<= prev.endF)||//in s3 t3源端左覆盖
                 (next.endF == prev.endF && next.startF >= prev.startF)//s3 t3'源端右覆盖
                )
          )
        {
            flag = true;
        }
        return flag;
    }
    bool CloseFeature::isContext(const PhraseBound& prev, const PhraseBound& next)
    {
        bool flag = false;
        if((next.endE == prev.startE-1 && next.endF == prev.startF-1)//左相邻
                ||(next.startE == prev.endE+1 && next.startF == prev.endF+1))//右相邻
        {
            flag = true;
        }
        else if((next.startE == prev.endE+1 && next.endF == prev.startF-1)||//交叉相邻 s2 t2_1
                (next.endE == prev.startE-1 && next.startF == prev.endF+1))
        {
            flag = true;
        }
        return flag;
    }
    bool CoverFeature::isContext(const PhraseBound& prev, const PhraseBound& next)
    {
        return InFeature::isContext(next,prev);
    }

    ContextFeatureManager::ContextFeatureManager(int mode){
        int flag = 1;
        if(mode&flag) features.push_back(new CloseFeature);
        flag<<=1;
        if(mode&flag) features.push_back(new InFeature);
        flag<<=1;
        if(mode&flag) features.push_back(new CoverFeature);
    }
    ContextFeatureManager::~ContextFeatureManager()
    {
        vector<ContextFeature*>::iterator featIter = features.begin();
        for(; featIter != features.end(); featIter++)
        {
            if(*featIter != NULL)
                delete *featIter; 
            *featIter = NULL;
        }
        features.clear();    
    }
    bool ContextFeatureManager::isContext(const PhraseBound& prev, const PhraseBound& next)
    {
        bool flag = false;
        vector<ContextFeature*>::iterator featIter = features.begin();
        for(; !flag && featIter !=features.end(); featIter++)
        {
            flag |= (*featIter)->isContext(prev,next);
        }
        return flag;
    }
}
