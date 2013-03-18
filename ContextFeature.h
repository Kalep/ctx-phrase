#ifndef _CONTEXT_FEATURE_H_
#define _CONTEXT_FEATURE_H
#include <vector>
using namespace std;
#pragma once

namespace Rabbit{
    /* *短语边界 */
    class PhraseBound{
        public:
            PhraseBound(int, int, int, int );
            int startF;/* *源端开始的位置 */
            int endF;  /* *源端结束的位置 */
            int startE;/* *目标端开始的位置 */
            int endE;  /* *目标端结束的位置 */

    };

    class ContextFeature{
        public:
        virtual bool isContext(const PhraseBound& prev ,const PhraseBound& next) ;
        virtual ~ContextFeature();
    };
    /* *是否是包含的特征*/
    class InFeature:public ContextFeature{
        public:
        virtual bool isContext(const PhraseBound& prev, const PhraseBound& next);
    };
    /* *是否临接的特征*/
    class CloseFeature:public ContextFeature{
        public:
        virtual bool isContext(const PhraseBound& prev, const PhraseBound& next);
    };
    /* *是否是覆盖的特征*/
    class CoverFeature:public InFeature{/* *覆盖是包含的反对称特征 */
        public:
        virtual bool isContext(const PhraseBound& prev, const PhraseBound& next);
    };
    /**组合模式*/
    class ContextFeatureManager:public ContextFeature{
        public:
            ContextFeatureManager(int mode);
            virtual ~ContextFeatureManager();
            virtual bool isContext(const PhraseBound& prev, const PhraseBound& next);
        private:
            vector<ContextFeature*> features;
    };   
}
#endif
