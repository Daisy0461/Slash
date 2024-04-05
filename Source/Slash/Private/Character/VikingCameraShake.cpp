// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VikingCameraShake.h"

UVikingCameraShake::UVikingCameraShake()
{
    OscillationDuration = 0.5f;
    OscillationBlendInTime = 0.1f;
    OscillationBlendOutTime = 0.2f;

    RotOscillation.Pitch.Amplitude = 5.0f;
    RotOscillation.Pitch.Frequency = 1.0f;
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Pitch.Waveform = EOscillatorWaveform::PerlinNoise;

    RotOscillation.Yaw.Amplitude = 2.5f;
    RotOscillation.Yaw.Frequency = 0.2f;
    RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

    RotOscillation.Roll.Amplitude = 2.5f;
    RotOscillation.Roll.Frequency = 0.2f;
    RotOscillation.Roll.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Roll.Waveform = EOscillatorWaveform::SineWave;

}
