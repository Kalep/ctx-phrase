/*
 * =====================================================================================
 *
 *       Filename:  HalfScorer.cpp
 *
 *    Description:  score implementation of HalfScorer.h
 *
 *        Version:  1.0
 *        Created:  03/15/2013 11:24:12 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
namespace Rabbit{
    HalfScorer::HalfScorer(InputFileStream& extract,InputFileStream& lexical,& OutputFileStream& output,bool inverse):extractFile(extract),lexicalFile(lexical),phraseTableFile(output),inverseFlag(inverse){
        lexicalTable.load(lexicalFile);
    }
    void HalfScorer::clearPhrasePairs(vector<PhraseAlignment*> & phrasePairs)
    {
        for(vector<PhraseAlignment*>::iterator pairIter = phrasePairs.begin(); pairIter != phrasePairs.end(); pairIter++)
            if(NULL != *pairIter)
                delete *pairIter;
        phrasePairs.clear();
    }
    void HalfScorer::score()
    {
        istream& extractFileP = this->extractFile;
        int line = 0 ;
        char currentLine[LINE_MAX_LENGTH]={'\0'},prevLine[LINE_MAX_LENGTH]={'\0'};

        vector<PhraseAlignment* > sameSourceAlignments;

        PhraseAlignment* current = NULL ;
        PhraseAlignment* last = NULL;
        while(true){
            if( extractFileP.eof()) break;
            if(0== ++line % 10000 ) cerr <<"line: "<< line<<endl;
            //1,read a line
            SAFE_GETLINE((extractFileP), currentLine, LINE_MAX_LENGTH, '\n', __FILE__);
            if(extractFileP.eof()) break;

            //2,check whether a same phrase or not
            if(0==strcmp(currentLine,prevLine))
            {
                //collect count
                last->count ++;
                continue;
            }
            //3, create a new alignment ;
            current = new PhraseAlignment;
            current->create(currentLine);
            //4, compare source side
            if( NULL == last || current->GetSource() == last->GetSource()){
                sameSourceAlignments.push_back(current);
            }else{
                // process sameSourceAlignments;
                this->processPhrasePairs(sameSourceAlignment);
                // clear sampeSourceAlignments;
                this->clearPhrasePairs(sameSourceAlignments);
                // push a new pair;
                sameSourceAlignments.push_back(current);
            }
            //clear the count buffer
            strcpy(prevLine,currentLine);
            last = current;
        }

    }
    
    void HalfScorer::processPhrasePairs(vector<PhraseAlignment*>& phrasePairs)
    {
        if(phrasePairs.size() == 0 ) return ;
        float sourceCount = 0 ;
        PhrasePairGroup groups;
        //1, 搜集源端的count;
        //2, 对源端和目标端相同，但是对齐不同的分组
        for( vector<PhraseAlignment*>::iterator pairIter = phrasePairs.begin();pairIter != phrasePairs.end();phraseIter++)
        {
            sourceCount += (*pairIter)->count;
            PhraseAlignmentCollection collection;
            collection.push_back(*pairIter);
            pair<PhrasePairGroup::iterator, bool> result = groups.insert(collection);
            if(!result.second){
                PhraseAlignmentCollection& existCollection = const_cast<PhraseAlignmentCollection& > (*result.first);
                existCollection->push_back(*pairIter);
            }
        }
        //3，遍历所有的组
        const SoretedKeys& sortedKeys = groups.GetSortedKeys();
        SortedKeys::const_iterator groupIter = sortedKeys.begin();
        for(; groupIter != sortedKeys.end(); groupIter++)
        {
            this->processSameRuleGroup(**groupIter,sourceCount);    
        } 
        //4，处理每个组里的对齐，找一个最好的对齐，算lexical 的分数
    }
    PhraseAlignment* HalfScorer::findBestAlignment(const PhraseAlignmentCollection& collection)
    {
        float bestAlignmentCount = -1;
        PhraseAlignment* bestAlignment = NULL;
        for( size_t i = 0 ; i< collection.size(); i++)
        {
            size_t alignInd;
            if(inverseFlag)
            {
                alignInd = collection.size() - i -1;    
            }
            else alignInd = i;
            if( collection[alignInd]->count > bestAlignmentCount )
            {
                bestAlignmentCount = collection[alignInd]->count;
                bestAlignment = collection[alignInd];
            }
        }
        return bestAlignment;
    }
    double  HalfScorer::computeLexicalScore(PhraseAlignment* bestAlignment)
    {
        WORD_ID null_id = 0 ;
        double score = 1.0;
        const PHRASE&  sourcePhrase = bestAlignment->GetSource();
        const PHRASE&  targetPhrase = bestAlignment->GetTarget();

        for( size_t fi = 0 ; fi< bestAlignment->alignedToT.size(); fi++)
        {
            const set< size_t > & sourceIndices = bestAlignment->alignedToT[fi];
            
            if(sourceIndices.size() == 0 )
                score *= this->lexicalTable.lookup(null_id,targetPhrase[fi]);
            else{
                double total = 0.0 ;
                for(size_t ei = 0; ei< sourceIndices.size() ; ei++)
                {
                    int sourceIndex = sourceIndices[ei];
                    total+= this->lexicalTable.lookup(sourcePhrase[sourceIndex],targetPhrase[fi]);
                }
                score * = (total/sourceIndices.size());
            }
        }
        return score;
    }
    void HalfScorer::processSameRuleGroup(const PhraseAlignmentCollection& collection, int totalCount)
    {
        if(collection.size() == 0 ) return ;
        //1,计算组内的总出现次数
        int count = 0 ;
        for( int i = 0 ; i < collection.size(); i++)
            count+= collection[i]->count;
        //2,确定一个最好的对齐
        PhraseAlignment* bestAlignment = this->findBestAlignment(collection);
        //3,根据最好的对齐确定词汇化翻译的分数
        double lexicalScore = this->computeLexicalScore(bestAlignment);
        PhraseAlignment* currentAlignment = collection[0];
        //4,输出源端
        copy(currentAlignment->GetSource().begin(), currentAlignment->GetSource().end(),ostream_iterator<PHRASE_ID >(this->phraseTableFile," "));
        this->phraseTableFile<<" ||| ";
        //5,输出目标端
        copy(currentAlignment->GetTarget().begin(), currrentAlignment->GetTarget().end(),ostream_iterator<PHRASE_ID >(this->phraseTableFile," "));
        this->phraseTableFile<<" ||| ";
        //6,输出分数
        this->phraseTableFile<<lexicalScore<<" ||| ";
        //7，输出count
        this->phraseTableFile<<totalCount<<" "<<count<<" ||| ";
        //8, 输出ruleId
        this->phraseTableFile<<currentAlignment->GetRuleId()<<endl;
    }
};

