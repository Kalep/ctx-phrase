#ifndef _RULE_GRAPH_EXTRACTOR_H
#define _RULE_GRAPH_EXTRACTOR_H
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "tables-core.h"
#include "ContextFeature.h"
#include "PhraseExtractionOptions.h"
//#include "ExtractTask.h"
using namespace std;
using namespace Moses;
using namespace MosesTraining;

namespace Rabbit{
    class RuleGraphExtractor{
        public:
            RuleGraphExtractor(InputFileStream& sourceFile, InputFileStream& targetFile, InputFileStream& alignFile,OutputFileStream& phrase,OutputFileStream& phraseInv,OutputFileStream& phraseOri,OutputFileStream& phraseRelation,OutputFileStream& phraseGraph,int featureMode,int maxLength);
            void extract();
            /* *保留各种数字化的表 */
            void saveTables(OutputFileStream& sourcePhraseOut, OutputFileStream& targetPhraseOut, OutputFileStream& ruleOut);

            //friend class ExtractTask; 
        public:
            PhraseTable r_sourcePhrases;/*源端的短语*/
            PhraseTable r_targetPhrases;/* *目标端的短语 */
            RuleTable r_ruleTables;    /* *规则表 */

            InputFileStream& m_sourceFile; /* *源文件的输入流 */
            InputFileStream& m_targetFile; /* *目标文件的输入流*/
            InputFileStream& m_alignFile;   /* *对齐文件的输入流*/

            OutputFileStream& m_extractedFile; /* *正向短语表的输出流*/
            OutputFileStream& m_extractedFileInv;/* *反向短语表的输出流*/
            OutputFileStream& m_extractedFileOrientation;/* *调序的输出流*/
            OutputFileStream& r_extractedFileContext;/* *规则关系的输出流*/
            OutputFileStream& r_extractedFileGraph;/* *规则的图文件*/

            ContextFeatureManager r_featureManager;/* * 特征的抽取配置*/
            PhraseExtractionOptions r_extractionOptions;/* *抽取的一些配置*/
    };  
}
#endif
