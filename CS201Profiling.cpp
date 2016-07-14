/*
 * Authors:
 * Sachin Shinde <sshin031@ucr.edu> : 861243251
 * Nikhil Kamthe <nkamt001@ucr.edu> : 861245635
 *
 */

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"

#include <map>
#include <set>
#include <stack>

using namespace llvm;
using namespace std;

namespace {

  /*
   * The following method is used to generate the code for printf function.
   */
  static Function* printf_prototype(LLVMContext& ctx, Module *mod) {

      std::vector<Type*> printf_arg_types;
      printf_arg_types.push_back(Type::getInt8PtrTy(ctx));
      FunctionType* printf_type = FunctionType::get(Type::getInt32Ty(ctx), printf_arg_types, true);
      Function *func = mod->getFunction("printf");
      if(!func)
        func = Function::Create(printf_type, Function::ExternalLinkage, Twine("printf"), mod);
      func->setCallingConv(CallingConv::C);
      return func;
    }

  /*
   * The following subclass extends the function pass to perform basic program analysis and compute the profiling data for a given program.
   * We have implemented following profiling methods:
   * 1. Basic block profiling:
   *    In order to compute the basic block profiling data we have maintained global variables (of type one dimensional array) corresponding to each function.
   *    The corresponding variables are incremented as soon as the basic block is executed. This has been achieved by inserting an instruction at the start of 
   *    every basic block to increment the corresponding value in the global variable for the function using the name of the current basic block.
   *    Finally we print the basic block profiling data by printing the values of these global variables at the end of the main function.
   * 2. Edge profiling:
   *    In order to compute the edge profiling data we have maintained global variables (of type two dimensional integer array) corresponding to each function.
   *    The indices of these array correspond to the end points of the edge. We also maintain a global variable to store the index of previously executed block.
   *    At the start of the basic block using the index of current basic block and index of previously executed basic block we find and increment the counter for
   *    corresponding edge in the two dimensional map for a fucntion. Finally we print the edge profiling data by using these global variables at the end of the main function.
   * 3. Loop iteration count:
   *    In oder to compute the loop iteration count we simply use the edge execution count of the back edge of a loop. We store all the loops corresponding to a function
   *    while performing the basic program analysis. We then use this information to find the back edges and use the global variables used for edge profiling to get
   *    execution counts of these back edges.
   */
  struct CS201Profiling : public FunctionPass {

    static char ID;
    // Stores the context to be used later.
    LLVMContext *Context;
    // Stores the printf function which is then used to print profiling data.
    Function *printf_func = NULL;
    // Stores the global variable which is used to store the id of previously executed basic block.
    GlobalVariable *previousBlockID = NULL;
    // Stores the function name and the corresponding global variable of type one dimensional array (which stores the basic block count).
    map<string, GlobalVariable*> bbCounters;
    // Stores the function name and the corresponding set of basic blocks.
    map<string, set<string>> basicBlockMap;
    // Stores the function name and corresponding map of block names and corresponding block IDs.
    map<string, map<string, int>> blockIdMap;
    // Stores the function name and corresponding global variable of type two dimensional array (which stores the edge count).
    map<string, GlobalVariable*> edgeCounters;
    // Stores the name of the function and corresponding set of edges.
    map<string, set<pair<string, string>>> edgeMap;
    // Stores the name of the function and corresponding set of loops. 
    map<string, set<set<string>>> loopMap;

    CS201Profiling() : FunctionPass(ID) {}

