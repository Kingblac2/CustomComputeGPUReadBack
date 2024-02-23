// Az		zuthor: nfgrep
// 2021

#include "CustomComputeShader.h"
#include "RenderGraph.h"
#include "RenderTargetPool.h"
#include "Engine/TextureRenderTarget2D.h"

// Associating the FCustomComputeShader class with the custom HLSL shader code
IMPLEMENT_GLOBAL_SHADER(FCustomComputeShader, "/Shaders/Private/ComputeShader.usf", "MainCompute", SF_Compute);


BEGIN_SHADER_PARAMETER_STRUCT(FCopyBufferParameters, )
RDG_BUFFER_ACCESS(Buffer, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

// This method builds and calls .Execute() on a render-graph for a single frame
void FCustomComputeShader::BuildAndExecuteGraph(FRHICommandListImmediate& RHICmdList, UTextureRenderTarget2D* RenderTarget, TArray<FVector> InVerts)
{
	// Our main point of contant with the RDG
	// We will use this to add resources (buffers) and passes to the render-graph
	FRDGBuilder GraphBuilder(RHICmdList);


	// This is a pointer to the shader-parameters we declared in the .h
	FParameters* PassParameters;
	FParameters parameters;
	// We ask the RDG to allocate some memory for our shader-parameters
	PassParameters = GraphBuilder.AllocParameters<FCustomComputeShader::FParameters>();

	FVector *dest= (FVector*)GraphBuilder.Alloc(sizeof(FVector) * InVerts.Num(), 16);
	
	FCopyBufferParameters* CopyParameters = GraphBuilder.AllocParameters<FCopyBufferParameters>();


	// --- Creating an SRV filled with vertex data ---
	// 1. Create a structured buffer
	FRDGBufferRef VerticesBuffer = CreateStructuredBuffer(
		GraphBuilder,
		TEXT("Vertices_StructuredBuffer"),
		sizeof(FVector),
		InVerts.Num(),
		InVerts.GetData(),
		sizeof(FVector) * InVerts.Num()
	);

	FRWBufferStructured Cargo;

	// 2. Create an RDG-tracked SRV from our structured buffer
	FRDGBufferSRVRef VerticesSRV = GraphBuilder.CreateSRV(VerticesBuffer, PF_R32_UINT);
	// 3. Set our pass params
	PassParameters->Vertices = VerticesSRV;

	
	// --- Creating a texture for the compute shader to write to ---
	// 1. Make a texture description 
	FRDGTextureDesc OutTextureDesc = FRDGTextureDesc::Create2D(
		FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY),
		PF_FloatRGBA,
		FClearValueBinding(),
		TexCreate_UAV,
		1,
		1);
	// 2. Allocate memory with above desc and get a ref to it
	FRDGTextureRef OutTextureRef = GraphBuilder.CreateTexture(OutTextureDesc, TEXT("Compute_Out_Texture"));
	// 3. Make a UAV description from our Texture Ref
	FRDGTextureUAVDesc OutTextureUAVDesc(OutTextureRef);
	// 4. Initialize it as our OutputTexture in our pass params
	PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutTextureUAVDesc);
	

	// ------ Add the compute pass ------
	// Get a reference to our global shader class
	TShaderMapRef<FCustomComputeShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	// Add the compute shader pass to the render graph
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("Compute Pass"), ComputeShader, PassParameters, FIntVector(32, 32, 1));

	CopyParameters->Buffer = VerticesBuffer;
	uint32 size = InVerts.Num() * sizeof(FVector);
 
 	   /*Write this pass in AddPass(ExcuteLamda&&)-*/
//	GraphBuilder.AddPass(
//		RDG_EVENT_NAME("StructuredBufferUpload(%s)", VerticesBuffer->Name),
//			CopyParameters,
//			ERDGPassFlags::Copy,
//			[VerticesBuffer,dest,size](FRHICommandListImmediate& RHICmdList)
//			{	FRHIStructuredBuffer* StructuredBuffer = VerticesBuffer->GetRHIStructuredBuffer();
//				void* psource = RHICmdList.LockStructuredBuffer(StructuredBuffer, 0, size, RLM_WriteOnly);
//				FMemory::Memcpy(dest, psource, size);
//				RHICmdList.UnlockStructuredBuffer(StructuredBuffer);
//			});


