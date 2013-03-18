/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  numberize module for source and target vocabulary
 *
 *        Version:  1.0
 *        Created:  03/07/2013 03:24:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *   Organization:  ICT.CAS
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <string>
#include <iostream>
#include "Numberizor.h"

using namespace Rabbit;
using namespace MosesTraining;
using namespace std;
using namespace Moses;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  Entry of numberize
 * =====================================================================================
 */
    int
main ( int argc, char *argv[] )
{   
    if(argc !=3 ) {
        cerr<<"[Usage]: nb single output_prefix\n";
        return EXIT_SUCCESS;
    }
    const string& input = argv[1];
    const string& output = argv[2];
    InputFileStream sourceInputStream(input);
    if(sourceInputStream.fail())
    {
        cerr << "ERROR: could not open file: " << input << endl;
        exit(1);
    }
    OutputFileStream sourceOutputStream;
    if(!sourceOutputStream.Open(output+".nb"))
    {
        cerr << "ERROR: could not write to file: "<< output <<".nb.gz"<<endl;
        exit(1);
    }
    OutputFileStream vocabOutputStream;
    if(!vocabOutputStream.Open(output+".vcb"))
    {
        cerr << "ERROR: could not write to file: "<< output <<".vcb.gz"<<endl;
        exit(1);    
    }
    Numberizor nb(sourceInputStream,vocabOutputStream,sourceOutputStream);
    nb.numberize();
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

