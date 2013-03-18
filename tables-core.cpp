// $Id$
//#include "beammain.h"
#include "tables-core.h"
#include <iterator>
#include <algorithm>
#include <fstream>
#define TABLE_LINE_MAX_LENGTH 1000
#define UNKNOWNSTR	"UNK"

using namespace std;

namespace MosesTraining
{
    ostream& operator<<(ostream& fout, const Vocabulary& vocabulary)
    {
        copy(vocabulary.vocab.begin(), vocabulary.vocab.end(),ostream_iterator< WORD > (fout,"\n"));
        return fout;
    }
    /*
    bool isNonTerminal( const WORD &symbol ) {
        return symbol.substr(0, 1) == "[" && symbol.substr(symbol.size()-1, 1) == "]";
    }*/
    bool Vocabulary::load(const string& path)
    {
        ifstream fin(path.c_str());
        if(!fin){
            cerr<<"ERROR: Could not open file : "<< path << endl;
            return false;
        }
        unsigned int id = 0;
        string word;
        while(fin>>word)
        {
            lookup[word]=++id;
        }
        fin.close();
        return true;
    }
    WORD_ID Vocabulary::storeIfNew( const WORD& word )
    {
        map<WORD, WORD_ID>::iterator i = lookup.find( word );

        if( i != lookup.end() )
            return i->second;

        WORD_ID id = vocab.size()+1;
        vocab.push_back( word );
        lookup[ word ] = id;
        return id;
    }

    WORD_ID Vocabulary::getWordID( const WORD& word )
    {
        map<WORD, WORD_ID>::iterator i = lookup.find( word );
        if( i == lookup.end() )
            return 0;
        return i->second;
    }
    
    PHRASE_ID PhraseTable::storeIfNew( const PHRASE& phrase )
    {
        map< PHRASE, PHRASE_ID >::iterator i = lookup.find( phrase );
        if( i != lookup.end() )
            return i->second;

        PHRASE_ID id  = phraseTable.size()+1;
        phraseTable.push_back( phrase );
        lookup[ phrase ] = id;
        return id;
    }

    PHRASE_ID PhraseTable::getPhraseID( const PHRASE& phrase )
    {
        map< PHRASE, PHRASE_ID >::iterator i = lookup.find( phrase );
        if( i == lookup.end() )
            return 0;
        return i->second;
    }

    void PhraseTable::clear()
    {
        lookup.clear();
        phraseTable.clear();
    }

    ostream& operator<<(ostream& out , const PhraseTable& phrases)
    {
        vector< PHRASE >::const_iterator phraseIter = phrases.phraseTable.begin();
        for( ; phraseIter != phrases.phraseTable.end(); phraseIter++)
        {
            copy(phraseIter->begin(), phraseIter->end(), ostream_iterator< WORD_ID > (out, " "));
            out<<endl;
        }
        return out;
    }

    RULE_ID RuleTable::storeIfNew( const RULE& rule)
    {
        map< RULE , RULE_ID > ::iterator i = this->lookup.find(rule);
        if( i != lookup.end())
            return i->second;
        RULE_ID id = ruleTable.size();
        lookup [ rule ] = id;
        ruleTable.push_back(rule);
        return id;
    }

    RULE_ID RuleTable::getRuleID(const RULE& rule)
    {
        map< RULE , RULE_ID >::iterator i = lookup.find(rule);
        if( i == lookup.end() ) 
            return 0;
        return i->second;
    }

    void RuleTable::clear()
    {
        ruleTable.clear();
        lookup.clear();
    }

    ostream& operator<<(ostream& out ,const RuleTable& rules)
    {
        vector< RULE >::const_iterator ruleIter = rules.ruleTable.begin();
        for( ; ruleIter != rules.ruleTable.end() ;ruleIter++)
            out<< ruleIter->first << "\t" << ruleIter->second << endl;
        return out;
    }
    /* 
       void DTable::init()
       {
       for(int i = -10; i<10; i++)
       dtable[i] = -abs( i );
       }

       void DTable::load( const string& fileName )
       {
       ifstream inFile;
       inFile.open(fileName.c_str());

       std::string line;
       int i=0;
       while(true) {
       i++;
       getline(inFile, line);
       if (inFile.eof()) break;
       if (!inFile) {
       std::cerr << "Error reading from " << fileName << std::endl;
       abort();
       }

       vector<string> token = tokenize(line.c_str());
       if (token.size() < 2) {
       cerr << "line " << i << " in " << fileName << " too short, skipping\n";
       continue;
       }

       int d = atoi( token[0].c_str() );
       double prob = log( atof( token[1].c_str() ) );
       dtable[ d ] = prob;
       }
       }

       double DTable::get( int distortion )
       {
       if (dtable.find( distortion ) == dtable.end())
       return log( 0.00001 );
       return dtable[ distortion ];
       }*/

}

