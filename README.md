# CustomComputeExample  
This is an example of how to write custom compute-shaders in Unreal Engine 4.27 (there is also a 4.26 branch)
The main goal of this project is to provide an example of how to use the RDG to transfer large amounts of data to the GPU.  
The [Wiki](https://github.com/nfgrep/CustomComputeExample/wiki) of this project contains an explaination of the code.  

The implementation and usage of the shader lies entirely in the plugin of this project.
To demonstrate the use of compute-shaders, the project just colours a texture with the position of the first vertex of a given mesh.  

 And I have Modified Engine Code For AllowRHIAccess Function

 Go to [\Engine\Source\Runtime\RenderCore\Public\RenderGraphResources.h}(https://github.com/EpicGames/UnrealEngine/blob/d94b38ae3446da52224bedd2568c078f828b4039/Engine/Source/Runtime/RenderCore/Public/RenderGraphResources.h#L54)

## Find The FRDGResource Class Make Following Changes.
(((
 /** Generic graph resource. */
class RENDERCORE_API FRDGResource
{
public:
	FRDGResource(const FRDGResource&) = delete;
	virtual ~FRDGResource() = default;

	// Name of the resource for debugging purpose.
	const TCHAR* const Name = nullptr;

	//////////////////////////////////////////////////////////////////////////
	//! The following methods may only be called during pass execution.

	/** Marks this resource as actually used by a resource. This is to track what dependencies on pass was actually unnecessary. */
#if RDG_ENABLE_DEBUG
	virtual void MarkResourceAsUsed();
#else
	inline  void MarkResourceAsUsed() {}
#endif

	FRHIResource* GetRHI() const
	{
		
		ValidateRHIAccess();
		return ResourceRHI;
	}
	void AllowRHIAccess(bool bAllow)
	{
		DebugData.bAllowRHIAccess = bAllow;
	}
	//////////////////////////////////////////////////////////////////////////

protected:
	FRDGResource(const TCHAR* InName)
		: Name(InName)
	{}

	/** Assigns this resource as a simple passthrough container for an RHI resource. */
	void SetPassthroughRHI(FRHIResource* InResourceRHI)
	{
		ResourceRHI = InResourceRHI;
#if RDG_ENABLE_DEBUG
		DebugData.bAllowRHIAccess = true;
		DebugData.bPassthrough = true;
#endif
	}

	bool IsPassthrough() const
	{
#if RDG_ENABLE_DEBUG
		return DebugData.bPassthrough;
#else
		return false;
#endif
	}

	/** Verify that the RHI resource can be accessed at a pass execution. */
	void ValidateRHIAccess() const
	{
		
#if RDG_ENABLE_DEBUG
		checkf(DebugData.bAllowRHIAccess,
			TEXT("Accessing the RHI resource of %s at this time is not allowed. If you hit this check in pass, ")
			TEXT("that is due to this resource not being referenced in the parameters of your pass."),
			Name);
#endif
	}

	FRHIResource* GetRHIUnchecked() const
	{
		return ResourceRHI;
	}

	FRHIResource* ResourceRHI = nullptr;

private:
#if RDG_ENABLE_DEBUG
	class FDebugData
	{
	private:
		/** Boolean to track at runtime whether a resource is actually used by the lambda of a pass or not, to detect unnecessary resource dependencies on passes. */
		bool bIsActuallyUsedByPass = false;

		/** Boolean to track at pass execution whether the underlying RHI resource is allowed to be accessed. */
		bool bAllowRHIAccess = false;

		/** If true, the resource is not attached to any builder and exists as a dummy container for staging code to RDG. */
		bool bPassthrough = false;

		friend FRDGResource;
		friend FRDGUserValidation;
	} DebugData;
#endif

	friend FRDGBuilder;
	friend FRDGUserValidation;
};

)))