    /*
     * The doInitialization method is allowed to do most of the things that FunctionPasses are not allowed to do. They can add and remove functions,
     * get pointers to functions, etc. The doInitialization method is designed to do simple initialization type of stuff that does not depend on the functions being processed.
     * The doInitialization method call is not scheduled to overlap with any other pass executions (thus it should be very fast).
     */
    bool doInitialization(Module &M) override {
      
      Context = &M.getContext();
      printf_func = printf_prototype(*Context, &M);

      errs() << "\nModule: " << M.getName() << "\n";
      for(auto &F : M) {

        int numBlocks = 0;
        for(auto &BB : F) {

          // The following code sets the name for each of the basic blocks.
          if (!BB.hasName()){
              BB.setName("b" + to_string(numBlocks));
          }
          numBlocks += 1;
        }

        // Here we create global variables (of type one dimensional array) to store basic block counts for each functions.
        ArrayType* singleArrayType = ArrayType::get(IntegerType::get(M.getContext(), 32), numBlocks);
        GlobalVariable* bbCountGVariable = new GlobalVariable(M, singleArrayType, false, GlobalValue::CommonLinkage, 0, "bbCounter_" + F.getName().str());;
        ConstantAggregateZero* singleArrayInitValues = ConstantAggregateZero::get(singleArrayType);
        bbCountGVariable->setInitializer(singleArrayInitValues);
        bbCounters[F.getName().str()] = bbCountGVariable;

        // Here we create global variables (of type two dimensional array) to store edge counts for each function.
        ArrayType* aType = ArrayType::get(ArrayType::get(IntegerType::get(M.getContext(), 32), numBlocks), numBlocks);
        GlobalVariable* gVariable = new GlobalVariable(M, aType, false, GlobalValue::CommonLinkage, 0, "edgeCounter_" + F.getName().str());
        ConstantAggregateZero* initValues = ConstantAggregateZero::get(aType);
        gVariable->setInitializer(initValues);
        edgeCounters[F.getName().str()] = gVariable;
      }

      // Here we create a global variable to (of type integer) to store id of previously executed block.
      previousBlockID = new GlobalVariable(M, Type::getInt32Ty(*Context), false, GlobalValue::InternalLinkage, ConstantInt::get(Type::getInt32Ty(*Context), 0), "previousBlockID");

      return true;
    }

    /*
     * The doFinalization method is an infrequently used method that is called when the pass framework has finished calling runOnFunction
     * for every function in the program being compiled.
     */
    bool doFinalization(Module &M) override {

      return false;
    }

    /*
     * The runOnFunction method is implemented by the subclass to do the transformation or analysis work of your pass.
     * A true value should be returned as the program is modified.
     */
    bool runOnFunction(Function &F) override {

      set<string> blocks;
      map<string, set<string>> predecessors;
      set<pair<string, string>> edges;
      map<string, set<string>> dominators;
      set<pair<string, string>> back_edges;
      set<set<string>> loops;
      map<string, int> block_ids;

      errs() << "\nFunction: " << F.getName() << '\n';

      populateBlockInfo(F, blocks, block_ids);
      populatePredsAndEdges(F, predecessors, edges);
      populateDominatorSets(dominators, predecessors, blocks);
      populateBackEdges(back_edges, edges, dominators);
      populateLoopInfomation(F.getName(), loops, back_edges, predecessors);

      for(auto &BB: F) {
        runOnBasicBlock(BB, predecessors);
        // The following code inserts the code to print the profiling data at the end of the main function.
        if(F.getName().equals("main") && isa<ReturnInst>(BB.getTerminator())) {
          printBasicBlockProfilingData(BB);
          printEdgeProfilingData(BB);          
          printLoopProfilingData(BB);
        }
      }

      printDominatorSet(dominators);
      printLoops(loops);

      return true;
    }

    /*
     * The following method stores the block names and their corresponding ids in separate lists for each function.
     * This information is later used to compute and print the profiling information.
     */
    void populateBlockInfo(Function &F, set<string> &blocks, map<string, int> &block_ids){
      
      int i = 0;
      for(auto &BB: F) {
        blocks.insert(BB.getName());
        block_ids[BB.getName()] = i++;
      }
      blockIdMap[F.getName()] = block_ids;
      basicBlockMap[F.getName()] = blocks;
    }

    /*
     * The following method stores the predecessors and edge information for each function.
     * This information is later used to compute dominator sets and populate loop information.
     */
    void populatePredsAndEdges(Function &F, map<string, set<string>> &predecessors, set<pair<string, string>> &edges){
       
      for(auto &BB: F) {
        TerminatorInst *TI = BB.getTerminator();
        for (unsigned s = 0, e = TI->getNumSuccessors(); s != e; ++s) {
          string BBname = TI->getSuccessor(s)->getName();
          auto it = predecessors.find(BBname);
          set<string> preds;
          if (it != predecessors.end()) {
            preds = predecessors[BBname];
          }
          preds.insert(BB.getName());
          predecessors[BBname] = preds;
          edges.insert(make_pair(BB.getName(), BBname));
        }
      }
      edgeMap[F.getName()] = edges;
    }

