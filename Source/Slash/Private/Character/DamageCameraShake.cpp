#include "Character/DamageCameraShake.h"

UDamageCameraShake:: UDamageCameraShake()
{
    OscillationDuration = 0.3f;
    OscillationBlendInTime = 0.1f;
    OscillationBlendOutTime = 0.2f;

    RotOscillation.Pitch.Amplitude = 3.0f;
    RotOscillation.Pitch.Frequency = 2.0f;
    RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    RotOscillation.Pitch.Waveform = EOscillatorWaveform::PerlinNoise;

    LocOscillation.Y.Amplitude = 2.0f;
    LocOscillation.Y.Frequency = 1.0f;
    LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;

    LocOscillation.Z.Amplitude = 2.0f;
    LocOscillation.Z.Frequency = 1.0f;
    LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
    LocOscillation.Z.Waveform = EOscillatorWaveform::SineWave;
}