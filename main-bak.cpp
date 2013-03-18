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
//#include "tables-core.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "PhraseExtractionOptions.h"

using namespace std;
using namespace MosesTraining;

namespace MosesTraining
{


    const long int LINE_MAX_LENGTH = 500000 ;

    REO_POS getOrientWordModel(bool, bool);
    string getOrientString(REO_POS, REO_MODEL_TYPE);
    bool isAligned (SentenceAlignment &, int, int);
    int sentenceOffset = 0;/**有这个标记，可以从多个地方开始起训练*/

}

namespace MosesTraining
{

    class ExtractTask
    {
        public:
            ExtractTask(size_t id, SentenceAlignment &sentence, PhraseExtractionOptions &initoptions, Moses::OutputFileStream &extractFile, Moses::OutputFileStream &extractFileInv, Moses::OutputFileStream &extractFileOrientation,Moses::OutputFileStream& extractFileContext,Moses::OutputFileStream& extractFileGraph):
                m_sentence(sentence),
                m_options(initoptions),
                m_extractFile(extractFile),
                m_extractFileInv(extractFileInv),
                m_extractFileOrientation(extractFileOrientation),
                m_extractFileContext(extractFileContext),
                m_extractFileGraph(extractFileGraph){}
            void Run();
        private:
            vector< string > m_extractedPhrases;
            vector< string > m_extractedPhrasesInv;
            vector< string > m_extractedPhrasesOri;

            vector< vector<int> > r_phrasePositions; /**抽出来的短语的位置*/

            void extract(SentenceAlignment &);
            void extractContext();/**抽取上下文的短语*/
            void addPhrase(SentenceAlignment &, int, int, int, int, string &);

            void addPosition(int, int, int,int); /**添加一个短语的位置*/
            void output(int, int, int, int, string& ) const;
            void writePhrasesToFile();
            void writeContextFile();
            void writeGraphFile();
            /**这些都是引用的形式*/
            SentenceAlignment &m_sentence;/**翻译的句子*/
            const PhraseExtractionOptions &m_options;/**翻译的配置*/
            Moses::OutputFileStream &m_extractFile;/**输出短语表*/
            Moses::OutputFileStream &m_extractFileInv;/**输出反向翻译的短语表*/
            Moses::OutputFileStream &m_extractFileOrientation;/**输出调序位置的信息*/
            Moses::OutputFileStream &m_extractFileContext;
            Moses::OutputFileStream &m_extractFileGraph;
    };
}

