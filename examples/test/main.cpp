#include "Buzzer.hpp"

buzzer_note trill[] = { {1, 20}, {0, 20}, {1, 20}, {0, 20}, {1, 20}, {0, 0} };

Buzzer buzzer(13, true);

void setup() {
    buzzer.begin();
    buzzer.click();
    buzzer.chirp();
    buzzer.beep(100);
    buzzer.beep(100, 1000);
    buzzer.play(trill);
}

void loop() {

}
