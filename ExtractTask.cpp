/*
 * extract.cpp
 *  Modified by: Rohit Gupta CDAC, Mumbai, India
 *  on July 15, 2012 to implement parallel processing
 *      Modified by: Nadi Tomeh - LIMSI/CNRS
 *      Machine Translation Marathon 2010, Dublin
 */
#include "ExtractTask.h"
#include "ContextFeature.h"

namespace MosesTraining
{
    ExtractTask::ExtractTask(RuleGraphExtractor& extractor, SentenceAlignment& sentence):r_ruleGraphExtractor(extractor),m_sentence(sentence){}

    void ExtractTask::Run()
    {
        //1, 抽取基本的短语
        extract();
        //2，抽取短语的上下文
        extractContext();
        //3，保存短语文件
        writePhrasesToFile();
        //4，保存context文件
        writeContextFile();
        //5，输出图文件
        //writeGraphFile();
        //6, 清空位置
        m_extractedPhrases.clear();
        m_extractedPhrasesInv.clear();
        m_extractedPhrasesOri.clear();
    }
    void ExtractTask::extractContext()
    {
        unsigned int startPhrase = 0 ;
        for(; startPhrase < this->r_rulePositions.size(); startPhrase++)
        {
            //两两比较，确定关系
            const vector<int>& prevPhrasePositions = this->r_rulePositions[startPhrase];
            PhraseBound prev(prevPhrasePositions[0],prevPhrasePositions[1],prevPhrasePositions[2],prevPhrasePositions[3]);
            for(unsigned int endPhrase = startPhrase+1; endPhrase < this->r_rulePositions.size(); endPhrase++)
            {
                const vector<int>& nextPhrasePositions = this->r_rulePositions[endPhrase];
                PhraseBound next(nextPhrasePositions[0],nextPhrasePositions[1],nextPhrasePositions[2],nextPhrasePositions[3]);
                if(this->r_ruleGraphExtractor.r_featureManager.isContext(prev,next))
                {
                    this->r_rulePositions[startPhrase].push_back(endPhrase);//前4个是位置的key,后面的都是相关的ctx
                    this->r_rulePositions[endPhrase].push_back(startPhrase);//对称的
                }
            }
        }
    }
    void ExtractTask::extract()
    {
        int countE = this->m_sentence.target.size();
        int countF = this->m_sentence.source.size();
        //cout<<"maxLength:"<<this->r_ruleGraphExtractor.r_extractionOptions.maxPhraseLength<<endl;
        //cout<<"F:\t"<<countF<<"\tE:\t"<<countE<<endl;
        // check alignments for target phrase startE...endE
        // loop over extracted phrases which are compatible with the word-alignments
        for (int startE = 0; startE < countE; startE++)
        {
            for (int endE = startE;
                    endE < countE && endE < startE + this->r_ruleGraphExtractor.r_extractionOptions.maxPhraseLength;
                    endE++)//最大长度指的是target端的啊
            {

                int minF = 9999;
                int maxF = -1;
                vector< int > usedF = this->m_sentence.alignedCountS; //注意这个地方是复制
                //1, 找到[start,end]对齐到的f的区间，f对齐的次数减减，如果f不为0，那就是不合法的
                for (int ei = startE; ei <= endE; ei++) //这个地方，自己实现的时候，出了点错
                {
                   // cout<<"ei:"<< this->m_sentence.alignedToT[ei].size() << endl;
                    for (size_t i = 0; i < this->m_sentence.alignedToT[ei].size(); i++)
                    {
                        int fi = this->m_sentence.alignedToT[ei][i];
                        if (fi < minF)
                        {
                            minF = fi;
                        }
                        if (fi > maxF)
                        {
                            maxF = fi;
                        }
                        usedF[ fi ]--; //这个做法，我没有想到
                    }
                }
                //cout<< minF<<"\t"<<maxF<<endl;
                if (maxF >= 0 && // aligned to any source words at all
                        (maxF - minF < this->r_ruleGraphExtractor.r_extractionOptions.maxPhraseLength)) // source phrase within limits
                {
                    // check if source words are aligned to out of bound target words
                    bool out_of_bounds = false;
                    for (int fi = minF; fi <= maxF && !out_of_bounds; fi++)
                        if (usedF[fi] > 0)
                        {
                            // cout << "ouf of bounds: " << fi << "\n";
                            out_of_bounds = true;
                        }

                    //cout << "doing if for ( " << minF << "-" << maxF << ", " << startE << "," << endE << ")\n";
                    if (!out_of_bounds)
                    {
                        // start point of source phrase may retreat over unaligned
                        for (int startF = minF;
                                (startF >= 0 &&
                                 (startF > maxF - this->r_ruleGraphExtractor.r_extractionOptions.maxPhraseLength) && // within length limit
                                 (startF == minF || this->m_sentence.alignedCountS[startF] == 0)); // unaligned
                                startF--)
                            // end point of source phrase may advance over unaligned
                            for (int endF = maxF;
                                    (endF < countF &&
                                     (endF < startF + this->r_ruleGraphExtractor.r_extractionOptions.maxPhraseLength) && // within length limit
                                     (endF == maxF || this->m_sentence.alignedCountS[endF] == 0)); // unaligned
                                    endF++)   // at this point we have extracted a phrase
                            {
                                //false
                                string orientationInfo = "";
                                //计算orientationInfo
                                if (this->r_ruleGraphExtractor.r_extractionOptions.isWordModel())//yes
                                {
                                    REO_POS wordPrevOrient, wordNextOrient;
                                    bool connectedLeftTopP  = isAligned( this->m_sentence, startF - 1, startE - 1 );
                                    bool connectedRightTopP = isAligned( this->m_sentence, endF + 1,   startE - 1 );
                                    bool connectedLeftTopN  = isAligned( this->m_sentence, endF + 1, endE + 1 );
                                    bool connectedRightTopN = isAligned( this->m_sentence, startF - 1,   endE + 1 );
                                    wordPrevOrient = getOrientWordModel( connectedLeftTopP, connectedRightTopP);
                                    wordNextOrient = getOrientWordModel( connectedLeftTopN, connectedRightTopN);
                                    orientationInfo += getOrientString(wordPrevOrient, this->r_ruleGraphExtractor.r_extractionOptions.isWordType()) + " " + getOrientString(wordNextOrient, this->r_ruleGraphExtractor.r_extractionOptions.isWordType());
                                }

                     //           cout<<"add:"<< startE <<"\t"<<endE<<"\t"<<startF<<"\t"<<endF<<"\t"<<orientationInfo<<endl;

                                cachePhraseOutput(startE, endE, startF, endF, orientationInfo);

                                addPosition(startF,endF,startE,endE);   

                            }
                    }
                }
            }
        }

    }

