#pragma once

#ifdef _DEBUG  
#pragma comment(lib,"opencv_world320d.lib") 
#else
#pragma comment(lib,"opencv_world320.lib") 
#endif

#include "Engine.h"

#include "TestMyBPF.generated.h"

UCLASS()
class TESTMYBP_API UTestMyBPF : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static bool ASimpleNodeFunction(UObject* AnyObject);
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UTexture2D* GenerateTextureFunction(TArray<uint8> ByteArray);
};