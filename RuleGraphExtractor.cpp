/*
 * =====================================================================================
 *
 *       Filename:  RuleGraphExtractor.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  03/07/2013 11:02:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *   Organization:  ICT.CAS
 *
 * =====================================================================================
 */
#include "RuleGraphExtractor.h"
#include "SentenceAlignment.h"
#include "tables-core.h"
#include "SafeGetline.h"
#include "ExtractTask.h"

namespace Rabbit{
    RuleGraphExtractor::RuleGraphExtractor(InputFileStream& sourceFile, InputFileStream& targetFile, InputFileStream& alignFile,OutputFileStream& extractFile,OutputFileStream& extractFileInv,OutputFileStream& extractFileOri,OutputFileStream& extractFileGraph,OutputFileStream& extractFileContext,int featureMode, int maxLength):\
        m_sourceFile(sourceFile),m_targetFile(targetFile),m_alignFile(alignFile),\
        m_extractedFile(extractFile),m_extractedFileInv(extractFileInv),\
        m_extractedFileOrientation(extractFileOri),r_extractedFileContext(extractFileContext),\
        r_extractedFileGraph(extractFileGraph),r_featureManager(featureMode),r_extractionOptions(maxLength)
        {
            
        }

    void RuleGraphExtractor::extract()
    {
        int line = 0;
        istream* eFileP = &this->m_targetFile;
        istream* fFileP = &this->m_sourceFile;
        istream* aFileP = &this->m_alignFile;

        while(true){
            line++;
            if( line %10000 == 0 ) cerr<<"line :"<<line<<endl;
            //1,读三行，创建一个SentenceAlignment
            char targetString[LINE_MAX_LENGTH];
            SAFE_GETLINE((*eFileP),targetString,LINE_MAX_LENGTH,'\n',__FILE__);
            if(eFileP->eof())break;
            //cout<<"target:\t"<<targetString<<endl;
            char sourceString[LINE_MAX_LENGTH];
            SAFE_GETLINE((*fFileP),sourceString,LINE_MAX_LENGTH,'\n',__FILE__);
            if(fFileP->eof()) break;
            //cout<<"source:\t"<<sourceString<<endl;
            char alignString[LINE_MAX_LENGTH];
            SAFE_GETLINE((*aFileP),alignString,LINE_MAX_LENGTH, '\n',__FILE__);
            if(aFileP->eof()) break;
            //cout<<alignString<<endl;
            SentenceAlignment sentence;
            bool isCreated = sentence.create(targetString, sourceString, alignString, line);
            //2,根据SentenceAlignment,创建一个ExtractTask,将本身作为一个指针传入
            if( isCreated){
               // cout<<"isCreated"<<endl;
                ExtractTask task(*this, sentence);
                task.Run();
            }
        }
        //3,关闭输出流
        this->m_targetFile.Close();
        this->m_sourceFile.Close();
        this->m_alignFile.Close();
    }

    void RuleGraphExtractor::saveTables(OutputFileStream& sourcePhraseOut,OutputFileStream& targetPhraseOut,OutputFileStream& ruleOut)
    {
        sourcePhraseOut<<this->r_sourcePhrases;
        targetPhraseOut<<this->r_targetPhrases;
        ruleOut<< this->r_ruleTables;
    }
}
