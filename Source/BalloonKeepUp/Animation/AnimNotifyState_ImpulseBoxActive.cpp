// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_ImpulseBoxActive.h"

#include "BalloonKeepUpCharacter.h"

void UAnimNotifyState_ImpulseBoxActive::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                    float TotalDuration)
{
	if (!MeshComp || !Animation) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ABalloonKeepUpCharacter* MyCharacter = Cast<ABalloonKeepUpCharacter>(Owner);
	if (!MyCharacter) return;

	MyCharacter->EnableImpulseBox(BoxType);
}

void UAnimNotifyState_ImpulseBoxActive::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !Animation) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ABalloonKeepUpCharacter* MyCharacter = Cast<ABalloonKeepUpCharacter>(Owner);
	if (!MyCharacter) return;

	MyCharacter->DeactivateImpulseBox(BoxType);
}
