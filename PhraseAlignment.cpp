/*
 *  PhraseAlignment.cpp
 *  extract
 *
 *  Created by Hieu Hoang on 28/07/2010.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <sstream>
#include "PhraseAlignment.h"
#include "SafeGetline.h"
#include "tables-core.h"
#include "score.h"
#include "StringHelper.h"
#include <cstdlib>

using namespace std;

namespace MosesTraining
{
    // read in a phrase pair and store it
    void PhraseAlignment::create( char line[], int lineID )
    {
        assert(phraseS.empty());
        assert(phraseT.empty());

        //cerr << "processing " << line;
        vector< string > token;
        tokenize( line ,token);
        int item = 1;
        int temp_int;
        //source ||| target ||| alignment ||| rule_Id
        for (size_t j=0; j<token.size(); j++) {
            if (token[j] == "|||") item++;
            else if (item == 1) { // source phrase
                sscanf(token[j].c_str(),"%d",&temp_int);
                phraseS.push_back( temp_int );
            }

            else if (item == 2) { // target phrase
                sscanf(token[j].c_str(),"%d",&temp_int);
                phraseT.push_back( temp_int );
            }
            else if (item == 3) { // alignment
                int s,t;
                sscanf(token[j].c_str(), "%d-%d", &s, &t);
                if ((size_t)t >= phraseT.size() || (size_t)s >= phraseS.size()) {
                    cerr << "WARNING: phrase pair " << lineID
                        << " has alignment point (" << s << ", " << t
                        << ") out of bounds (" << phraseS.size() << ", " << phraseT.size() << ")\n";
                } else {
                    // first alignment point? -> initialize
                    if( this->alignedToT.size() == 0)
                        this->alignedToT.resize(phraseT.size());
                    if( this->alignedToS.size() == 0)
                        this->alignedToS.resize(phraseS.size());
                    // add alignment point
                    alignedToT[t].insert( s );
                    alignedToS[s].insert( t );
                }
            } else if (item == 4) { // count
                sscanf(token[j].c_str(), "%d", &ruleId);
            }
        }
        // for possible null alignment 
        if( this->alignedToT.size() == 0)
            this->alignedToT.resize(phraseT.size());
        if( this->alignedToS.size() == 0)
            this->alignedToS.resize(phraseS.size());

        if (item == 4) {
            count = 1.0;
        }
        if (item < 4 || item > 6) {
            cerr << "ERROR: faulty line " << lineID << ": " << line << endl;
        }
    }

    void PhraseAlignment::clear()
    {
        phraseS.clear();
        phraseT.clear();
        alignedToT.clear();
        alignedToS.clear();
    }

    // check if two word alignments between a phrase pair are the same
    bool PhraseAlignment::equals( const PhraseAlignment& other )
    {
        if (this == &other) return true;
        if (other.GetRuleId() != this->GetRuleId() ) return false;
        if (other.alignedToT != alignedToT) return false;
        if (other.alignedToS != alignedToS) return false;
        return true;
    }

    // check if two word alignments between a phrase pairs "match"
    // i.e. they do not differ in the alignment of non-termimals
    bool PhraseAlignment::match( const PhraseAlignment& other )
    {
        if (this == &other) return true;
        if(other.GetRuleId() != this->GetRuleId()) return false;
        return true;
    }

    int PhraseAlignment::Compare(const PhraseAlignment &other) const
    {
        if (this == &other) // comparing with itself
            return 0;

        if (GetTarget() != other.GetTarget()) //先比的是目标端
            return ( GetTarget() < other.GetTarget() ) ? -1 : +1;

        if (GetSource() != other.GetSource())
            return ( GetSource() < other.GetSource() ) ? -1 : +1;

        return 0;

    }
    std::pair<PhrasePairGroup::iterator,bool > PhrasePairGroup::insert(const PhraseAlignmentCollection& obj)
    {
        std::pair<PhrasePairGroup::iterator,bool>  result = this->m_coll.insert(obj);
        if(result->second){
            this->m_sortedKeys.push_back(&obj);
        }
        return result;
    }
}

