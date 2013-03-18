#ifndef _EXTRACT_TASK_H
#define _EXTRACT_TASK_H
/*
 * extract.cpp
 *  Modified by: Rohit Gupta CDAC, Mumbai, India
 *  on July 15, 2012 to implement parallel processing
 *      Modified by: Nadi Tomeh - LIMSI/CNRS
 *      Machine Translation Marathon 2010, Dublin
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <cstring>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include "SafeGetline.h"
#include "SentenceAlignment.h"
#include "tables-core.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "RuleGraphExtractor.h"

using namespace std;
using namespace MosesTraining;
using namespace Rabbit;

namespace MosesTraining
{
    REO_POS getOrientWordModel(bool, bool);
    string getOrientString(REO_POS, REO_MODEL_TYPE);
    bool isAligned (SentenceAlignment &, int, int);
}

class Rabbit::RuleGraphExtractor;
namespace MosesTraining
{

    class ExtractTask
    {
        public:
            ExtractTask(Rabbit::RuleGraphExtractor& extractor,SentenceAlignment& sentence);
            void Run();
        private:
            vector< string > m_extractedPhrases;
            vector< string > m_extractedPhrasesInv;
            vector< string > m_extractedPhrasesOri;

            vector< vector<int> > r_rulePositions; /**抽出来的短语的位置*/

            void extract();
            void extractContext();/**抽取上下文的短语*/

            void cachePhraseOutput(int, int, int, int, string &);
            /* *添加一个源端的短语 */
            PHRASE_ID addSourcePhrase(int,int);
            /* *添加一个目标端的短语位置 */
            PHRASE_ID addTargetPhrase(int,int);
            /* *添加一条RULE*/
            RULE_ID addRule(PHRASE_ID, PHRASE_ID);
            /**添加一个短语的位置*/
            void addPosition(int, int, int,int); 

            void writePhrasesToFile();
            void writeContextFile();
            void writeGraphFile();
            /**这些都是引用的形式*/
            Rabbit::RuleGraphExtractor& r_ruleGraphExtractor;
            SentenceAlignment &m_sentence;/**翻译的句子*/

            map<pair<int,int> ,PHRASE_ID > r_localTargetPhrases;
            map<pair<int,int> ,PHRASE_ID > r_localSourcePhrases;
            map<pair<PHRASE_ID,PHRASE_ID >, RULE_ID >  r_localRules;

            vector< RULE_ID > r_ruleIds;
    };
}
#endif
