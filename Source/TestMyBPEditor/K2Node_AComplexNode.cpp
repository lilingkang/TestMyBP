#include "K2Node_AComplexNode.h"

#include "EdGraphSchema_K2.h"
#include "K2Node_CallFunction.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"

FText UK2Node_AComplexNode::GetTooltipText() const
{
	return FText::FromString(TEXT("This is a complex node..."));
}

FText UK2Node_AComplexNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Complex Custom Node"));
}

FText UK2Node_AComplexNode::GetMenuCategory() const
{
	return FText::FromString(TEXT("Custom Node"));
}

void UK2Node_AComplexNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
    UClass* ActionKey = GetClass();

    if (ActionRegistrar.IsOpenForRegistration(ActionKey))
    {
        UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
        check(NodeSpawner != nullptr);

        ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
    }
}

void UK2Node_AComplexNode::AllocateDefaultPins()
{
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, "Condition");
    CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Boolean, "OtherFirst");
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, "Do");
    CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, "Other");
}

void UK2Node_AComplexNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
    // 判断是否应该先执行Other
    UK2Node_IfThenElse* OtherBranch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
    OtherBranch->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *OtherBranch->GetExecPin());
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "OtherFirst"),
        *GetNodePin(OtherBranch, "Condition"));

    // 先执行Other序列
    UK2Node_ExecutionSequence* TrueSeq = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this,
        SourceGraph);
    TrueSeq->AllocateDefaultPins();
    OtherBranch->GetThenPin()->MakeLinkTo(TrueSeq->GetExecPin());
    // 后执行Other序列
    UK2Node_ExecutionSequence* FalseSeq = CompilerContext.SpawnIntermediateNode<UK2Node_ExecutionSequence>(this,
        SourceGraph);
    FalseSeq->AllocateDefaultPins();
    OtherBranch->GetElsePin()->MakeLinkTo(FalseSeq->GetExecPin());

    // 连接到输出引脚Other
    TArray<UEdGraphPin*> OtherPins = GetNodePin(this, "Other")->LinkedTo;
    GetNodePin(this, "Other")->BreakAllPinLinks(false);
    for(int32 a=0; a < OtherPins.Num(); a++)
    {
        GetNodePin(TrueSeq, "then_0")->MakeLinkTo(OtherPins[a]);
        GetNodePin(FalseSeq, "then_1")->MakeLinkTo(OtherPins[a]);
    }
    // CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "Other"),
    //     *GetNodePin(TrueSeq, "then_0"));
    // CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "Other"),
    //     *GetNodePin(FalseSeq, "then_1"));

    // 连接到输出引脚Do
    UK2Node_IfThenElse* DoBranch = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
    DoBranch->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "Condition"),
        *GetNodePin(DoBranch, "Condition"));
    GetNodePin(TrueSeq, "then_1")->MakeLinkTo(DoBranch->GetExecPin());
    GetNodePin(FalseSeq, "then_0")->MakeLinkTo(DoBranch->GetExecPin());
    CompilerContext.MovePinLinksToIntermediate(*GetNodePin(this, "Do"),
        *DoBranch->GetThenPin());
}

// 按名称寻找节点引脚
UEdGraphPin* UK2Node_AComplexNode::GetNodePin(UK2Node* node, FName name)
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
