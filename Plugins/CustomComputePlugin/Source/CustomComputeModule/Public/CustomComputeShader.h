// Author: nfgrep
// 2021

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"
#include "RHIResources.h"
#include "RHI.h"
#include "RenderGraphUtils.h"
class UTextureRenderTarget2D;
// This is the class that represents our shader within the engine
// This class is associated with shader-code in the corresponding .cpp
class FCustomComputeShader : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCustomComputeShader)
	SHADER_USE_PARAMETER_STRUCT(FCustomComputeShader, FGlobalShader)
		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, ) 

		SHADER_PARAMETER_RDG_BUFFER_SRV(RWStructuredBuffer<FVector>, Vertices)
		
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<FVector4>, OutputTexture)

	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
	
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	void BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, UTextureRenderTarget2D* RenderTarget, TArray<FVector> InVerts);
	static void CopyBuffer(FRHICommandListImmediate& RHICmdList, TRefCountPtr<FRDGPooledBuffer>& source, void* dest, SIZE_T size);
	void ReadbackRTT(FRHICommandListImmediate& RHI_cmd_list, UTextureRenderTarget2D* rtt);
	void ReadbackVertx(FRHICommandListImmediate& RHICmdList, FRDGBuilder& GraphBuilder, FRDGBufferRef OutputBuffer);
	LAYOUT_FIELD(FStructuredBufferRHIRef, test_retrival)
	LAYOUT_FIELD(TArray<FVector>, ValueToPrint_FROM_Retrival)
	 //TArray<uint32> ValueToPrint_FROM_Retrival = { 0 };
};

struct FComputeReadBackStruct
{
	
	TUniquePtr<FRHIGPUBufferReadback> ReadBack_GPU;
	TArray<uint8>                     Data;
};


struct TestStruct {
	FVector TestPosition;
};