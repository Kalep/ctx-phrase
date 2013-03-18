/*
 * =====================================================================================
 *
 *       Filename:  extract-main.cpp
 *
 *    Description:  extract base phrase from given alignment
 *
 *        Version:  1.0
 *        Created:  03/08/2013 09:33:35 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tengzhiyang@ict.ac.cn
 *        Company:  ICT.CAS
 *
 * =====================================================================================
 */
#include "RuleGraphExtractor.h"
#include "InputFileStream.h"
#include "OutputFileStream.h"
#include <iostream>
using namespace Rabbit;
using namespace Moses;
using namespace std;

#define OPEN(OBJ,PATH) InputFileStream OBJ(PATH);\
    if(OBJ.fail()){ \
        cerr<<"ERROR:Could not open file:"<< PATH <<endl;\
        return EXIT_SUCCESS;\
    }
#define WRITE(OBJ,PATH) OutputFileStream OBJ;\
    if(!OBJ.Open(PATH.c_str())){\
        cerr<<"ERROR:Could not write to file : "<< PATH <<endl; \
        return EXIT_SUCCESS; \
    }

    int
main ( int argc, char *argv[] )
{   
    if(argc != 7){
        cerr<<"[Usage]:extract targetPrefix sourcePrefix alignment outputPrefix featureMode maxLength"<<endl;
        cerr<<"\t--targetPrefix: target should be numberized, targetPrefix should point to prefix of numberized file"<<endl;
        cerr<<"\t--sourcePrefix: source should be numberized, sourcePrefix should point to prefix of numberized file"<<endl;
        cerr<<"\t--featureMode : 1 for close, 2 for in, 3,for close and in, 4 for cover,5 for close and cover,6 for in and cover,7 for all"<<endl;
        cerr<<"\t--maxLength   : phrase length limit " <<endl;
        return EXIT_SUCCESS;
    }
    /**1,打开3个输入文件*/
    const string& eFilePrefix = argv[1];
    const string& fFilePrefix = argv[2];
    const string& aFilePath = argv[3];
    const string& extractPrefix = argv[4];

    const string& nbPrefix = ".nb";
    const string& eFilePath = eFilePrefix+nbPrefix;
    OPEN(eFile,eFilePath)
    
    const string& fFilePath = fFilePrefix+nbPrefix;
    OPEN(fFile,fFilePath)
    
    OPEN(aFile,aFilePath)
 
    /**2,准备7个输出文件 */
    const string& extractSuffix = "";
    const string& extractPath = extractPrefix+extractSuffix;
    WRITE(extractFile,extractPath)
   
    const string& extractInvPath = extractPrefix+".inv"+extractSuffix;
    WRITE(extractFileInv,extractInvPath)
    
    const string& extractOriPath = extractPrefix+".o"+ extractSuffix;
    WRITE(extractFileOri, extractOriPath)

    const string& extractRelationPath = extractPrefix+".dot"+extractSuffix;
    WRITE(extractFileRelation, extractRelationPath)
    
    const string& extractGraphPath = extractPrefix+".ctx";
    WRITE(extractFileGraph,extractGraphPath)    
    //three vcb file    
    const string& targetPhraseVcbPath = eFilePrefix+".phrase.vcb"+extractSuffix;
    WRITE(vcbTargetPhrase,targetPhraseVcbPath)
    
    const string& sourcePhraseVcbPath = fFilePrefix+".phrase.vcb"+extractSuffix;
    WRITE(vcbSourcePhrase,sourcePhraseVcbPath)
    
    const string& ruleVcbPath = extractPrefix+".rule.vcb"+extractSuffix;
    WRITE(vcbRule, ruleVcbPath)
    
    /* *  3, 创建一个Extractor */
    RuleGraphExtractor ruleGraphExtractor(fFile,eFile,aFile,extractFile,extractFileInv,extractFileOri,extractFileRelation,extractFileGraph,atoi(argv[5]),atoi(argv[6]));
    /* *  4，抽取 */
    ruleGraphExtractor.extract();
    /* *  5, 保存序列化的词标文件 */
    ruleGraphExtractor.saveTables(vcbSourcePhrase,vcbTargetPhrase,vcbRule);
    /* *  6, 关闭10个文件 */
    eFile.Close();
    fFile.Close();
    aFile.Close();
    extractFile.Close();
    extractFileInv.Close();
    extractFileOri.Close();
    extractFileRelation.Close();
    extractFileGraph.Close();
    vcbTargetPhrase.Close();
    vcbSourcePhrase.Close();         
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