    REO_POS getOrientWordModel(bool connectedLeftTop, bool connectedRightTop)
    {

        if ( connectedLeftTop && !connectedRightTop)
            return LEFT;
        if (!connectedLeftTop &&  connectedRightTop)
            return RIGHT;
        return UNKNOWN;
    }

    bool isAligned ( SentenceAlignment &sentence, int fi, int ei )
    {
        if (ei == -1 && fi == -1)//左上角
            return true;
        if (ei <= -1 || fi <= -1)
            return false;
        if ((size_t)ei == sentence.target.size() && (size_t)fi == sentence.source.size()) //右下角
            return true;
        if ((size_t)ei >= sentence.target.size() || (size_t)fi >= sentence.source.size())
            return false;
        for (size_t i = 0; i < sentence.alignedToT[ei].size(); i++)
            if (sentence.alignedToT[ei][i] == fi)
                return true;
        return false;
    }

    string getOrientString(REO_POS orient, REO_MODEL_TYPE modelType)
    {
        switch (orient)
        {
            case LEFT:
                return "mono";
                break;
            case RIGHT:
                return "swap";
                break;
            case UNKNOWN:
                return "other";
                break;
        }
        return "";
    }
    PHRASE_ID ExtractTask::addSourcePhrase(int startF, int endF)
    {
        PHRASE_ID sourcePhraseId ;
        pair<int,int> phrase(startF,endF);
        map<pair<int,int> , PHRASE_ID >::iterator sourcePairIter = this->r_localSourcePhrases.find(phrase);
        if(sourcePairIter == this->r_localSourcePhrases.end())
        {
            //构造一个词的vector
            vector< WORD_ID > phrases(this->m_sentence.source.begin()+startF, this->m_sentence.source.begin()+endF+1);
            sourcePhraseId = this->r_ruleGraphExtractor.r_sourcePhrases.storeIfNew(phrases);
            this->r_localSourcePhrases[phrase] = sourcePhraseId;
        }
        else sourcePhraseId = sourcePairIter->second;
        return sourcePhraseId;
    }
    PHRASE_ID ExtractTask::addTargetPhrase(int startE, int endE)
    {
        PHRASE_ID targetPhraseId;
        pair<int,int> phrase(startE,endE);
        map<pair<int,int> ,PHRASE_ID >::iterator targetPairIter = this->r_localTargetPhrases.find(phrase);
        if( targetPairIter == this->r_localTargetPhrases.end())
        {
            vector< WORD_ID > phrases(this->m_sentence.target.begin()+startE, this->m_sentence.target.begin()+endE+1);
            targetPhraseId = this->r_ruleGraphExtractor.r_targetPhrases.storeIfNew(phrases);
            this->r_localTargetPhrases[phrase] = targetPhraseId;
        }
        else targetPhraseId = targetPairIter->second;
        return targetPhraseId;
    }
    RULE_ID ExtractTask::addRule(PHRASE_ID sourcePhraseId, PHRASE_ID targetPhraseId)
    {
        RULE_ID ruleId;
        pair<PHRASE_ID, PHRASE_ID> rule(sourcePhraseId, targetPhraseId);
        map<pair<PHRASE_ID , PHRASE_ID >, RULE_ID >::iterator ruleIter = this->r_localRules.find(rule);
        if( ruleIter == this->r_localRules.end())
        {
            ruleId = this->r_ruleGraphExtractor.r_ruleTables.storeIfNew(rule);
            //cout<<"push:\t"<<sourcePhraseId<<"\t"<<targetPhraseId<<"\t"<<ruleId<<"\t"<<this->r_ruleIds.size()<<endl;
            //push to local :
            this->r_localRules[rule] = ruleId;

        }
        else ruleId = ruleIter->second;
        this->r_ruleIds.push_back(ruleId);
        return ruleId;
    }
    void ExtractTask::addPosition(int startE, int endE, int startF, int endF)
    {
        vector<int> current_position;

        current_position.push_back(startE);
        current_position.push_back(endE);
        current_position.push_back(startF);
        current_position.push_back(endF);

        this->r_rulePositions.push_back(current_position);
    }
    void ExtractTask::cachePhraseOutput(int startE, int endE, int startF, int endF , string &orientationInfo)
    {
        //1,查找ID
        PHRASE_ID sourcePhraseId = this->addSourcePhrase(startF,endF);//localHash
        PHRASE_ID targetPhraseId = this->addTargetPhrase(startE,endE);//localHash
        RULE_ID ruleId = this->addRule(sourcePhraseId, targetPhraseId);//localHash
        //2,输出正向翻译和反向翻译的信息
        //   // cout << "adding ( " << startF << "-" << endF << ", " << startE << "-" << endE << ")\n";
        ostringstream outextractstr;
        ostringstream outextractstrInv;
        ostringstream outextractstrOrientation;
        //2.1 输出源端
        for( int fi = startF; fi <= endF; fi++)
        {
            outextractstr << this->m_sentence.source[fi]<<" ";
        }
        outextractstr << "||| ";
        //2.2 输出目标端
        for( int ei = startE ; ei <= endE; ei++)
        {
            outextractstr << this->m_sentence.target[ei]<<" ";
            outextractstrInv << this->m_sentence.target[ei] << " ";
        }
        outextractstr <<"||| ";
        outextractstrInv<<"||| ";
        //2.3 source (for inverse)
        for (int fi = startF; fi <= endF; fi++)
            outextractstrInv << this->m_sentence.source[fi] << " ";
        outextractstrInv << "|||";
        //2.4 输出对齐信息，不合并
        {
            for (int ei = startE; ei <= endE; ei++)
            {
                for (unsigned int i = 0; i < this->m_sentence.alignedToT[ei].size(); i++)
                {
                    int fi = this->m_sentence.alignedToT[ei][i];
                    outextractstr << " " << fi - startF << "-" << ei - startE;
                    outextractstrInv << " " << ei - startE << "-" << fi - startF;
                }
            }
        }
        //2.5 附上规则的Id
        outextractstr <<" ||| "<<ruleId<< "\n";
        outextractstrInv<<" ||| "<<ruleId<< "\n";
        //3. 输出调序信息
        outextractstrOrientation << sourcePhraseId<<" ||| "<<targetPhraseId<<" ||| "<<orientationInfo<<" ||| "<<ruleId << endl;
        //4. 将输出信息压入到缓存区
        m_extractedPhrases.push_back(outextractstr.str());
        m_extractedPhrasesInv.push_back(outextractstrInv.str());
        m_extractedPhrasesOri.push_back(outextractstrOrientation.str());
    }
    void ExtractTask::writeGraphFile()
    {
        ostringstream outextractGraph;
        //outextractGraph<<"<gexf xmlns:viz=\"http:///www.gexf.net/1.1draft/viz\" version=\"1.1\" xmlns=\"http://www.gexf.next/1.1draft\">\n";
        //outextractGraph<<"<meta lastmodifieddate=\"2010-03-03+23:44\">\n";
        //outextractGraph<<"<creator> Gephi 0.7 \n</creator>\n</meta>\n";
        //outextractGraph<<"<graph defaultedgetype=\"undirected\" idtype=\"string\" type=\"static\">\n";
        //outextractGraph<<"<node count=\""<<this->r_rulePositions.size()<<"\">\n";
        //output node 
        //outextractGraph<<"</node>\n";
        //this->this->r_ruleGraphExtractor.r_extractedFileGraph<<outextractGraph.str();
        //outextractGraph.str(""); */
        outextractGraph<<"graph {\n";
        outextractGraph<<"edge [fontname=\"FangSong\"];\n";
        outextractGraph<<"node [fontname=\"FangSong\"];\n";
        //output edges
        //int edgeId = 0;
        for(unsigned int ruleIndex = 0u ; ruleIndex < this->r_rulePositions.size(); ruleIndex++)
        {
            if(this->r_rulePositions[ruleIndex].size()<5) continue;
            for(unsigned int relatedIndex = 4u; relatedIndex < this->r_rulePositions[ruleIndex].size(); relatedIndex++)
            {
                int relatedRuleIndex = this->r_rulePositions[ruleIndex][relatedIndex];
                //outextractGraph<<"<edge id=\""<<edgeId<<"\" source=\""<<ruleIndex<<"\" target=\""<<relatedRuleIndex<<"\"/>\n";
                //edgeId++;
                outextractGraph << this->r_ruleIds[ruleIndex]<<"--"<<this->r_ruleIds[relatedRuleIndex]<<";\n";
            }
        }
        //this->this->r_ruleGraphExtractor.r_extractedFileGraph<<"<edges count=\""<<edgeId<<"\">\n"<<outextractGraph.str()<<"</edges>\n</graph>\n</gexf>\n";
        outextractGraph<<"}";
        this->r_ruleGraphExtractor.r_extractedFileGraph<<outextractGraph.str();
    } 
    void ExtractTask::writeContextFile()
    {
        unsigned int ruleIndex = 0;
        ostringstream outextractContext;
        //cout<<"size:"<< this->r_rulePositions.size() <<"\t"<<this->r_ruleIds.size()<<endl;
        for( ;ruleIndex < this->r_rulePositions.size(); ruleIndex++)
        {
            vector<int>& positions = this->r_rulePositions[ruleIndex];
            //根据前四个位置关系，输出一下
            //string prevString;
            //this->output(positions[0],positions[1],positions[2],positions[3],prevString);
            outextractContext<<this->r_ruleIds[ruleIndex]<<" " ;

            if(positions.size() < 5) {
                outextractContext<<endl;
                continue;
            } 
            unsigned int relatedRulePositionIndex = 4u;
            for(; relatedRulePositionIndex < positions.size(); relatedRulePositionIndex++)
            {
                unsigned int relatedRuleIndex = positions[relatedRulePositionIndex];
                //string nextString;
                //this->output(nextPositions[0],nextPositions[1],nextPositions[2],nextPositions[3],nextString);
                outextractContext<<this->r_ruleIds[relatedRuleIndex]<<" ";
                //根据位置关系输出下
            }
            outextractContext<<endl;
        }
        this->r_ruleGraphExtractor.r_extractedFileContext<<outextractContext.str();
        //输出一个graph文件
    }

    void ExtractTask::writePhrasesToFile()
    {

        ostringstream outextractFile;
        ostringstream outextractFileInv;
        ostringstream outextractFileOrientation;

        for (vector<string>::const_iterator phrase = m_extractedPhrases.begin(); phrase != m_extractedPhrases.end(); phrase++)
        {
            outextractFile << phrase->data();
        }
        for (vector<string>::const_iterator phrase = m_extractedPhrasesInv.begin(); phrase != m_extractedPhrasesInv.end(); phrase++)
        {
            outextractFileInv << phrase->data();
        }
        for (vector<string>::const_iterator phrase = m_extractedPhrasesOri.begin(); phrase != m_extractedPhrasesOri.end(); phrase++)
        {
            outextractFileOrientation << phrase->data();
        }

        this->r_ruleGraphExtractor.m_extractedFile << outextractFile.str();
        this->r_ruleGraphExtractor.m_extractedFileInv  << outextractFileInv.str();
        this->r_ruleGraphExtractor.m_extractedFileOrientation << outextractFileOrientation.str();
    }
}
