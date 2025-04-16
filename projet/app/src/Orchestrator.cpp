#include "Orchestrator.h"

#include "Robot.h"

Orchestrator Orchestrator::orchestrator_{Robot::get().getMidi()};

Orchestrator::Orchestrator(Midi& midiPlayer)
    : isPaused_(true), song_(nullptr), sustainTimeLeft_(0), silenceTimeLeft_(0),
      songLength_(0), songIndex_(0), midiPlayer_(midiPlayer) {}

Orchestrator& Orchestrator::get() {
    return Orchestrator::orchestrator_;
}

void Orchestrator::loadSong(const music::MidiNote* song, size_t songLength) {
    this->song_ = song;
    this->songLength_ = songLength;
    this->songIndex_ = 0;

    if (this->song_ != nullptr && this->songLength_ > 0) {
        this->sustainTimeLeft_ = this->song_[0].sustainTimeMs;
        this->silenceTimeLeft_ = this->song_[0].silenceTimeMs;
    }
}

void Orchestrator::update(uint16_t deltaTimeMs) {
    if (this->isPaused_)
        return;

    if (this->songIndex_ >= this->songLength_) {
        this->pause();
        return;
    }

    const music::MidiNote& note = this->song_[songIndex_];
    if (this->sustainTimeLeft_ > 0) {
        if (this->sustainTimeLeft_ == note.sustainTimeMs)
            this->midiPlayer_.playNote(note.note);

        this->sustainTimeLeft_ =
            ::saturatingSubtract(this->sustainTimeLeft_, deltaTimeMs);
    } else if (this->silenceTimeLeft_ > 0) {
        if (this->silenceTimeLeft_ == note.silenceTimeMs)
            this->midiPlayer_.stop();

        this->silenceTimeLeft_ =
            ::saturatingSubtract(this->silenceTimeLeft_, deltaTimeMs);
    } else {
        ++this->songIndex_;

        if (this->songIndex_ < this->songLength_) {
            this->sustainTimeLeft_ =
                this->song_[this->songIndex_].sustainTimeMs;
            this->silenceTimeLeft_ =
                this->song_[this->songIndex_].silenceTimeMs;
        }
    }
}

void Orchestrator::play() {
    this->isPaused_ = false;
}

void Orchestrator::pause() {
    if (!this->isPaused_)
        this->midiPlayer_.stop();

    this->isPaused_ = true;
}

bool Orchestrator::isPaused() const {
    return this->isPaused_;
}
