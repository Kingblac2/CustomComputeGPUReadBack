// Author: nfgrep
// 2021

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include<CustomComputeModule/Public/CustomComputeShader.h>
// We'll use this class as the API for our CustomComputeModule
// This will be the only class exposed to other modules via the CUSTOMCOMPUTEMODULE_API
class CUSTOMCOMPUTEMODULE_API FCustomComputeModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void EnqueueRenderCommand(UTextureRenderTarget2D* RenderTarget, TArray<FVector> Vertices);

	TArray<FVector> GetRBvalue();

private:

};
