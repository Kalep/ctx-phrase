#ifndef _HALF_SCORER_H
#define _HALF_SCORER_H
#include "SaftGetline.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "String.h"
#include "score.h"
#include <vector>
#include "PhraseAlignment.h"

using namespace std;
using namespace MosesTraing;
namespace Rabbit{
    class HalfScorer{
        public:
            HalfScorer(InputFileStream& , InputFileStream& , OutputFileStream& , bool );
            void score();
        private:
            void clearPhrasePairs(vector<PhraseAlignment*>& );
            void processPhrasePairs(vector<PhraseAlignment*>& ); 

            InputFileStream& extractFile;
            InputFileStream& lexicalFile;
            OutputFileStream& phraseTableFile;
            bool inverseFlag;
            LexicalTable lexicalTable;
    };
}
#endif
