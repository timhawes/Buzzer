// SPDX-FileCopyrightText: 2018-2023 Tim Hawes
//
// SPDX-License-Identifier: MIT

#include <Buzzer.hpp>

Buzzer::Buzzer(int pin, bool modulate, double default_frequency)
{
    _pin = pin;
    _modulate = modulate;
    _default_frequency = default_frequency;
#ifdef ESP8266
    ticker.attach_ms(5, std::bind(&Buzzer::callback, this));
#else
    ticker.attach_ms(5, +[](Buzzer* buzzer) { buzzer->callback(); }, this));
#endif
}

void Buzzer::callback()
{
    if (playlist_active && (long)(millis() - next_change) >= 0) {
        next_note();
    } else if (active && (long)(millis() - next_change) >= 0) {
        stop();
    }
}

void Buzzer::stop()
{
    noTone(_pin);
    digitalWrite(_pin, LOW);
    active = false;
}

void Buzzer::beep(unsigned int ms)
{
    noTone(_pin);
    next_change = millis() + ms;
    active = true;
    if (_modulate) {
        tone(_pin, _default_frequency);
    } else {
        digitalWrite(_pin, HIGH);
    }
}

void Buzzer::beep(unsigned int ms, double frequency)
{
    active = true;
    next_change = millis() + ms;
    tone(_pin, frequency);
}

void Buzzer::chirp()
{
    beep(1);
}

void Buzzer::click()
{
    if (!active) {
        digitalWrite(_pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(_pin, LOW);
    }
}

void Buzzer::play(buzzer_note *playlist)
{
    current_playlist = playlist;
    playlist_position = 0;
    playlist_active = true;
    active = true;
    next_note();
}

void Buzzer::next_note()
{
    if (playlist_active) {
        if (current_playlist[playlist_position].ms > max_note_length) {
            // we might be reading from a corrupt/deallocated buffer, abort
            stop();
            playlist_active = false;
            return;
        }
        if (current_playlist[playlist_position].frequency == 0 && current_playlist[playlist_position].ms == 0) {
            stop();
            playlist_active = false;
            return;
        }
        if (current_playlist[playlist_position].frequency == 0) {
            stop();
        } else if (current_playlist[playlist_position].frequency == 1) {
            if (_modulate) {
                tone(_pin, _default_frequency);
            } else {
                noTone(_pin);
                digitalWrite(_pin, HIGH);
            }
        } else {
            tone(_pin, current_playlist[playlist_position].frequency);
        }
        next_change = millis() + current_playlist[playlist_position].ms;
        playlist_position++;
    } else {
        stop();
        playlist_active = false;
    }
}
