/*
 * =====================================================================================
 *
 *       Filename:  Numberizor.cpp
 *
 *    Description:  implementation of numberizor.h 
 *
 *        Version:  1.0
 *        Created:  03/07/2013 03:55:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *   Organization:  ICT.CAS
 *
 * =====================================================================================
 */
#include "SafeGetline.h"
#include <iostream>
#include "StringHelper.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include "Numberizor.h"
#include "tables-core.h"

using namespace std;
using namespace MosesTraining;
using namespace Moses;

namespace Rabbit{
    Numberizor::Numberizor(InputFileStream& input, OutputFileStream& vocOutput, OutputFileStream& srcOutput):\
        r_inputStream(input),r_vocOutput(vocOutput),r_srcOutput(srcOutput)
    {

    }
    void Numberizor::numberize(const string& line)
    {
        vector<string> words;
        tokenize(line.c_str(),words);
        vector< WORD_ID > ids(words.size());
        for( unsigned int wordIndex = 0u ; wordIndex < words.size() ; wordIndex++)
        {
            WORD_ID  wordId = this->r_simpleVoc.storeIfNew(words[wordIndex]);
            ids[wordIndex] = wordId;
        }  
        copy(ids.begin(),ids.end(), ostream_iterator<WORD_ID>(this->r_srcOutput," "));
        this->r_srcOutput<<endl;
    }
    void Numberizor::numberize()
    {
        istream * r_inputStreamP = &this->r_inputStream;
        while(true){
            char sourceString[LINE_MAX_LENGTH];
            SAFE_GETLINE((*r_inputStreamP) , sourceString, LINE_MAX_LENGTH, '\n',__FILE__);
            if(r_inputStreamP->eof()) break;
            this->numberize(sourceString);
        }
        this->r_inputStream.Close();
        this->r_vocOutput<< this->r_simpleVoc;
        this->r_vocOutput.Close();
        this->r_srcOutput.Close();
    }
}

