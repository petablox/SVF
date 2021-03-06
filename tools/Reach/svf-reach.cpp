//===- svf-ex.cpp -- A driver example of SVF-------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // A driver program of SVF including usages of SVF APIs
 //
 // Author: Yulei Sui,
 */

#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "WPA/TypeAnalysis.h"
#include "Util/BasicTypes.h"
#include "SVF-FE/LLVMUtil.h"
#include "WPA/FlowSensitive.h"

#include <set>
#include <fstream>

using namespace llvm;
using namespace std;

static llvm::cl::opt<std::string> InputFilename(cl::Positional, llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));
static llvm::cl::opt<string> OutputFilename("o", cl::desc("Specify output filename"), cl::value_desc("filename"));
static llvm::cl::opt<string> AnalType("t", cl::desc("Analysis type"), cl::value_desc("type"));
static llvm::cl::opt<string> EntryModule("m", cl::desc("Entry module"), cl::value_desc("module bc"), llvm::cl::init("main"));

bool reachable(PTACallGraphNode *n, std::set<std::string> *reachables) {
  if (EntryModule == "main") {
    return n->isReachableFromProgEntry();
  } else {
    return n->isReachableFromModule(reachables);
  } 
}

void chomp(string &s) {
  int pos;
  if((pos=s.find('\n')) != string::npos)
    s.erase(pos);
}

int main(int argc, char ** argv) {

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    PointerAnalysis* pta = NULL;
    if (AnalType.compare("type") == 0) {
      std::cout << "Using type call graph" << std::endl;
      pta = TypeAnalysis::createTypeAnalysis(svfModule);
    } else if (AnalType.compare("nanders") == 0) {
      std::cout << "Using nanders call graph" << std::endl;
      pta = Andersen::createAndersen(svfModule);
    }
    else if (AnalType.compare("flow") == 0) {
      std::cout << "Using flow call graph" << std::endl;
      pta = FlowSensitive::createFSWPA(svfModule);
    }
    else if (AnalType.compare("anders") == 0) {
      std::cout << "Using anders call graph" << std::endl;
      pta = AndersenWaveDiff::createAndersenWaveDiff(svfModule);
    } else {
      std::cout << "unrecognize type " << AnalType << std::endl;
      return 0;
    } 

    std::set<std::string> modules;
    std::set<std::string> reachables;
    if (EntryModule != "main") {
      std::ifstream infile(EntryModule);
      std::string str;
      while (std::getline(infile, str)) {
        std::cout << "Using " << str << std::endl;
        reachables.insert(str);
      }
    }

    std::cout << "Starting reachability..." << std::endl; 

    /// Call Graph
    PTACallGraph* graph = pta->getPTACallGraph();

    PTACallGraph::iterator it = graph->begin();
    PTACallGraph::iterator eit = graph->end();
    for (; it != eit; ++it) { 
      PTACallGraphNode *n = it->second;

      if (!reachable(n, &reachables)) 
        continue;

      const SVFFunction *f = n->getFunction();
      Function *llvmFun = f->getLLVMFun();
      Module *mod = llvmFun->getParent();

      std::string name = mod->getModuleIdentifier();
      if (modules.find(name) == modules.end()) {
        modules.insert(name);
      }
    }

    std::ofstream mfile;
    mfile.open(OutputFilename.c_str());
    std::string trim(".bc");

    for (auto it = modules.begin(); it != modules.end(); it++) {
      std::string name = *it;

      std::string::size_type i = name.find(trim);

      if (i != std::string::npos)
           name.erase(i, trim.length());

      std::cout << "Reached " << name << std::endl;
      mfile << name << std::endl;
    }

    mfile.close();

    return 0;
}