    /*
     * The following method computes the dominator sets from the predecessor information of all the basic blocks in the function.
     * It uses an iterative approach which terminates on convergence i.e. when there is no change in dominator sets.
     */
    void populateDominatorSets(map<string, set<string>> &dominators, map<string, set<string>> predecessors, set<string> blocks){
      
      for(set<string>::iterator it = blocks.begin(), end_it = blocks.end() ; it != end_it ; ++it) {
        if(it == blocks.begin()) {
            set<string> domset;
            domset.insert(*it);
            dominators[*it] = domset;
        } else {
            dominators[*it] = blocks;
        }
      }
      bool change;
      do {
        set<string>::iterator it = blocks.begin(), end_it = blocks.end();
        it++;
        change = false;
        for(; it != end_it ; ++it) {
          set<string> old = dominators[*it];
          set<string> temp = blocks;
          set<string> preds = predecessors[*it];
          for(auto pred: preds) {
              set<string> intersect;
              set_intersection(temp.begin(), temp.end(), dominators[pred].begin(), dominators[pred].end(),
                    std::inserter(intersect, intersect.begin()));
              temp = intersect;
          }
          temp.insert(*it);
          dominators[*it] = temp;
          if(old != dominators[*it])
            change = true;
        }
      } while(change);
    }

    /*
     * The following method computes the back edges from the dominator sets of all the basic blocks for a function.
     */
    void populateBackEdges(set<pair<string, string>> &back_edges, set<pair<string, string>> edges, map<string, set<string>> dominators){
      
      for(auto edge: edges) {
        auto doms = dominators[edge.first];
        if(doms.find(edge.second) != doms.end()){
          back_edges.insert(edge);
        }
      }
    }

    /*
     * The following method computes all the loops in the current function using the back edge and predecessor information.
     */
    void populateLoopInfomation(string functionName, set<set<string>> &loops, set<pair<string, string>> back_edges, map<string, set<string>> predecessors){
      
      for(auto edge: back_edges) {
        set<string> loop;
        loop.insert(edge.second);
        stack<string> bbStack;
        bbStack.push(edge.first);
        loop.insert(edge.first);
        while(!bbStack.empty()) {
          auto top = bbStack.top();
          bbStack.pop();
          for(auto pred: predecessors[top]) {
            if(loop.find(pred) == loop.end()){
              loop.insert(pred);
              bbStack.push(pred);
            }
          }
        }
        loops.insert(loop);
      }
      loopMap[functionName] = loops;
    }

    /*
     * The following method is used to insert code into a basic block. We insert three instructions in every basic block.
     * 1. Instruction to increment appropriate basic block execution counter at the start of the block using the current block index.
     * 2. Instruction to increment appropriate edge execution counter using the index of previously executed block (global variable).
     * 3. Instruction to update the previously executed block (global variable).
     */
    bool runOnBasicBlock(BasicBlock &BB, map<string, set<string>> &predecessors) {

      errs() << "\nBasicBlock: " << BB.getName() << '\n';
      errs() << "Predecessors: ";
      for(auto pred: predecessors[BB.getName()]) {
        errs() << pred << " ";
      }
      errs() << "\nInstructions:\n";
      for(auto &I: BB){
        errs() << I << "\n";
      }

      IRBuilder<> IRB(BB.getFirstInsertionPt());
      std::vector<Value*> edgeIndex;
      Value* zeroIndex = ConstantInt::get(Type::getInt32Ty(*Context), 0);
      edgeIndex.push_back(zeroIndex);

      Value* indexFirst = IRB.CreateLoad(previousBlockID);
      edgeIndex.push_back(indexFirst);

      Value* indexSecond = ConstantInt::get(Type::getInt32Ty(*Context), blockIdMap[BB.getParent()->getName()][BB.getName()]);
      edgeIndex.push_back(indexSecond);

      std::vector<Value*> bbIndex;
      bbIndex.push_back(zeroIndex);
      bbIndex.push_back(indexSecond);

      Value* bbCountVal = IRB.CreateGEP(bbCounters[BB.getParent()->getName()], bbIndex);
      Value* OldBBCountVal = IRB.CreateLoad(bbCountVal);
      Value *bbAddCounter = IRB.CreateAdd(OldBBCountVal, ConstantInt::get(Type::getInt32Ty(*Context), 1));
      IRB.CreateStore(bbAddCounter, bbCountVal);

      Value* edgeCountVal = IRB.CreateGEP(edgeCounters[BB.getParent()->getName()], edgeIndex);
      Value* OldEdgeCountVal = IRB.CreateLoad(edgeCountVal);
      Value *edgeAddCounter = IRB.CreateAdd(OldEdgeCountVal, ConstantInt::get(Type::getInt32Ty(*Context), 1));
      IRB.CreateStore(edgeAddCounter, edgeCountVal);

      IRBuilder<> builder(BB.getTerminator());
      Value *blockID = ConstantInt::get(Type::getInt32Ty(*Context), blockIdMap[BB.getParent()->getName()][BB.getName()]);
      builder.CreateStore(blockID, previousBlockID);

      return true;
    }

