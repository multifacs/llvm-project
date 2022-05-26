#include "llvm/Pass.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
// #include "llvm/IR/BasicBlock.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define DEBUG_TYPE "obfuscate"

using namespace llvm;

namespace {

struct ObfuscateI8Add : public FunctionPass {
    static char ID;
    ObfuscateI8Add() : FunctionPass(ID) {}

    bool runOnFunction(Function& function) override {
        errs() << "********** PEEPHOLE OPTIMIZATION COURSE **********\n";
        errs() << "********** Obfuscate I8 Add **********\n";
        errs() << "********** Function: " << function.getName() << '\n';

        bool changed = false;
        for (auto basicBlock = function.begin(), basicBlockEnd = function.end(); basicBlock != basicBlockEnd; ++basicBlock) {
            for (auto instruction = basicBlock->begin(), ie = basicBlock->end(); instruction != ie; ++instruction) {
                auto binaryOperator = dyn_cast<BinaryOperator>(instruction);
                if (!binaryOperator) {
                    continue;
                }

                if (binaryOperator->getOpcode() != Instruction::Add) {
                    continue;
                }

                if (!binaryOperator->getType()->isIntegerTy() || binaryOperator->getType()->getIntegerBitWidth() != 8) {
                    continue;
                }

                auto Const39 = ConstantInt::get(binaryOperator->getType(), 39);
                auto Const151 = ConstantInt::get(binaryOperator->getType(), 151);
                auto Const23 = ConstantInt::get(binaryOperator->getType(), 23);
                auto Const2 = ConstantInt::get(binaryOperator->getType(), 2);
                auto Const111 = ConstantInt::get(binaryOperator->getType(), 111);

                IRBuilder<> builder(binaryOperator);
                auto obfuscated = BinaryOperator::CreateAdd(
                    Const111,
                    builder.CreateMul(
                        Const151,
                        builder.CreateAdd(
                            Const23,
                            builder.CreateMul(
                                Const39,
                                builder.CreateAdd(
                                    builder.CreateXor(binaryOperator->getOperand(0), binaryOperator->getOperand(1)),
                                    builder.CreateMul(
                                        Const2, builder.CreateAnd(binaryOperator->getOperand(0), binaryOperator->getOperand(1))
                                    )
                                )
                            )
                        )
                    )
                );

                ReplaceInstWithInst(basicBlock->getInstList(), instruction, obfuscated);

                LLVM_DEBUG(dbgs() << *binaryOperator << " -> " << *obfuscated << '\n');
                changed = true;
            }
        }

        return changed;
    }
};

}  // namespace

char ObfuscateI8Add::ID = 0;
static RegisterPass<ObfuscateI8Add> X(
    "obfuscate-i8-add",                                    /* Command line argument */
    "Peephole Optimization Course Pass: Obfuscate I8 Add", /* Help string */
    false                                                  /* Changes the CFG */,
    false                                                  /* This is not the Analysis Pass */
);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new ObfuscateI8Add());
    }
);
