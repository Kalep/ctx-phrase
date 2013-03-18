OBJDIR=bin
all: ObjDir nb extract lexnb

nb : InputFileStream.o OutputFileStream.o nb-main.o StringHelper.o Numberizor.o tables-core.o 
		g++ -g -o nb $(OBJDIR)/InputFileStream.o $(OBJDIR)/OutputFileStream.o $(OBJDIR)/nb-main.o $(OBJDIR)/Numberizor.o $(OBJDIR)/StringHelper.o $(OBJDIR)/tables-core.o -lz -lboost_iostreams

extract:InputFileStream.o OutputFileStream.o RuleGraphExtractor.o extract-main.o tables-core.o SentenceAlignment.o ExtractTask.o ContextFeature.o
		g++ -g -o extract $(OBJDIR)/InputFileStream.o $(OBJDIR)/OutputFileStream.o $(OBJDIR)/extract-main.o $(OBJDIR)/tables-core.o $(OBJDIR)/RuleGraphExtractor.o $(OBJDIR)/SentenceAlignment.o $(OBJDIR)/ExtractTask.o $(OBJDIR)/ContextFeature.o -lz -lboost_iostreams 

lexnb:  tables-core.o lexnb-main.o
	g++ -g -o lexnb $(OBJDIR)/tables-core.o $(OBJDIR)/lexnb-main.o

extract-main.o: extract-main.cpp
		g++ -Wall -O3 -g -c extract-main.cpp -o $(OBJDIR)/extract-main.o
lexnb-main.o : lexnb-main.cpp
		g++ -Wall -O3 -g -c lexnb-main.cpp -o $(OBJDIR)/lexnb-main.o

RuleGraphExtractor.o : RuleGraphExtractor.h RuleGraphExtractor.cpp
		g++ -Wall -O3 -g -c RuleGraphExtractor.cpp -o $(OBJDIR)/RuleGraphExtractor.o

SentenceAlignment.o : SentenceAlignment.h SentenceAlignment.cpp
		g++ -Wall -O3 -g -c SentenceAlignment.cpp -o $(OBJDIR)/SentenceAlignment.o
ObjDir:
	mkdir -p $(OBJDIR)

nb-main.o : nb-main.cpp
	g++ -Wall -O3 -g -c nb-main.cpp -o $(OBJDIR)/nb-main.o

InputFileStream.o : InputFileStream.h InputFileStream.cpp 
	g++ -Wall -O3 -g -c InputFileStream.cpp -o $(OBJDIR)/InputFileStream.o

OutputFileStream.o : OutputFileStream.h OutputFileStream.cpp gzfilebuf.h
	g++ -Wall -O3 -g -c OutputFileStream.cpp -o $(OBJDIR)/OutputFileStream.o

Numberizor.o : Numberizor.h Numberizor.cpp
	g++ -Wall -O3 -g -c Numberizor.cpp -o $(OBJDIR)/Numberizor.o
StringHelper.o : StringHelper.cpp StringHelper.h
	g++ -Wall -O3 -g -c StringHelper.cpp -o $(OBJDIR)/StringHelper.o
tables-core.o: tables-core.h tables-core.cpp
	g++ -Wall -O3 -g -c tables-core.cpp -o $(OBJDIR)/tables-core.o
ExtractTask.o: ExtractTask.h ExtractTask.cpp
	g++ -Wall -O3 -g -c ExtractTask.cpp -o $(OBJDIR)/ExtractTask.o
ContextFeature.o: ContextFeature.h ContextFeature.cpp
	g++ -Wall -O3 -g -c ContextFeature.cpp -o $(OBJDIR)/ContextFeature.o
clean :
	rm nb $(OBJDIR)/ -rf

