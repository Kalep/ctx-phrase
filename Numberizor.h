#ifndef _NUMBERIZOR_H
#define _NUMBERIZOR_H
#include "tables-core.h"
#include <string>
#include "InputFileStream.h"
#include "OutputFileStream.h"

namespace Rabbit{
    class Numberizor{
        public:
            Numberizor(Moses::InputFileStream& input,Moses::OutputFileStream& vocOutput,Moses::OutputFileStream& srcOutput );
            void numberize();
        private:
            void numberize(const std::string& line); 
            MosesTraining::Vocabulary r_simpleVoc; /**词表文件*/
            Moses::InputFileStream& r_inputStream;/**输入流*/
            Moses::OutputFileStream& r_vocOutput; /**词表输出流*/
            Moses::OutputFileStream& r_srcOutput; /**源文件数字化的输出流*/
    };
}
#endif
