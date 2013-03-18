/***********************************************************************
  Moses - factored phrase-based language decoder
  Copyright (C) 2010 University of Edinburgh

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***********************************************************************/

/* Created by Rohit Gupta, CDAC, Mumbai, India on 18 July, 2012*/

#pragma once
#ifndef PHRASEEXTRACTIONOPTIONS_H_INCLUDED_
#define PHRASEEXTRACTIONOPTIONS_H_INCLUDED_

namespace MosesTraining
{
enum REO_MODEL_TYPE {REO_MSD};
enum REO_POS {LEFT, RIGHT, UNKNOWN};


class PhraseExtractionOptions
{

public:
    const int maxPhraseLength;
private:
    bool allModelsOutputFlag;
    bool wordModel;
    REO_MODEL_TYPE wordType;
    bool orientationFlag;
    bool translationFlag;
    bool gzOutput;

public:
    PhraseExtractionOptions(const int initmaxPhraseLength):
        maxPhraseLength(initmaxPhraseLength),
        allModelsOutputFlag(true),
        wordModel(true),
        wordType(REO_MSD),
        orientationFlag(true),
        translationFlag(true),
        gzOutput(false) {}

    //functions for initialization of options
    void initAllModelsOutputFlag(const bool initallModelsOutputFlag)
    {
        allModelsOutputFlag = initallModelsOutputFlag;
    }
    void initWordModel(const bool initwordModel)
    {
        wordModel = initwordModel;
    }
    void initWordType(REO_MODEL_TYPE initwordType )
    {
        wordType = initwordType;
    }
    void initOrientationFlag(const bool initorientationFlag)
    {
        orientationFlag = initorientationFlag;
    }
    void initGzOutput (const bool initgzOutput)
    {
        gzOutput = initgzOutput;
    }

    // functions for getting values
    inline bool isAllModelsOutputFlag() const
    {
        return allModelsOutputFlag;
    }
    inline bool isWordModel() const
    {
        return wordModel;
    }
    inline REO_MODEL_TYPE isWordType() const
    {
        return wordType;
    }
    inline bool isOrientationFlag() const
    {
        return orientationFlag;
    }
    inline bool isTranslationFlag() const
    {
        return translationFlag;
    }
    bool isGzOutput () const
    {
        return gzOutput;
    }
};

}

#endif
