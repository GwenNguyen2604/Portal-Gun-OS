#include "Speakers.h"

#if ENABLE_SOUND

////////////////////////////////////////////////////////////////////////////////////////
//////////                        OBJECTS DEFINITION                          //////////

Speaker speaker1(PIN_SPEAKER1_RX, PIN_SPEAKER1_TX, VOLUME_PLAYER1, 1);
Speaker speaker2(PIN_SPEAKER2_RX, PIN_SPEAKER2_TX, VOLUME_PLAYER2, 2);

static constexpr uint16_t DFPLAYER_COMMAND_SETTLE_MS = 200;


////////////////////////////////////////////////////////////////////////////////////////
//////////                       FUNCTION DEFINITIONS                         //////////

bool
SetupSpeakers()
{
    debugPrintln("> Setup Sound... ");

    speaker1.SoftSerial.begin(9600);
    speaker2.SoftSerial.begin(9600);

    bool player1Ready = speaker1.dfPlayer.begin(speaker1.SoftSerial);
    bool player2Ready = speaker2.dfPlayer.begin(speaker2.SoftSerial);

    if (!player1Ready) {
        delay(1000);
        player1Ready = speaker1.dfPlayer.begin(speaker1.SoftSerial);
    }
    if (!player2Ready) {
        delay(1000);
        player2Ready = speaker2.dfPlayer.begin(speaker2.SoftSerial);
    }

    if (!player1Ready) {
        debugPrintln(">   Error: DFPlayer1 not starting!");
    }
    if (!player2Ready) {
        debugPrintln(">   Error: DFPlayer2 not starting!");
    }

    delay(1000);

    if (player1Ready) {
        speaker1.dfPlayer.stop();
    }
    if (player2Ready) {
        speaker2.dfPlayer.stop();
    }

    if (player1Ready) {
        speaker1.dfPlayer.reset();
    }
    if (player2Ready) {
        speaker2.dfPlayer.reset();
    }
    if (player1Ready || player2Ready) {
        delay(1000);
    }

    if (player1Ready) {
        uint8_t player1Attempts = 0;
        do {
            speaker1.dfPlayer.loop(Speaker1SoundFiles::IDLE_AMBIENT);
            delay(150);
            player1Attempts++;
            if (player1Attempts > 10) {
                debugPrintln(">   Error: Player 1 not responding.");
                break;
            }
        } while ((speaker1.dfPlayer.currentFlashTrack() != Speaker1SoundFiles::IDLE_AMBIENT)
                 && (speaker1.dfPlayer.currentMode() != 2));

        speaker1.dfPlayer.volume(speaker1.volumePlayer);
        delay(200);
        speaker1.isIdle = true;
        speaker1.idleLoopStarted = true;
        speaker1.playingEndTime = 0;
    }

    if (player2Ready) {
        uint8_t player2Attempts = 0;
        do {
            speaker2.dfPlayer.play(Speaker2SoundFiles::ACTIVATION);
            delay(150);
            player2Attempts++;
            if (player2Attempts > 10) {
                debugPrintln(">   Error: Player 2 not responding.");
                break;
            }
        } while ((speaker2.dfPlayer.currentFlashTrack() != Speaker2SoundFiles::ACTIVATION)
                 && (speaker2.dfPlayer.currentMode() != 2));

        speaker2.dfPlayer.volume(speaker2.volumePlayer);
        delay(200);
        speaker2.isIdle = false;
        speaker2.idleLoopStarted = false;
        speaker2.playingEndTime = 0;
    }

    if (player1Ready) {
        debugPrint(">   Player 1 Tracks: ");
        debugPrintln(speaker1.dfPlayer.numSdTracks());
    }
    if (player2Ready) {
        debugPrint(">   Player 2 Tracks: ");
        debugPrintln(speaker2.dfPlayer.numSdTracks());
    }
    debugPrintln("> Done.");

    return player1Ready && player2Ready;
}


void
HandleSpeakersIdlingStates()
{
    speaker2.UpdateIdlingState();
    speaker1.UpdateIdlingState();

    if (speaker1.isIdle && !speaker1.idleLoopStarted) {
        speaker1.PlaySound(Speaker1SoundFiles::IDLE_AMBIENT, true);
    }
}


void
PlayShootingSoundBlue()
{
    speaker2.PlaySound(random(Speaker2SoundFiles::BLUE_1, Speaker2SoundFiles::BLUE_4));
}

void
PlayShootingSoundOrange()
{
    speaker2.PlaySound(random(Speaker2SoundFiles::ORANGE_1, Speaker2SoundFiles::ORANGE_4));
}