//	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("CopyPasss"), ERDGPassFlags::Copy,ComputeShader, PassParameters, FIntVector(1,1,1));
	// ------ Extracting to pooled render target ------
	TRefCountPtr<IPooledRenderTarget> PooledComputeTarget;
	// Copy the result of compute shader from UAV to pooled renderT-target
	GraphBuilder.QueueTextureExtraction(OutTextureRef, &PooledComputeTarget);
	TRefCountPtr<FRDGPooledBuffer> pooled_vertices;
	GraphBuilder.QueueBufferExtraction(VerticesBuffer, &pooled_vertices, ERHIAccess::CPURead);
										
	
												/*Real Redback Processiing*/
			
			
	
	AddPass(GraphBuilder, [VerticesBuffer,dest,size](FRHICommandListImmediate & RHICmdList)
		{
			VerticesBuffer->AllowRHIAccess(true);
				FStructuredBufferRHIRef StructuredBuffer = VerticesBuffer->GetRHIStructuredBuffer();
				void* psource = (FVector*)RHICmdList.LockStructuredBuffer(StructuredBuffer.GetReference(), 0, size, RLM_ReadOnly);
				FMemory::Memcpy(dest, psource, size);
				RHICmdList.UnlockStructuredBuffer(StructuredBuffer);

		});					
	// Execute the graph
GraphBuilder.Execute();
	

	if (dest != nullptr)
	{
	
			UE_LOG(LogTemp, Warning, TEXT("%f , %f ,%f"), dest->X, dest->Y, dest->Z)
	
	}

	//if (dest == nullptr)
	//{
	//
	//	UE_LOG(LogTemp, Warning, TEXT("Radhe Shyam"));
	//}
	//else {
// R//eadbackVertx(RHICmdList, GraphBuilder, VerticesBuffer); }
	//	UE_LOG(LogTemp, Warning, TEXT("Viswas Rakhana"));
	//}
	//VerticesBuffer->GetRHIVertexBuffer();
	/* Use This under my comment*/ /*Future Me*/

	//FRDGBufferSRVRef BufferSRV = Cast<FRDGBufferSRV>(VerticesSRV->GetParent());
	//FCustomComputeShader::CopyBuffer(RHICmdList, pooled_vertices,ValueToPrint_FROM_Retrival.GetData(), sizeof(FVector) * ValueToPrint_FROM_Retrival.Num());
	
//	void* pSource = RHICmdList.LockStructuredBuffer(pooled_vertices->GetStructuredBufferRHI(), 0, sizeof(FVector) * ValueToPrint_FROM_Retrival.Num(),RLM_ReadOnly);
	//const uint32* retrive_data = (const uint32*)RHICmdList.LockStructuredBuffer(test_retrival, 0, sizeof(uint32), EResourceLockMode::RLM_ReadOnly);
//	FMemory::Memcpy(ValueToPrint_FROM_Retrival.GetData(), pSource, sizeof(FVector) * ValueToPrint_FROM_Retrival.Num());
//
//	//FMemory::Memcpy(ValueToPrint_FROM_Retrival.GetData(), retrive_data, sizeof(float) * 1);
//	// Queue the UAV we wrote to for extraction 
//	RHICmdList.UnlockStructuredBuffer(pooled_vertices->GetStructuredBufferRHI());

	RHICmdList.CopyToResolveTarget(PooledComputeTarget.GetReference()->GetRenderTargetItem().TargetableTexture, RenderTarget->GetRenderTargetResource()->TextureRHI, FResolveParams());
	//dest = RHICmdList.AcquireTransientResource_RenderThread(VerticesBuffer->GetRHIStructuredBuffer());
	
	//UE_LOG(LogTemp, Warning, TEXT("FFT OUTPUT START"));
//	ReadbackRTT(RHICmdList, RenderTarget);
	//ReadbackVertx(RHICmdList, VerticesBuffer->GetRHIVertexBuffer(), InVerts.Num());
//	UE_LOG(LogTemp, Warning, TEXT("FFT OUTPUT END"));

