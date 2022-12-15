// Fill out your copyright notice in the Description page of Project Settings.


#include "FrameAnimInstance.h"
#include "FrameCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "WeaponType.h"

UFrameAnimInstance::UFrameAnimInstance() :
    Speed(0.f),
    bIsInAir(false),
    bIsAccelerating(false),
    MovementOffsetYaw(0.f),
    LastMovementOffsetYaw(0.f),
    bAiming(false),
    CharacterRotation(FRotator(0.f)),
    CharacterRotationLastFrame(FRotator(0.f)),
    TIPCharacterYaw(0.f),
    TIPCharacterYawLastFrame(0.f),
    YawDelta(0.f),
    RootYawOffset(0.f),
    Pitch(0.f),
    bReloading(false),
    OffsetState(EOffsetState::EOS_Hip),
    RecoilWeight(1.f),
    bTurningInPlace(false),
    EquippedWeaponType(EWeaponType::EWT_MAX),
    bUseFABRIK(false)
{

}

void UFrameAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
    if (FrameCharacter == nullptr)
    {
        FrameCharacter = Cast<AFrameCharacter>(TryGetPawnOwner());
    }
    if (FrameCharacter)
    {
        bCrouching = FrameCharacter->GetCrouching();
        bReloading = FrameCharacter->GetCombatState() == ECombatState::ECS_Reloading;
        bEquipping = FrameCharacter->GetCombatState() == ECombatState::ECS_Equipping;
        bUseFABRIK = FrameCharacter->GetCombatState() == ECombatState::ECS_Unoccupied || FrameCharacter->GetCombatState() == ECombatState::ECS_FireTimerInProgress;
        
        //Get lateral speed of character from velocity
        FVector Velocity{ FrameCharacter->GetVelocity() };
        Velocity.Z = 0;
        Speed = Velocity.Size();

        //Is character in the air?
        bIsInAir = FrameCharacter->GetCharacterMovement()->IsFalling();

        //Is character accelerating?
        if (FrameCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
        {
            bIsAccelerating = true;
        }
        else
        {
            bIsAccelerating = false;
        }

        FRotator AimRotation = FrameCharacter->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(FrameCharacter->GetVelocity());

        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        if (FrameCharacter->GetVelocity().Size() > 0.f)
        {
            LastMovementOffsetYaw = MovementOffsetYaw;
        }
        
        bAiming = FrameCharacter->GetAiming();

        if (bReloading)
        {
            OffsetState = EOffsetState::EOS_Reloading;
        }
        else if (bIsInAir)
        {
            OffsetState = EOffsetState::EOS_InAir;
        }
        else if (FrameCharacter->GetAiming())
        {
            OffsetState = EOffsetState::EOS_Aiming;
        }
        else
        {
            OffsetState = EOffsetState::EOS_Hip;
        }
        //Check if player has valid equipped weapon
        if (FrameCharacter->GetEquippedWeapon())
        {
            EquippedWeaponType = FrameCharacter->GetEquippedWeapon()->GetWeaponType();
        }

    }
        TurnInPlace();
        Lean(DeltaTime);
}

void UFrameAnimInstance::NativeInitializeAnimation()
{
    FrameCharacter = Cast<AFrameCharacter>(TryGetPawnOwner());
}

void UFrameAnimInstance::TurnInPlace()
{
    if (FrameCharacter == nullptr) return;

    Pitch = FrameCharacter->GetBaseAimRotation().Pitch;

    if (Speed > 0 || bIsInAir)
    {
        //Don't want to turn in place, character is moving
        RootYawOffset = 0.f;
        TIPCharacterYaw = FrameCharacter->GetActorRotation().Yaw;
        TIPCharacterYawLastFrame = TIPCharacterYaw;
        RotationCurveLastFrame = 0.f;
        RotationCurve = 0.f;
    }
    else
    {
        TIPCharacterYawLastFrame = TIPCharacterYaw;
        TIPCharacterYaw = FrameCharacter->GetActorRotation().Yaw;
        const float TIPYawDelta{ TIPCharacterYaw - TIPCharacterYawLastFrame };

        //RootYawOffset updated and clamped to -180, 180.
        RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

        //1.0f if turning, 0.0f if not
        const float Turning{ GetCurveValue(TEXT("Turning")) };
        if (Turning > 0)
        {
            bTurningInPlace = true;
            RotationCurveLastFrame = RotationCurve;
            RotationCurve = GetCurveValue(TEXT("Rotation"));
            const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };

            //If RootYawOffset is +ve (>0), we are turning left. If RootYawOffset is <0, we are turning right
            RootYawOffset > 0 ? RootYawOffset -= DeltaRotation : RootYawOffset += DeltaRotation;

            const float ABSRootYawOffset{ FMath::Abs(RootYawOffset) };
            if (ABSRootYawOffset > 90.f)
            {
                const float YawExcess{ ABSRootYawOffset - 90.f };
                RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
            }
        }
        else
        {
            bTurningInPlace = false;
        }
    }
    //Set recoil weight
    if (bTurningInPlace)
        {
            if (bReloading || bEquipping)
            {
                RecoilWeight = 1.f;
            }
            else
            {
                RecoilWeight = 0.f;
            }
        }
        else //Not turning
        {
            if (bCrouching)
            {
                if (bReloading || bEquipping)
                {
                    RecoilWeight = 1.f;
                }
                else
                {
                    RecoilWeight = 0.1f;
                }
            }
            else
            {
                if (bAiming || bReloading || bEquipping)
                {
                    RecoilWeight = 1.f;
                }
                else
                {
                    RecoilWeight = 0.5f;
                }
            }
        }
}

void UFrameAnimInstance::Lean(float DeltaTime)
{
    if (FrameCharacter == nullptr) return;
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = FrameCharacter->GetActorRotation();

    const FRotator Delta{ UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame) };

    const float Target = Delta.Yaw / DeltaTime;
    const float Interp{ FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f) };
    YawDelta = FMath::Clamp(Interp, -90.f, 90.f);

    
}   

