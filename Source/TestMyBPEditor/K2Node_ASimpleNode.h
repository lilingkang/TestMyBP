#pragma once

#include "Engine.h"
#include "K2Node.h"
#include "TestMyBP/TestMyBPF.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#include "K2Node_ASimpleNode.generated.h"

UCLASS()
class TESTMYBPEDITOR_API UK2Node_ASimpleNode : public UK2Node
{
	GENERATED_BODY()

public:
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

private:
	static UEdGraphPin* GetNodePin(UK2Node* node, FName name);
};
