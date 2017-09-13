#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {
struct DynamicInstructionCountPass : public FunctionPass {
	static char ID;
	/**
	 * Name of the external function to be called during instrumentation
	 */
	static const std::string increment_function_name;

	DynamicInstructionCountPass()
	    : FunctionPass(ID) {}

	/**
	 * Inserts a call to a function after every basic block in
	 * order to count the number of LLVM IR instructions executed by the
	 * code.
	 *
	 * @param F function under inspection
	 * @return true if function is modified, false otherwise
	 */
	virtual bool runOnFunction(Function& F)
	{
		// Get the function to call from our runtime library.
		LLVMContext& Ctx = F.getContext();

		Constant* resultFunc
		    = F.getParent()->getOrInsertFunction(increment_function_name,
		        Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx), NULL);

		bool flag = false;

		for (auto& B : F) {

			int count = 0;
			count += B.size();

			IRBuilder<> builder(&B);
			builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

			Value* args[] = { builder.getInt32(count) };
			builder.CreateCall(resultFunc, args);
			flag = true;
		}

		return flag;
	}
};
}

// Ugly function name because of c++ function name mangling
// TODO: Find workaround
const std::string DynamicInstructionCountPass::increment_function_name
    = "_ZN6player17PlayerCodeWrapper14IncrementCountEi";

char DynamicInstructionCountPass::ID = 0;

// Automatically enable the pass.
static void registerDynamicInstructionCountPass(
    const PassManagerBuilder&, legacy::PassManagerBase& PM)
{
	PM.add(new DynamicInstructionCountPass());
}

static RegisterStandardPasses RegisterMyPass(
    PassManagerBuilder::EP_EarlyAsPossible,
    registerDynamicInstructionCountPass);
