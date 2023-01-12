#include "K2Node_GenerateTextureFromByteArray.h"

#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

FText UK2Node_GenerateTextureFromByteArray::GetTooltipText() const
{
	return FText::FromString(TEXT("该节点可以从字节数组生成2D纹理"));
}

FText UK2Node_GenerateTextureFromByteArray::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Generate Texture From Byte Array"));
}

FText UK2Node_GenerateTextureFromByteArray::GetMenuCategory() const
{
	return FText::FromString(TEXT("Custom Node"));
}

void UK2Node_GenerateTextureFromByteArray::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_GenerateTextureFromByteArray::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec,UEdGraphSchema_K2::PN_Then);

	UEdGraphNode::FCreatePinParams PinParams;
	PinParams.ContainerType = EPinContainerType::Array;
	PinParams.bIsReference = false;
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, "ByteArray", PinParams);
	// CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, "SrcHeight");
	// CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, "SrcWidth");
	// CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, "TextureHeight");
	// CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Int, "TextureWidth");
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object,
		"Texture2D")->PinType.PinSubCategoryObject = UTexture2D::StaticClass();
}

void UK2Node_GenerateTextureFromByteArray::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	UK2Node_CallFunction* SelfFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	SelfFunction->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UTestMyBPF,
		GenerateTextureFunction), UTestMyBPF::StaticClass());
	SelfFunction->AllocateDefaultPins();
	
	CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *SelfFunction->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, UEdGraphSchema_K2::PN_Then),
		*GetNodePin(SelfFunction, UEdGraphSchema_K2::PN_Then));
	CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "ByteArray"),
		*GetNodePin(SelfFunction, "ByteArray"));
	// CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "SrcHeight"),
	// 	*GetNodePin(SelfFunction, "SrcHeight"));
	// CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "SrcWidth"),
	// 	*GetNodePin(SelfFunction, "SrcWidth"));
	// CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "TextureHeight"),
	// 	*GetNodePin(SelfFunction, "TextureHeight"));
	// CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "TextureWidth"),
	// *GetNodePin(SelfFunction, "TextureWidth"));
	CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "Texture2D"),
		*GetNodePin(SelfFunction, "ReturnValue"));
}

// 按名称寻找节点引脚
UEdGraphPin* UK2Node_GenerateTextureFromByteArray::GetNodePin(UK2Node* node, FName name)
{
	for(int32 a=0;a<node->Pins.Num();a++)
	{
		if(node->Pins[a]->PinName == name)
		{
			return node->Pins[a];
		}
	}
	return nullptr;
}
