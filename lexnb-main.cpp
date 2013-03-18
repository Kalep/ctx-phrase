/*
 * =====================================================================================
 *
 *       Filename:  lexnb-main.cpp
 *
 *    Description:  lexical numberize module
 *
 *        Version:  1.0
 *        Created:  03/12/2013 04:16:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *        Company:  ICT.CAS
 *
 * =====================================================================================
 */
#include "tables-core.h"
#include <sstream>
using namespace MosesTraining;
using namespace std;

    int
main ( int argc, char *argv[] )
{   
    if(argc!=4){
        cerr<<"[Usage]:./lexnb lexical-translation-file source-vocabulary-file target-vocabulary-file"<<endl;
        cerr<<"\t\tlexical-translation-file:\t format source-word target-word prob"<<endl;
        cerr<<"\t\tsource-vocabulary-file  :\t format one word per line"<<endl;
        cerr<<"\t\ttarget-vocabulary-file  :\t format one word per line"<<endl;
        return -1;
    }
    Vocabulary sourceVocabulary,targetVocabulary;
    ifstream fin(argv[1]);
    if(!fin){   
        cerr<<"ERROR: Could not open file "<< argv[1]<<endl;
        return -1;
    }
    if(!sourceVocabulary.load(argv[2]))
        return -1;
    if(!targetVocabulary.load(argv[3]))
        return -1;
    ofstream fout((string(argv[1])+".nb").c_str());
    if(!fout){
        cerr<<"ERROR: Could not write to file "<< argv[1] <<".nb"<<endl;
        return -1;
    }
    int line = 1 ;
    ostringstream sout;
    string source,target;
    double prob;
    while(fin>>source>>target>>prob)
    {
        if(0==line%999) {
            cout <<"line: "<<line<<endl;
            fout<< sout.str();
            sout.str("");
        }
        WORD_ID sourceId = sourceVocabulary.getWordID(source);
        WORD_ID targetId = sourceVocabulary.getWordID(target);
        sout<<sourceId<<" "<<targetId<<" "<<prob<<endl;
        line++;
    }
    fout<<sout.str();
    fout.close();
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