    /*
     * The following method print the basic block profiling data for each of the functions in the program.
     */
    void printBasicBlockProfilingData(BasicBlock& BB){
      
      printString(BB, "\nBASIC BLOCK PROFILING:\n");
      for(auto &function: bbCounters){
        printBasicBlockCount(BB, function.first, basicBlockMap[function.first], function.second);
      }
    }

    /*
     * The following method inserts a printf function at the end of the main function 
     * to display the basic block execution count.
     */
     void printBasicBlockCount(BasicBlock& BB, string funcName, set<string> blocks, GlobalVariable* counter){

      int i = 0;
      IRBuilder<> builder(BB.getTerminator());
      for(auto &block : blocks){
        ++i;
        Constant *format_const;
        GlobalVariable *BBPrintfFormatStr;

        if(i == 1) {
          printString(BB, "\n" + funcName + ":\n");
        }

        format_const = ConstantDataArray::getString(*Context, (block + ": %d\n").c_str());
        BBPrintfFormatStr = new GlobalVariable(*(BB.getParent()->getParent()), llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), strlen((block + ": %d\n").c_str())+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "BBPrintfFormatStr");

        std::vector<Constant*> indices;
        Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
        indices.push_back(zero);
        indices.push_back(zero);        
        Constant *var_ref = ConstantExpr::getGetElementPtr(BBPrintfFormatStr, indices);

        std::vector<Constant*> bbIndex;
        ConstantInt* zeroIndex = ConstantInt::get(Type::getInt32Ty(*Context), 0);
        bbIndex.push_back(zeroIndex);

        ConstantInt* indexFirst = ConstantInt::get(Type::getInt32Ty(*Context), blockIdMap[funcName][block]);
        bbIndex.push_back(indexFirst);

        Constant* bbCountVal = ConstantExpr::getGetElementPtr(counter, bbIndex);

        Value *bbCount = builder.CreateLoad(bbCountVal);
        CallInst *call = builder.CreateCall2(printf_func, var_ref, bbCount);
        call->setTailCall(false);
      }
    }

    /*
     * The following method print the edge profiling data for each of the functions in the program.
     */
    void printEdgeProfilingData(BasicBlock& BB){
      
      printString(BB, "\nEDGE PROFILING:\n");
      for(auto &function: edgeCounters){
        printEdgeCount(BB, function.first, edgeMap[function.first], function.second);
      }
    }

