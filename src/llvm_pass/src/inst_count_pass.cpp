#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

namespace {
struct DynamicInstructionCountPass : public llvm::FunctionPass {
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
	virtual bool runOnFunction(llvm::Function& F)
	{
		// Get the function to call from our runtime library.
		llvm::LLVMContext& Ctx = F.getContext();

		llvm::Constant* resultFunc
		    = F.getParent()->getOrInsertFunction(increment_function_name,
		        llvm::Type::getVoidTy(Ctx), llvm::Type::getInt32Ty(Ctx), NULL);

		bool flag = false;

		for (auto& B : F) {

			int count = 0;
			count += B.size();

			llvm::IRBuilder<> builder(&B);
			builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

			llvm::Value* args[] = { builder.getInt32(count) };
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
    = "_ZN7drivers12PlayerDriver14IncrementCountEm";

char DynamicInstructionCountPass::ID = 0;

// Automatically enable the pass.
static void registerDynamicInstructionCountPass(
    const llvm::PassManagerBuilder&, llvm::legacy::PassManagerBase& PM)
{
	PM.add(new DynamicInstructionCountPass());
}

static llvm::RegisterStandardPasses RegisterMyPass(
    llvm::PassManagerBuilder::EP_EarlyAsPossible,
    registerDynamicInstructionCountPass);
