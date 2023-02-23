#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CGameState.h"
#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;

UCLASS(config=Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFP_FirstPersonCharacter();

protected:
	/** Fires a virtual projectile. */
	void OnFire();

	UFUNCTION(Server, Reliable)
		void OnServerFire(const FVector& LineStart, const FVector& LineEnd);
	void OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastFireEffect();
	void MulticastFireEffect_Implementation();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayDamage();
	void PlayDamage_Implementation();
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDead();
	void PlayDead_Implementation();

public:
	UFUNCTION(NetMulticast, Reliable)
		void SetTeamColor(ETeamTypes InTeamType);
	void SetTeamColor_Implementation(ETeamTypes InTeamType);

	class ACPlayerState* GetSelfPlayerState();
	void SetSelfPlayerState(class ACPlayerState* NewPlayerState);

protected:
	virtual void PossessedBy(AController* NewController) override;
	void BeginPlay() override;
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	/* 
	 * Performs a trace between two points
	 * 
	 * @param	StartTrace	Trace starting point
	 * @param	EndTrac		Trace end point
	 * @returns FHitResult returns a struct containing trace result - who/what the trace hit etc.
	 */
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return FP_Mesh; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return Camera; }

private:
	UFUNCTION()
		void Respawn();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponDamage;

	// FP
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay")
		class UParticleSystemComponent* FP_GunShotParticle;

	// TP
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_HitAnimation;

	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay")
		class UParticleSystemComponent* TP_GunShotParticle;
	
public:
	UPROPERTY(Replicated)
		ETeamTypes CurrentTeam;
	
private:
	class UMaterialInstanceDynamic* DynamicMaterial;
	class ACPlayerState* SelfPlayerState;
};
