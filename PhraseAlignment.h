#pragma once
/*
 *  PhraseAlignment.h
 *  extract
 *
 *  Created by Hieu Hoang on 28/07/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#include "tables-core.h"
#include <vector>
#include <set>
#include <map>

namespace MosesTraining
{

    // data structure for a single phrase pair
    class PhraseAlignment
    {
        protected:
            PHRASE phraseS;
            PHRASE phraseT;
            RULE_ID ruleId;
        public:
            float count;

            std::vector< std::set<size_t> > alignedToT;
            std::vector< std::set<size_t> > alignedToS;

            void create( char*, int );
            void clear();
            bool equals( const PhraseAlignment& );
            bool match( const PhraseAlignment& );

            int Compare(const PhraseAlignment &compare) const;
            inline bool operator<(const PhraseAlignment &compare) const
            { 
                return Compare(compare) < 0;
            }

            inline const PHRASE &GetSource() const {
                return phraseS;
            }
            inline const PHRASE &GetTarget() const {
                return phraseT;
            }
            inline const RULE_ID& getRuleId() const{
                return ruleId;
            }
    };
    
    typedef std::vector<PhraseAlignment*> PhraseAlignmentCollection;
    
    class PhraseAlignmentCollectionComparator{
        public:
            bool operator()(const PhraseAlignmentCollection& left, const PhraseAlignmentCollection& right) const
            {
                assert(left.size()>0);
                assert(right.size()>0);
                return (*left[0]) < (*right[0]);
            }
    };
    
    class PhrasePairGroup{
                   
        private:
            typedef std::set<PhraseAlignmentCollection , PhraseAlignmentCollectionComparator > Coll;
            Coll m_coll;
        public:
            typedef Coll::iterator iterator;
            typedef Coll::const_iterator iterator;
            //用vector是为了保证顺序
            typedef std::vector< const PhraseAlignmentCollection* > SortedKeys;
            
            std::pair<Coll::iterator , bool > insert(const  PhraseAlignmentCollection & obj);
            
            const SortedKeys& GetSortedKeys() const
            {
                return m_sortedKeys;
            }
            
            size_t GetSize() const
            {
                return m_coll.size();
            }
        private:
            SortedKeys m_sortedKeys;
    };
}