void
PlayEncoderSelectedSound(EncoderNum encNum)
{
    Speaker1SoundFiles soundFile = Speaker1SoundFiles::IDLE_AMBIENT;
    switch (encNum) {
        case EncoderNum::POS_1:
            soundFile = Speaker1SoundFiles::RADIO_TUNE;
            break;
        case EncoderNum::POS_2:
            soundFile = static_cast<Speaker1SoundFiles>(random(Speaker1SoundFiles::CAVE_WELCOME,
                                                               Speaker1SoundFiles::GLADOS_WELCOME + 1));
            break;
        case EncoderNum::POS_3:
            soundFile = static_cast<Speaker1SoundFiles>(random(Speaker1SoundFiles::GLADOS_CAKE_1,
                                                               Speaker1SoundFiles::GLADOS_CAKE_2 + 1));
            break;
        case EncoderNum::POS_4:
            soundFile = Speaker1SoundFiles::CAVE_ABESTOS;
            break;
        case EncoderNum::POS_5:
        case EncoderNum::POS_10:
            soundFile = static_cast<Speaker1SoundFiles>(random(Speaker1SoundFiles::TURRET_1,
                                                               Speaker1SoundFiles::TURRET_4 + 1));
            break;
        case EncoderNum::POS_6:
            soundFile = Speaker1SoundFiles::GLADOS_WEIGHT;
            break;
        case EncoderNum::POS_7:
            soundFile = Speaker1SoundFiles::CAVE_MANTIS;
            break;
        case EncoderNum::POS_8:
            soundFile = Speaker1SoundFiles::GLADOS_BIRD;
            break;
        case EncoderNum::POS_9:
            soundFile = Speaker1SoundFiles::CAVE_LEMON;
            break;
        default:
            return;
    }

    int8_t soundLengthIndex = soundFile - 2;
    // Sanity check that the soundFile enum still falls in the indices range
    if (soundLengthIndex < 0 || soundLengthIndex >= SoundFile1LengthArrSize) {
        debugPrint(">   Error: " + String(soundLengthIndex) + " is of SoundFile1Length's bound");
        return;
    }

    uint32_t soundLength = SoundFile1LengthInMilli[soundLengthIndex];
    speaker1.StopAndFlush(DFPLAYER_COMMAND_SETTLE_MS, false);
    speaker1.dfPlayer.volume(VOLUME_ENCODER);
    delay(DFPLAYER_COMMAND_SETTLE_MS);
    speaker1.dfPlayer.play(soundFile);
    delay(DFPLAYER_COMMAND_SETTLE_MS);
    speaker1.dfPlayer.volume(VOLUME_ENCODER);
    speaker1.isIdle = false;
    speaker1.idleLoopStarted = false;
    speaker1.playingEndTime = millis() + soundLength;
}


void
PlayLowBatterySound()
{
    static uint32_t lastPlayed = 0;

    if (millis() - lastPlayed >= LOW_BATTERY_WARNING_INTERVAL) {
        speaker2.PlaySound(LOW_BATTERY);
        lastPlayed = millis();
    }
}


void
Speaker::FadeOut(uint16_t durationMs)
{
    const uint8_t startVolume = volumePlayer;

    if (startVolume == 0 || durationMs == 0) {
        dfPlayer.stopRepeat();
        dfPlayer.stopRepeatPlay();
        dfPlayer.stop();
        dfPlayer.volume(volumePlayer);
        idleLoopStarted = false;
        return;
    }

    const uint16_t stepDelay = durationMs / startVolume;
    for (int16_t volume = startVolume; volume >= 0; --volume) {
        dfPlayer.volume(static_cast<uint8_t>(volume));
        if (volume > 0 && stepDelay > 0) {
            delay(stepDelay);
        }
    }

    dfPlayer.stopRepeat();
    dfPlayer.stopRepeatPlay();
    dfPlayer.stop();
    dfPlayer.volume(volumePlayer);
    idleLoopStarted = false;
}


void
Speaker::StopAndFlush(uint16_t settleMs, bool restoreVolume)
{
    dfPlayer.stopRepeat();
    dfPlayer.stopRepeatPlay();
    dfPlayer.stop();

    if (settleMs > 0) {
        delay(settleMs);
    }

    while (SoftSerial.available() > 0) {
        SoftSerial.read();
    }
    SoftSerial.flush();
    if (restoreVolume) {
        dfPlayer.volume(volumePlayer);
    }

    isIdle = true;
    idleLoopStarted = false;
    playingEndTime = 0;
}


void
Speaker::PlaySound(uint8_t soundFile, bool isLooping, uint16_t bufferGapMs)
{
    if (!isLooping) {
        dfPlayer.play(soundFile);
        if (bufferGapMs > 0) {
            delay(bufferGapMs);
        }
        dfPlayer.volume(volumePlayer);
        isIdle = false;
        idleLoopStarted = false;
        playingEndTime = 0;
        return;
    }

    dfPlayer.loop(soundFile);
    if (bufferGapMs > 0) {
        delay(bufferGapMs);
    }
    dfPlayer.volume(volumePlayer);
    isIdle = true;
    idleLoopStarted = true;
    playingEndTime = 0;
}


void
Speaker::UpdateIdlingState()
{
    if (!isIdle) {
        bool soundFinished = false;

        if (playingEndTime > 0) {
            soundFinished = millis() > playingEndTime;
        } else {
            soundFinished = !dfPlayer.isPlaying();
        }

        if (soundFinished) {
            isIdle = true;
            idleLoopStarted = false;
            playingEndTime = 0;
        }
    }
}

#endif // ENABLE_SOUND