//    FRDGBufferRef verticies_buff = GraphBuilder.RegisterExternalBuffer(pooled_vertices, TEXT("Vertices_StructuredBuffer"));

	struct TestStruct TestElement;
	TestElement.TestPosition =  FVector(InVerts.GetData()->X,InVerts.GetData()->Y,InVerts.GetData()->Z);
	TResourceArray<struct TestStruct> bufferData;
	bufferData.Reset();
	bufferData.Add(TestElement);
	bufferData.SetAllowCPUAccess(true);

	FRHIResourceCreateInfo TestCreateInfo;
	TestCreateInfo.ResourceArray = &bufferData;

	FStructuredBufferRHIRef TestStructuredBuffer = RHICreateStructuredBuffer(sizeof(TestStruct), sizeof(TestStruct) * 1, BUF_UnorderedAccess | BUF_ShaderResource, TestCreateInfo);
	TArray<FVector> Data;
	Data.Reset();
	FVector TestEle(0.0f, 1.0f, 1.0f);
	Data.Add(TestEle);
	

	FVector* srcptr = (FVector*)RHILockStructuredBuffer(TestStructuredBuffer.GetReference(), 0, sizeof(FVector), EResourceLockMode::RLM_ReadOnly);
	FMemory::Memcpy(Data.GetData(), srcptr, sizeof(FVector));
	RHIUnlockStructuredBuffer(TestStructuredBuffer.GetReference());
			
	for (auto i = Data.begin() ; i != Data.end();++i)
	{
		
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%f, %f, %f"), i.operator*().X,i.operator*().Y,i.operator*().Z));
	}
}

void FCustomComputeShader::CopyBuffer(FRHICommandListImmediate& RHICmdList, TRefCountPtr<FRDGPooledBuffer>& source, void* dest, SIZE_T size)
{
	
			void* psource = RHILockStructuredBuffer(source->GetStructuredBufferRHI(), 0, size, RLM_ReadOnly);
			FMemory::Memcpy(dest, psource, size);
			RHIUnlockStructuredBuffer(source->GetStructuredBufferRHI());
		
}

 void FCustomComputeShader::ReadbackRTT(FRHICommandListImmediate& RHI_cmd_list, UTextureRenderTarget2D* rtt) {
	FRHIResourceCreateInfo CreateInfo;
	FTexture2DRHIRef readback_tex = RHICreateTexture2D(
		rtt->SizeX,
		rtt->SizeY,
		PF_FloatRGBA,
		1,
		1,
		TexCreate_RenderTargetable,
		CreateInfo);

	RHI_cmd_list.CopyToResolveTarget(
		rtt->GetRenderTargetResource()->TextureRHI,
		readback_tex->GetTexture2D(),
		FResolveParams()
	);

	FReadSurfaceDataFlags read_flags(RCM_MinMax);
	read_flags.SetLinearToGamma(false);

	TArray<FFloat16Color> data;
	RHI_cmd_list.ReadSurfaceFloatData(
		readback_tex->GetTexture2D(),
		FIntRect(0, 0, rtt->SizeX, rtt->SizeY),
		data,
		read_flags
	);

	for (int i = 0; i < data.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("%i: (%f, %f, %f, %f)"), i, data[i].R.GetFloat(), data[i].G.GetFloat(), data[i].B.GetFloat(), data[i].A.GetFloat());
	}
}

 void FCustomComputeShader::ReadbackVertx(FRHICommandListImmediate& RHICmdList,FRDGBuilder& GraphBuilder, FRDGBufferRef OutputBuffer)
 {
	 FComputeReadBackStruct ReadBackDataStruct;
	 ReadBackDataStruct.ReadBack_GPU.Reset(new FRHIGPUBufferReadback(TEXT("HelloFellas")));
	 FComputeReadBackStruct* DataPtr = &ReadBackDataStruct;
	if (DataPtr && DataPtr->ReadBack_GPU.IsValid())
			 {
				 ensure(DataPtr->ReadBack_GPU->IsReady());
			
					const void* BufferPtr = DataPtr->ReadBack_GPU->Lock(1);
				 FMemory::Memcpy(DataPtr->Data.GetData(), BufferPtr, DataPtr->Data.Num());
				 DataPtr->ReadBack_GPU->Unlock();
				 if (DataPtr->Data.GetData() != nullptr)
				 {
					 
					 AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [DataPtr]() {
						 for (int i = 0; i < DataPtr->Data.Num(); i++) {
							 UE_LOG(LogTemp, Warning, TEXT("%i: %s"), i,DataPtr->Data.GetData());
						 }
						 
						 });
				 }
				 else {
									UE_LOG(LogTemp, Warning, TEXT("Problem In array"));

				 }
     }
	else
	{
		AsyncTask(ENamedThreads::GameThread, []() {
			UE_LOG(LogTemp, Warning, TEXT("Else Not Valid"));

			});

	}
			 
		
	
									
   }