    /*
     * The following method inserts a printf function at the end of the main function 
     * to display the edge execution count.
     */
    void printEdgeCount(BasicBlock& BB, string funcName, set<pair<string, string>> edges, GlobalVariable* counter) {
      
      int i = 0;
      IRBuilder<> builder(BB.getTerminator());
      for(auto &edge: edges) {
        ++i;
        Constant *format_const;
        GlobalVariable *EdgePrintfFormatStr;
        if(i == 1) {
          printString(BB, "\n" + funcName + ":\n");
        }

        format_const = ConstantDataArray::getString(*Context, (edge.first + " -> " + edge.second + ": %d\n").c_str());
        EdgePrintfFormatStr = new GlobalVariable(*(BB.getParent()->getParent()), llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), strlen((edge.first + " -> " + edge.second + ": %d\n").c_str())+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "EdgePrintfFormatStr");

        std::vector<Constant*> indices;
        Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
        indices.push_back(zero);
        indices.push_back(zero);
        Constant *var_ref = ConstantExpr::getGetElementPtr(EdgePrintfFormatStr, indices);

        std::vector<Constant*> edgeIndex;
        ConstantInt* zeroIndex = ConstantInt::get(Type::getInt32Ty(*Context), 0);
        edgeIndex.push_back(zeroIndex);

        ConstantInt* indexFirst = ConstantInt::get(Type::getInt32Ty(*Context), blockIdMap[funcName][edge.first]);
        edgeIndex.push_back(indexFirst);

        ConstantInt* indexSecond = ConstantInt::get(Type::getInt32Ty(*Context), blockIdMap[funcName][edge.second]);
        edgeIndex.push_back(indexSecond);

        Constant* edgeCountVal = ConstantExpr::getGetElementPtr(counter, edgeIndex);

        Value *edgeCount = builder.CreateLoad(edgeCountVal);
        CallInst *call = builder.CreateCall2(printf_func, var_ref, edgeCount);
        call->setTailCall(false);
      }
    }

    /*
     * The following method print the loop profiling data for each of the functions in the program.
     */
    void printLoopProfilingData(BasicBlock& BB){
      
      printString(BB, "\nLOOP PROFILING:\n");
      for(auto &func: loopMap) {
        if(func.second.size() > 0) {
          printString(BB, "\n" + func.first + ":\n");
          for(auto loop: func.second) {
            int first = blockIdMap[func.first][*loop.begin()];
            int last;
            string loopStr = "";
            for(auto &b: loop) {
              loopStr += (b + " ");
              last = blockIdMap[func.first][b];
            }
            printString(BB, loopStr + ": ");
            printLoopCount(BB, first, last, edgeCounters[func.first]);
          }
        }
      }
    }

    /*
     * The following method inserts a printf function at the end of the main function 
     * to display the loop execution count.
     */
    void printLoopCount(BasicBlock& BB, int first, int last, GlobalVariable* edge) {

      IRBuilder<> builder(BB.getTerminator());
      Constant *format_const;
      GlobalVariable *PrintfFormatStr;
      string toPrint = "%d\n";
      format_const = ConstantDataArray::getString(*Context, toPrint.c_str());
      PrintfFormatStr = new GlobalVariable(*(BB.getParent()->getParent()), llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), strlen(toPrint.c_str())+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "PrintfFormatStr");
      
      std::vector<Constant*> indices;
      Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
      indices.push_back(zero);
      indices.push_back(zero);
      Constant *var_ref = ConstantExpr::getGetElementPtr(PrintfFormatStr, indices);

      std::vector<Constant*> edgeIndex;
      ConstantInt* zeroIndex = ConstantInt::get(Type::getInt32Ty(*Context), 0);
      edgeIndex.push_back(zeroIndex);

      ConstantInt* indexFirst = ConstantInt::get(Type::getInt32Ty(*Context), last);
      edgeIndex.push_back(indexFirst);

      ConstantInt* indexSecond = ConstantInt::get(Type::getInt32Ty(*Context), first);
      edgeIndex.push_back(indexSecond);

      Constant* edgeCountVal = ConstantExpr::getGetElementPtr(edge, edgeIndex);

      Value *loopCount = builder.CreateLoad(edgeCountVal);
      CallInst *call = builder.CreateCall2(printf_func, var_ref, loopCount);
      call->setTailCall(false);
    }

    /*
     * The following method is used to print the computed dominator set information of a particular function.
     */
    void printDominatorSet(map<string, set<string>> &dominators) {
      
      errs() << "\nDominatorSets:\n";
      for (std::map<string,set<string>>::iterator dominatorIterator=dominators.begin(); dominatorIterator!=dominators.end(); ++dominatorIterator) {
          errs() << "DomSet[" << dominatorIterator->first << "] => ";
          for(auto pred : dominatorIterator->second) {
            errs() << pred << " ";
          }
          errs() << "\n";
      }
    }    

    /*
     * The following method is used to print the loops inside a particular function.
     */
    void printLoops(set<set<string>> &loops) {

      if(loops.size() > 0) {
        errs() << "\nLoops:\n";
        for(auto loop: loops) {
          for(auto block: loop) {
            errs() << block << " ";
          }
          errs() << "\n";
        }
      }
    }

    /*
     * The following method inserts a printf function after the last instruction of the basic block
     */
    void printString(BasicBlock& BB, string toPrint) {
      
      IRBuilder<> builder(BB.getTerminator());
      Constant *format_const;
      GlobalVariable *PrintfFormatStr;
      format_const = ConstantDataArray::getString(*Context, toPrint.c_str());
      PrintfFormatStr = new GlobalVariable(*(BB.getParent()->getParent()), llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), strlen(toPrint.c_str())+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "PrintfFormatStr");
      
      std::vector<Constant*> indices;
      Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
      indices.push_back(zero);
      indices.push_back(zero);
      Constant *var_ref = ConstantExpr::getGetElementPtr(PrintfFormatStr, indices);

      Value *bbc = ConstantInt::get(*Context, APInt(32, StringRef("0"), 10));
      CallInst *call = builder.CreateCall2(printf_func, var_ref, bbc);
      call->setTailCall(false);
    }
  };
}

/*
 * We initialize pass ID here. LLVM uses ID’s address to identify a pass.
 */
char CS201Profiling::ID = 0;
/*
 * We register our class here.
 */
static RegisterPass<CS201Profiling> X("pathProfiling", "CS201Profiling Pass", false, false);