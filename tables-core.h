// $Id$

#ifndef _TABLES_H
#define _TABLES_H

#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <map>
#include <cmath>

namespace MosesTraining
{

    typedef std::string WORD;
    typedef unsigned int WORD_ID;
    const long int LINE_MAX_LENGTH = 500000 ;

    class Vocabulary
    {
        public:
            WORD_ID storeIfNew( const WORD& );
            WORD_ID getWordID( const WORD& );
            inline WORD &getWord( WORD_ID id ) {
                return vocab[ id ];
            }
            bool load(const std::string& tablePath);
            friend std::ostream& operator<<(std::ostream& out,const Vocabulary& voc);
        private:
            std::map<WORD, WORD_ID>  lookup;
            std::vector< WORD > vocab;
    };
    // add by tzy 
    /**
      class SimpleVocabulary
      {
      public:
      WORD_ID storeIfNew(const WORD& );
      friend std::ostream& operator<<(std::ostream& out,const SimpleVocabulary& voc);
      private:
      std::map<WORD,WORD_ID> lookup;
      };*/

    typedef std::vector< WORD_ID > PHRASE;
    typedef unsigned int PHRASE_ID;

    class PhraseTable
    {
        public:
            PHRASE_ID storeIfNew( const PHRASE& );
            PHRASE_ID getPhraseID( const PHRASE& );
            void clear();
            inline PHRASE &getPhrase( const PHRASE_ID id ) {
                return phraseTable[ id ];
            }
            friend std::ostream& operator << (std::ostream& out , const PhraseTable& );
        private:
            std::map< PHRASE, PHRASE_ID > lookup;
            std::vector< PHRASE > phraseTable;
    };
    typedef std::pair< PHRASE_ID , PHRASE_ID > RULE;
    typedef unsigned int RULE_ID;  
    //add by tzy 
    class RuleTable
    {
        public: 
            RULE_ID storeIfNew( const RULE& );
            RULE_ID getRuleID( const RULE&);
            void clear();
            inline RULE & getRule( const RULE_ID id ) {
                return ruleTable[ id ]; 
            }
            friend std::ostream& operator << (std::ostream& out, const RuleTable& );
        private:
            std::map< RULE , RULE_ID> lookup;
            std::vector< RULE > ruleTable;
    };
    //typedef std::vector< std::pair< PHRASE_ID, double > > PHRASEPROBVEC;
    /*
       class TTable
       {
       public:
       std::map< PHRASE_ID, std::vector< std::pair< PHRASE_ID, double > > > ttable;
       std::map< PHRASE_ID, std::vector< std::pair< PHRASE_ID, std::vector< double > > > > ttableMulti;
       };

       class DTable
       {
       public:
       std::map< int, double > dtable;
       void init();
       void load( const std::string& );
       double get( int );
       };*/

}

#endif