int main(int argc, char *argv[])
{
    //抽短语
    cerr    << "PhraseExtract v1.4, written by Philipp Koehn,Modified by Tzy\n"
        << "phrase extraction from an aligned parallel corpus\n";
    //命令: moses中使用的是extract en de align extract max-length orientation --model wbe-msd --GZOutput
    if (argc < 5)
    {
        cerr << "[Usage]: extract en de align output_prefix max-length \n";
        // 默认的是orientation --model wbe-msd --GZOutput
        exit(1);
    }
    /**三种不同的输出流*/
    Moses::OutputFileStream extractFile;
    Moses::OutputFileStream extractFileInv;
    Moses::OutputFileStream extractFileOrientation;
    Moses::OutputFileStream extractFileContext;
    Moses::OutputFileStream extractFileGraph;
    const char *const &fileNameE = argv[1];
    const char *const &fileNameF = argv[2];
    const char *const &fileNameA = argv[3];
    /*输入文件 */
    const string fileNameExtract = string(argv[4]);

    /* 初始化一些抽取的候选项 */
    PhraseExtractionOptions options(atoi(argv[5]));
    // open input files
    Moses::InputFileStream eFile(fileNameE);
    Moses::InputFileStream fFile(fileNameF);
    Moses::InputFileStream aFile(fileNameA);

    istream *eFileP = &eFile;
    istream *fFileP = &fFile;
    istream *aFileP = &aFile;
    // open output files
    if (options.isTranslationFlag())//yes
    {
        //yes
        string fileNameExtractInv = fileNameExtract + ".inv" + (options.isGzOutput() ? ".gz" : "");
        extractFile.Open( (fileNameExtract + (options.isGzOutput() ? ".gz" : "")).c_str());
        extractFileInv.Open(fileNameExtractInv.c_str());
        const string& fileNameExtractContext = string(argv[4])+".ctx"+(options.isGzOutput()?".gz":"");
        extractFileContext.Open(fileNameExtractContext);
        extractFileGraph.Open(fileNameExtract+".dot");
    }
    if (options.isOrientationFlag())//yes
    {
        string fileNameExtractOrientation = fileNameExtract + ".o" + (options.isGzOutput() ? ".gz" : "");
        extractFileOrientation.Open(fileNameExtractOrientation.c_str());
    }

    int i = sentenceOffset;
    while (true)
    {
        i++;
        if (i % 10000 == 0) cerr << "." << flush;
        char englishString[LINE_MAX_LENGTH];
        char foreignString[LINE_MAX_LENGTH];
        char alignmentString[LINE_MAX_LENGTH];
        //读一行
        SAFE_GETLINE((*eFileP), englishString, LINE_MAX_LENGTH, '\n', __FILE__);
        if (eFileP->eof()) break;

        SAFE_GETLINE((*fFileP), foreignString, LINE_MAX_LENGTH, '\n', __FILE__);

        SAFE_GETLINE((*aFileP), alignmentString, LINE_MAX_LENGTH, '\n', __FILE__);


        SentenceAlignment sentence;

        //关键函数
        if (sentence.create( englishString, foreignString, alignmentString,i))
        {
            ExtractTask *task = new ExtractTask(i - 1, sentence, options, extractFile , extractFileInv, extractFileOrientation,extractFileContext,extractFileGraph);
            task->Run();
            delete task;
        }
    }

    eFile.Close();
    fFile.Close();
    aFile.Close();

    //az: only close if we actually opened it
    if (options.isTranslationFlag())//yes
    {
        extractFile.Close();
        extractFileInv.Close();

    }
    if (options.isOrientationFlag())//yes
    {
        extractFileOrientation.Close();
    }
}

