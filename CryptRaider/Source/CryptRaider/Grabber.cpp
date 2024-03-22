// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicalHandle();
	if (PhysicsHandle != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("We found a PH - %s"), *PhysicsHandle->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find a PH!!"));
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicalHandle();
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		FVector TargetLocation = GetComponentLocation() + GetForwardVector() * HoldDistance;
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
	}

	/*float damage;
	if (HasDamage(damage))
	{
		PrintDamage(damage);
	}*/

	/*
	float Damage = 0;
	float& DamageRef = Damage;
	float* DamagePtr = &Damage;

	UE_LOG(LogTemp, Display, TEXT("1. Damage: %f -- DamageRef: %f -- DamagePtr: %f"), Damage, DamageRef, *DamagePtr);

	DamageRef = 5;

	UE_LOG(LogTemp, Display, TEXT("2. Damage: %f -- DamageRef: %f -- DamagePtr: %f"), Damage, DamageRef, *DamagePtr);

	float newDamage = 6;
	DamagePtr = &newDamage;

	UE_LOG(LogTemp, Display, TEXT("3. Damage: %f -- DamageRef: %f -- DamagePtr: %f"), Damage, DamageRef, *DamagePtr);

	DamageRef = 10;

	UE_LOG(LogTemp, Display, TEXT("4. Damage: %f -- DamageRef: %f -- DamagePtr: %f"), Damage, DamageRef, *DamagePtr);

	FRotator rotation = GetComponentRotation();
	FString rotationString = rotation.ToCompactString();
	UE_LOG(LogTemp, Display, TEXT("The grabber's location: %s"), *rotationString);
	float time = world->TimeSeconds;
	UE_LOG(LogTemp, Display, TEXT("Time : %f"), time);
	*/

}

void UGrabber::PrintDamage(const float& damage)
{
	UE_LOG(LogTemp, Display, TEXT("1. Damage: %f"), damage);
}

bool UGrabber::HasDamage(float& outDamage)
{
	outDamage = 5;
	return true;
}

void UGrabber::Release()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicalHandle();

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent();
		GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
		GrabbedComponent->WakeAllRigidBodies();
		PhysicsHandle->ReleaseComponent();
	}
}

void UGrabber::Grab()
{
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicalHandle();
	if (PhysicsHandle == nullptr)
		return;

	FHitResult hitResult;
	bool hasHit = GetGrabbableInReach(hitResult);

	if (hasHit)
	{
		UPrimitiveComponent* HitComponent = hitResult.GetComponent();
		HitComponent->WakeAllRigidBodies();
		HitComponent->SetSimulatePhysics(true);
		hitResult.GetActor()->Tags.Add("Grabbed");
		hitResult.GetActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			HitComponent,
			NAME_None,
			hitResult.ImpactPoint,
			GetComponentRotation()
		);

		/*FString actorName = hitResult.GetActor()->GetActorNameOrLabel();
		DrawDebugSphere(world, hitResult.Location, 10, 10, FColor::Blue, false, 5);
		DrawDebugSphere(world, hitResult.ImpactPoint, 10, 10, FColor::Magenta, false, 5);
		UE_LOG(LogTemp, Display, TEXT("I HIT %s"), *actorName);*/
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicalHandle() const
{
	return GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
}

bool UGrabber::GetGrabbableInReach(FHitResult& OutHitResult) const
{
	UWorld* world = GetWorld();
	FVector start = GetComponentLocation();
	FVector end = start + GetForwardVector() * MaxGrabDistance;
	DrawDebugLine(world, start, end, FColor::Red);

	FCollisionShape sphere = FCollisionShape::MakeSphere(GrabRadius);
	bool hasHit = world->SweepSingleByChannel(
		OutHitResult,
		start, end,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		sphere
	);

	return hasHit;
}

