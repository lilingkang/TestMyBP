#include "K2Node_ASimpleNode.h"

#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

FText UK2Node_ASimpleNode::GetTooltipText() const
{
	return FText::FromString(TEXT("This is a simple node..."));
}

FText UK2Node_ASimpleNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Simple Custom Node"));
}

FText UK2Node_ASimpleNode::GetMenuCategory() const
{
	return FText::FromString(TEXT("Custom Node"));
}

void UK2Node_ASimpleNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

void UK2Node_ASimpleNode::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object,
        "AnyObject")->PinType.PinSubCategoryObject = UObject::StaticClass();
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, "ReturnValue");
}

void UK2Node_ASimpleNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    UK2Node_CallFunction* SelfFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
    SelfFunction->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UTestMyBPF,
        ASimpleNodeFunction), UTestMyBPF::StaticClass());
    SelfFunction->AllocateDefaultPins();

    // CompilerContext.MessageLog.NotifyIntermediateObjectCreation(SelfFunction, this);
    
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *SelfFunction->GetExecPin());
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, UEdGraphSchema_K2::PN_Then),
        *GetNodePin(SelfFunction, UEdGraphSchema_K2::PN_Then));
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "AnyObject"),
        *GetNodePin(SelfFunction, "AnyObject"));
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "ReturnValue"),
        *GetNodePin(SelfFunction, "ReturnValue"));
}

// 按名称寻找节点引脚
UEdGraphPin* UK2Node_ASimpleNode::GetNodePin(UK2Node* node, FName name)
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