namespace MosesTraining
{
    void ExtractTask::Run()
    {
        //抽取
        extract(m_sentence);
        extractContext();
        writePhrasesToFile();
        writeContextFile();
        writeGraphFile();
        //清空位置
        m_extractedPhrases.clear();
        m_extractedPhrasesInv.clear();
        m_extractedPhrasesOri.clear();
    }
    void ExtractTask::extractContext()
    {
        unsigned int startPhrase = 0 ;
        for(; startPhrase < this->r_phrasePositions.size(); startPhrase++)
        {
            //两两比较，确定关系
            const vector<int>& prevPhrasePositions = this->r_phrasePositions[startPhrase];
            int prevStartE = prevPhrasePositions[0];
            int prevEndE = prevPhrasePositions[1];
            int prevStartF = prevPhrasePositions[2];
            int prevEndF = prevPhrasePositions[3];
            //cerr << prevStartE <<" "<< prevEndE<<" "<< prevStartF<<" "<< prevEndF<<endl;
            for(unsigned int endPhrase = startPhrase+1; endPhrase < this->r_phrasePositions.size(); endPhrase++)
            {
                const vector<int>& nextPhrasePositions = this->r_phrasePositions[endPhrase];
                int nextStartE = nextPhrasePositions[0];
                int nextEndE = nextPhrasePositions[1];
                int nextStartF = nextPhrasePositions[2];
                int nextEndF = nextPhrasePositions[3];
                //以后，写成一个抽象类
                //第一种关系，s2和t2，边界左相邻，或者右相邻
                if((nextEndE == prevStartE-1 && nextEndF == prevStartF-1)//左相邻
                    ||(nextStartE == prevEndE+1 && nextStartF == prevEndF+1))//右相邻
                    {
                        this->r_phrasePositions[startPhrase].push_back(endPhrase);//前4个是位置的key,后面的都是相关的ctx
                       // this->r_phrasePositions[endPhrase].push_back(startPhrase);//对称的
                    }
                else if((nextStartE == prevEndE+1 && nextEndF == prevStartF-1)||//交叉相邻 s2 t2_1
                        (nextEndE == prevStartE-1 && nextStartF == prevEndF+1))
                {
                        this->r_phrasePositions[startPhrase].push_back(endPhrase);//前4个是位置的key,后面的都是相关的ctx
                       // this->r_phrasePositions[endPhrase].push_back(startPhrase);//对称的
                }
                else if(
                        (
                            (nextStartE == prevStartE && nextEndE<= prevEndE)||
                            (nextStartE >= prevStartE && nextEndE == prevEndE)
                        )
                        &&
                         (
                            (nextStartF == prevStartF && nextEndF<= prevEndF)||//in s3 t3
                            (nextEndF == prevEndF && nextStartF >= prevStartF)//s3 t3'
                         )
                        )
                {
                       // cout << prevStartE<<" "<< prevEndE<<" "<< prevStartF<<" "<< prevEndF<<" ||| ";
                       // cout << nextStartE<<" "<< nextEndE<<" "<< nextStartF<<" "<< nextEndF<<endl;
                        this->r_phrasePositions[startPhrase].push_back(endPhrase);//前4个是位置的key,后面的都是相关的ctx
                       // this->r_phrasePositions[endPhrase].push_back(startPhrase);//对称的
                }
                else if(//s4 t4
                         (
                            (nextStartE == prevStartE && nextEndE>=prevEndE)||
                            (nextStartE <=prevStartE && nextEndE == prevEndE)
                         )
                         &&
                         (
                            (nextStartF == prevStartF && nextEndF >= prevEndF)||
                            (nextStartF <=prevStartF && nextEndF == prevEndF)
                         )
                    )
                {
                     this->r_phrasePositions[startPhrase].push_back(endPhrase);//前4个是位置的key,后面的都是相关的ctx
                     //this->r_phrasePositions[endPhrase].push_back(startPhrase);//对称的
                }
            }
        }
    }
    void ExtractTask::extract(SentenceAlignment &sentence)
    {
        int countE = sentence.target.size();
        int countF = sentence.source.size();

        // check alignments for target phrase startE...endE
        // loop over extracted phrases which are compatible with the word-alignments
        for (int startE = 0; startE < countE; startE++)
        {
            for (int endE = startE;
                    endE < countE && endE < startE + m_options.maxPhraseLength;
                    endE++)//最大长度指的是target端的啊
            {

                int minF = 9999;
                int maxF = -1;
                vector< int > usedF = sentence.alignedCountS; //注意这个地方是复制
                //1, 找到[start,end]对齐到的f的区间，f对齐的次数减减，如果f不为0，那就是不合法的
                for (int ei = startE; ei <= endE; ei++) //这个地方，自己实现的时候，出了点错
                {
                    for (size_t i = 0; i < sentence.alignedToT[ei].size(); i++)
                    {
                        int fi = sentence.alignedToT[ei][i];
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

                if (maxF >= 0 && // aligned to any source words at all
                        (maxF - minF < m_options.maxPhraseLength)) // source phrase within limits
                {

                    // check if source words are aligned to out of bound target words
                    bool out_of_bounds = false;
                    for (int fi = minF; fi <= maxF && !out_of_bounds; fi++)
                        if (usedF[fi] > 0)
                        {
                            // cout << "ouf of bounds: " << fi << "\n";
                            out_of_bounds = true;
                        }

                    // cout << "doing if for ( " << minF << "-" << maxF << ", " << startE << "," << endE << ")\n";
                    if (!out_of_bounds)
                    {
                        // start point of source phrase may retreat over unaligned
                        for (int startF = minF;
                                (startF >= 0 &&
                                 (startF > maxF - m_options.maxPhraseLength) && // within length limit
                                 (startF == minF || sentence.alignedCountS[startF] == 0)); // unaligned
                                startF--)
                            // end point of source phrase may advance over unaligned
                            for (int endF = maxF;
                                    (endF < countF &&
                                     (endF < startF + m_options.maxPhraseLength) && // within length limit
                                     (endF == maxF || sentence.alignedCountS[endF] == 0)); // unaligned
                                    endF++)   // at this point we have extracted a phrase
                            {
                                //false
                                string orientationInfo = "";
                                //计算orientationInfo
                                if (m_options.isWordModel())//yes
                                {
                                    REO_POS wordPrevOrient, wordNextOrient;
                                    bool connectedLeftTopP  = isAligned( sentence, startF - 1, startE - 1 );
                                    bool connectedRightTopP = isAligned( sentence, endF + 1,   startE - 1 );
                                    bool connectedLeftTopN  = isAligned( sentence, endF + 1, endE + 1 );
                                    bool connectedRightTopN = isAligned( sentence, startF - 1,   endE + 1 );
                                    wordPrevOrient = getOrientWordModel( connectedLeftTopP, connectedRightTopP);
                                    wordNextOrient = getOrientWordModel( connectedLeftTopN, connectedRightTopN);
                                    orientationInfo += getOrientString(wordPrevOrient, m_options.isWordType()) + " " + getOrientString(wordNextOrient, m_options.isWordType());
                                }
                                addPhrase(sentence, startE, endE, startF, endF, orientationInfo);

                                addPosition(startE,endE,startF,endF);
                                
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
    void ExtractTask::addPosition(int startE, int endE, int startF, int endF)
    {
        vector<int> current_position;
        
        current_position.push_back(startE);
        current_position.push_back(endE);
        current_position.push_back(startF);
        current_position.push_back(endF);

        this->r_phrasePositions.push_back(current_position);
    }
    void ExtractTask::output(int startE, int endE, int startF, int endF,string& res) const
    {
        ostringstream sout;
        sout<<"\"";
        for(int fi = startF; fi <= endF; fi++){

           sout<<this->m_sentence.source[fi];
           if(fi != endF) sout<<"_";
        }
        sout<<"_";
        for(int ei = startE; ei <= endE; ei++){
            sout<<this->m_sentence.target[ei];
            if(ei != endE) sout<<"_";
        }
        sout<<"\"";
        res = sout.str();
    }
    void ExtractTask::addPhrase( SentenceAlignment &sentence, int startE, int endE, int startF, int endF , string &orientationInfo)
    {
        // source
        //   // cout << "adding ( " << startF << "-" << endF << ", " << startE << "-" << endE << ")\n";
        ostringstream outextractstr;
        ostringstream outextractstrInv;
        ostringstream outextractstrOrientation;

        for (int fi = startF; fi <= endF; fi++)//输出源端
        {
            if (m_options.isTranslationFlag()) outextractstr << sentence.source[fi] << " ";
            if (m_options.isOrientationFlag()) outextractstrOrientation << sentence.source[fi] << " ";
        }
        if (m_options.isTranslationFlag()) outextractstr << "||| ";
        if (m_options.isOrientationFlag()) outextractstrOrientation << "||| ";

        // target
        for (int ei = startE; ei <= endE; ei++)//输出目标端
        {
            if (m_options.isTranslationFlag()) outextractstr << sentence.target[ei] << " ";
            if (m_options.isTranslationFlag()) outextractstrInv << sentence.target[ei] << " ";
            if (m_options.isOrientationFlag()) outextractstrOrientation << sentence.target[ei] << " ";
        }
        if (m_options.isTranslationFlag()) outextractstr << "|||";
        if (m_options.isTranslationFlag()) outextractstrInv << "||| ";
        if (m_options.isOrientationFlag()) outextractstrOrientation << "||| ";

        // source (for inverse)

        if (m_options.isTranslationFlag()) //反向的输出源端
        {
            for (int fi = startF; fi <= endF; fi++)
                outextractstrInv << sentence.source[fi] << " ";
            outextractstrInv << "|||";
        }
        // alignment
        if (m_options.isTranslationFlag())//输出对齐信息，并没有全局合并啊
        {
            for (int ei = startE; ei <= endE; ei++)
            {
                for (unsigned int i = 0; i < sentence.alignedToT[ei].size(); i++)
                {
                    int fi = sentence.alignedToT[ei][i];
                    outextractstr << " " << fi - startF << "-" << ei - startE;
                    outextractstrInv << " " << ei - startE << "-" << fi - startF;
                }
            }
        }

        if (m_options.isOrientationFlag())//yes
            outextractstrOrientation << orientationInfo;

        if (m_options.isTranslationFlag()) outextractstr << "\n";
        if (m_options.isTranslationFlag()) outextractstrInv << "\n";
        if (m_options.isOrientationFlag()) outextractstrOrientation << "\n";


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
        //outextractGraph<<"<node count=\""<<this->r_phrasePositions.size()<<"\">\n";
        vector<string> labels(this->r_phrasePositions.size());
        //output node 
        unsigned int phraseIndex = 0 ;
        for(;phraseIndex < this->r_phrasePositions.size(); phraseIndex++)
        {
        //   if(labels[phraseIndex].size()==0)
                this->output(this->r_phrasePositions[phraseIndex][0],\
                             this->r_phrasePositions[phraseIndex][1],\
                             this->r_phrasePositions[phraseIndex][2],\
                             this->r_phrasePositions[phraseIndex][3],\
                             labels[phraseIndex]);
         //  outextractGraph<<"<node id=\""<<phraseIndex<<"\" label=\""<<labels[phraseIndex]<<"\"/>\n";
        }
        //outextractGraph<<"</node>\n";
        //this->m_extractFileGraph<<outextractGraph.str();
        //outextractGraph.str(""); */
        outextractGraph<<"graph {\n";
        outextractGraph<<"edge [fontname=\"FangSong\"];\n";
        outextractGraph<<"node [fontname=\"FangSong\" size=\"20,20\"];\n";
        //output edges
        //int edgeId = 0;
        for(phraseIndex = 0 ; phraseIndex < this->r_phrasePositions.size(); phraseIndex++)
        {
            if( this->r_phrasePositions[phraseIndex].size()<5) continue;
            for(unsigned int relatedIndex = 4; relatedIndex < this->r_phrasePositions[phraseIndex].size(); relatedIndex++)
            {
               int relatedPhraseIndex = this->r_phrasePositions[phraseIndex][relatedIndex];
               //outextractGraph<<"<edge id=\""<<edgeId<<"\" source=\""<<phraseIndex<<"\" target=\""<<relatedPhraseIndex<<"\"/>\n";
                //edgeId++;
               outextractGraph << labels[phraseIndex]<<"--"<<labels[relatedPhraseIndex]<<";\n";
            }
        }
        //this->m_extractFileGraph<<"<edges count=\""<<edgeId<<"\">\n"<<outextractGraph.str()<<"</edges>\n</graph>\n</gexf>\n";
        outextractGraph<<"}";
        this->m_extractFileGraph<<outextractGraph.str();
    } 
    void ExtractTask::writeContextFile()
    {
        unsigned int phraseIndex = 0;
        ostringstream outextractContext;
        for( ;phraseIndex < this->r_phrasePositions.size(); phraseIndex++)
        {
            vector<int>& positions = this->r_phrasePositions[phraseIndex];
            //根据前四个位置关系，输出一下
            string prevString;
            this->output(positions[0],positions[1],positions[2],positions[3],prevString);
            outextractContext<<prevString<<" ||| " ;

            if(positions.size() < 5) {
                outextractContext<<endl;
                continue;
            } 
            unsigned int relatedPhraseIndex = 4u;
            //sort(positions.begin()+4,positions.end());
            for(; relatedPhraseIndex < positions.size(); relatedPhraseIndex++)
            {
                const vector<int>& nextPositions = this->r_phrasePositions[positions[relatedPhraseIndex]];
                string nextString;
                this->output(nextPositions[0],nextPositions[1],nextPositions[2],nextPositions[3],nextString);
                outextractContext<<nextString;
                //根据位置关系输出一下
            }
            outextractContext<<endl;
        }
        this->m_extractFileContext<<outextractContext.str();
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

        m_extractFile << outextractFile.str();
        m_extractFileInv  << outextractFileInv.str();
        m_extractFileOrientation << outextractFileOrientation.str();
    }
}
