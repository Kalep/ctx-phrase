/***********************************************************************
  Moses - factored phrase-based language decoder
  Copyright (C) 2010 University of Edinburgh

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***********************************************************************/

#include "SentenceAlignment.h"

#include <map>
#include <set>
#include <string>
#include "StringHelper.h"
#include <iostream>
//#include "test.h"

using namespace Rabbit;
using namespace std;

namespace MosesTraining
{
    SentenceAlignment::~SentenceAlignment() {}
    /**
     * create a structure for Alignment
     * @param  targetString     en
     * @param  sourceString    [ch]
     * @param  alignmentString [0-0]
     * @param  weightString    [description]
     * @param  sentenceID      []
     * @param  boundaryRules   [false]
     * @return                 [description]
     */
    bool SentenceAlignment::create( char targetString[], char sourceString[], char alignmentString[], int sentenceID)
    {
        tokenize(targetString, this->target);
        tokenize(sourceString, this->source);
        // check if sentences are empty
        if (target.size() == 0 || source.size() == 0)
        {
            std::cerr << "no target (" << target.size() << ") or source (" << source.size() << ") words << end insentence " << sentenceID << endl;
            std::cerr << "T: " << targetString << endl << "S: " << sourceString << endl;
            return false;
        }

        // prepare data structures for alignments
        for (size_t i = 0; i < source.size(); i++)
        {
            alignedCountS.push_back( 0 ); 
        }
        for (size_t i = 0; i < target.size(); i++)
        {
            vector< int > dummy;
            alignedToT.push_back( dummy );
        }

        // reading in alignments
        vector<string> alignmentSequence;
        tokenize(alignmentString,alignmentSequence);

        for (size_t i = 0; i < alignmentSequence.size(); i++)
        {
            int s, t;
            // cout << "scaning " << alignmentSequence[i].c_str() << endl;
            if (! sscanf(alignmentSequence[i].c_str(), "%d-%d", &s, &t))
            {
                cerr << "WARNING: " << alignmentSequence[i] << " is a bad alignment point in sentence " << sentenceID << endl;
                cerr << "T: " << targetString << endl << "S: " << sourceString << endl;
                return false;
            }

            // cout << "alignmentSequence[i] " << alignmentSequence[i] << " is " << s << ", " << t << endl;
            if ((size_t)t >= target.size() || (size_t)s >= source.size())
            {
                cerr << "WARNING: sentence " << sentenceID << " has alignment point (" << s << ", " << t << ") out of bounds (" << source.size() << ", " << target.size() << ")\n";
                cerr << "T: " << targetString << endl << "S: " << sourceString << endl;
                return false;
            }
            alignedToT[t].push_back( s ); //t就是e端的
            alignedCountS[s]++;
        }
        return true;
    }
}

