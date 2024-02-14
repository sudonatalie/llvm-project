//===-- SPIRVPrepareModuleVariables.cpp - modify function signatures --*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass creates SPIR-V module variables from HLSL Resource metadata.
// TODO: Maybe makes more sense to create the variables at the GLOBAL intrinsic
// lowering. Is this still the best way to get this metadata indexed by
// GlobalVariable?
//
//===----------------------------------------------------------------------===//

#include "SPIRV.h"
#include "SPIRVSubtarget.h"
#include "SPIRVTargetMachine.h"
#include "llvm/Frontend/HLSL/HLSLResource.h"

using namespace llvm;

namespace llvm {
void initializeSPIRVPrepareModuleVariablesPass(PassRegistry &);
}

namespace {

class SPIRVPrepareModuleVariables : public ModulePass {
  const SPIRVTargetMachine &TM;

public:
  static char ID;
  SPIRVPrepareModuleVariables(const SPIRVTargetMachine &TM)
      : ModulePass(ID), TM(TM) {
    initializeSPIRVPrepareModuleVariablesPass(*PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override;

  StringRef getPassName() const override {
    return "SPIRV prepare module variables";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    ModulePass::getAnalysisUsage(AU);
  }
};

} // namespace

char SPIRVPrepareModuleVariables::ID = 0;

INITIALIZE_PASS(SPIRVPrepareModuleVariables, "prepare-module-variables",
                "SPIRV prepare module variables", false, false)

bool SPIRVPrepareModuleVariables::runOnModule(Module &M) {
  const SPIRVSubtarget *ST = TM.getSubtargetImpl();
  SPIRVGlobalRegistry *GR = ST->getSPIRVGlobalRegistry();

  bool Changed = false;

  NamedMDNode *UavResources = M.getNamedMetadata("hlsl.uavs");
  for (MDNode *UavResource : UavResources->operands()) {
    hlsl::FrontendResource hlslResource = hlsl::FrontendResource(UavResource);
    GlobalVariable *GlobalVar = hlslResource.getGlobalVariable();

    GR->setHLSLResourceForGlobalVar(GlobalVar, UavResource);
  }

  return Changed;
}

ModulePass *
llvm::createSPIRVPrepareModuleVariablesPass(const SPIRVTargetMachine &TM) {
  return new SPIRVPrepareModuleVariables(TM);
}
