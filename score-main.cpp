/*
 * =====================================================================================
 *
 *       Filename:  score-main.cpp
 *
 *    Description:  compute one direction lexical score and collect phrase count
 *
 *        Version:  1.0
 *        Created:  03/15/2013 10:00:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *   Organization:  ICT.CAS
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <iostream>
#include <string>
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include "HalfScorer.h"

using namespace std;
using namespace MosesTraining;
using namespace Rabbit;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
    int
main ( int argc, char *argv[] )
{   
    if( argc < 4|| argc >5)
    {
        cerr<<"Usage: score extract lexical output.half [--Inverse]"<<endl;
        cerr<<"\t--Inverse : Inverse Phrase Tables"<<endl;
        return -1;
    }
    const string& extractFilePath = argv[1];
    const string& lexicalFilePath = argv[2];
    const string& outputFilePath = argv[3];
    bool inverseFlag = false;
    if( argc > 4)
    {
        if(0==strcmp(argv[4],"--Inverse"))
            inverseFlag = true;
    }
    InputFileStream extractFile(extractFilePath);
    if(extractFile.fail()){
        cerr<<"ERROR: could not open file :"<< extractFilePath << endl;
        return -1;
    }
    InputFileStream lexicalFile(lexicalFilePath);
    if(lexicalFile.fail()){
        cerr<<"ERROR: could not open file : "<< lexicalFilePath << endl;
        return -1;
    }
    OutputFileStream outputFile;
    if(!outputFile.open(outputFilePath.c_str())){
        cerr<<"ERROR: could not write to file :"<< outputFilePath << endl;
        return -1;
    }
    HalfScorer scorer(extractFile,lexicalFile,outputFile,inverseFlag);
    scorer.score();
    extractFile.Close();
    lexicalFile.Close();
    outputFile.Close():
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */


