#include <Keyboard.h>
#include "inputs_config.h"

/* -----------------------------
   HELPER FUNCTIONS
----------------------------- */
void pressWithModifiers(uint8_t key, const uint8_t* mods, bool pressed) {
    if(!key) return;
    for(uint8_t i = 0; i < 3; i++) {
        if(mods[i]) {
            if(pressed) Keyboard.press(mods[i]);
            else Keyboard.release(mods[i]);
        }
    }
    if(pressed) Keyboard.press(key);
    else Keyboard.release(key);
}

Predicate* getPredicate(const char* name){
    for(uint8_t i=0;i<PRED_COUNT;i++){
        if(strcmp(predicates[i].id,name)==0) return &predicates[i];
    }
    return nullptr;
}

VirtualKey* getVirtualKey(const char* name){
    for(uint8_t i=0;i<VK_COUNT;i++){
        if(strcmp(virtualKeys[i].id,name)==0) return &virtualKeys[i];
    }
    return nullptr;
}

void onPredicateChanged(const char* predName, bool newState) {
    // ---------- PREDICATE OFF ----------
    if (!newState) {

        for (uint8_t i = 0; i < PHYS_COUNT; i++) {
            PhysicalInput* pi = &physicalInputs[i];

            if (pi->heldVK &&
                pi->heldVK->predicate &&
                strcmp(pi->heldVK->predicate, predName) == 0) {

                pressWithModifiers(
                    pi->heldVK->key,
                    pi->heldVK->modifiers,
                    false
                );

                pi->heldVK->isDown = false;
                pi->heldVK = nullptr;
            }
        }
    }
    // ---------- PREDICATE ON ----------
    else {

        for (uint8_t i = 0; i < PHYS_COUNT; i++) {
            PhysicalInput* pi = &physicalInputs[i];

            if (pi->type != BUTTON) continue;
            if (pi->mode != SEND_KEY) continue;
            if (pi->heldVK) continue;
            if (!(!digitalRead(pi->pinA))) continue; // physically held

            const char* keyName = pi->keyOn;
            if (!keyName) continue;

            VirtualKey* vk = getVirtualKey(keyName);
            if (!vk) continue;
            if (!vk->predicate) continue;
            if (strcmp(vk->predicate, predName) != 0) continue;

            pressWithModifiers(vk->key, vk->modifiers, true);
            vk->isDown = true;
            pi->heldVK = vk;
        }
    }
}

/* -----------------------------
   SETUP
----------------------------- */
void setup(){
    delay(500);          // let the board settle
    Keyboard.begin();

    for(uint8_t i = 0; i < PHYS_COUNT; i++){
        pinMode(physicalInputs[i].pinA, INPUT_PULLUP);
        if(physicalInputs[i].type == TOGGLE_3WAY)
            pinMode(physicalInputs[i].pinB, INPUT_PULLUP);

        physicalInputs[i].lastState = -1;       // initial lastState
        physicalInputs[i].stableState = -1;     // initial debounced state
        physicalInputs[i].lastChangeTime = 0;   // debounce timer start
    }
}


/* -----------------------------
   LOOP
----------------------------- */
void loop() {

    uint32_t now = millis();

    for (uint8_t i = 0; i < PHYS_COUNT; i++) {
        PhysicalInput* pi = &physicalInputs[i];

        // ----- READ PHYSICAL STATE -----
        bool a = !digitalRead(pi->pinA);
        bool b = (pi->type == TOGGLE_3WAY) ? !digitalRead(pi->pinB) : false;

        int8_t rawState = -1; // -1 = open circuit (button), 0 = closed circuit (pinA), 1 = open circuit (toggles), 2 = closed circuit (pinB)

        if (pi->type == BUTTON)
            rawState = a ? 0 : -1;
        else if (pi->type == TOGGLE_2WAY)
            rawState = a ? 0 : 1;
        else if (pi->type == TOGGLE_3WAY) {
            if (a) rawState = 0;
            else if (b) rawState = 2;
            else rawState = 1;
        }

        // ----- DEBOUNCE -----
        if (rawState != pi->stableState) {
            if (now - pi->lastChangeTime >= 20) {  // 20ms debounce
                pi->stableState = rawState;
                pi->lastChangeTime = now;
            }
        } else {
            pi->lastChangeTime = now;  // reset timer if state stable
        }

        int8_t state = pi->stableState;

        // ================= SEND_KEY =================
        if (pi->mode == SEND_KEY) {

            // ---------- BUTTON ----------
            if (pi->type == BUTTON) {

                // PRESS
                if (state == 0 && pi->lastState != 0) {

                    const char* keyName = pi->keyOn;
                    VirtualKey* vk = keyName ? getVirtualKey(keyName) : nullptr;

                    if (vk) {
                        bool allowed = true;
                        if (vk->predicate) {
                            Predicate* p = getPredicate(vk->predicate);
                            if (!p || !p->state) allowed = false;
                        }

                        if (allowed) {
                            pressWithModifiers(vk->key, vk->modifiers, true);
                            vk->isDown = true;
                            pi->heldVK = vk;   // latch
                        }
                    }
                }

                // RELEASE
                if (state == -1 && pi->lastState == 0) {
                    if (pi->heldVK) {
                        pressWithModifiers(
                            pi->heldVK->key,
                            pi->heldVK->modifiers,
                            false
                        );
                        pi->heldVK->isDown = false;
                        pi->heldVK = nullptr;
                    }
                }
            }

            // ---------- TOGGLES ----------
            else if (state != pi->lastState && state != -1) {

                const char* keyName = nullptr;
                if (state == 0) keyName = pi->keyOn;
                else if (state == 1) 
                {
                    if(pi->type == TOGGLE_2WAY) keyName = pi->keyOff;
                    else if(pi->type == TOGGLE_3WAY) keyName = pi->keyCenter; 
                }
                else if (state == 2) keyName = pi->keyOff;

                if (keyName) {
                    VirtualKey* vk = getVirtualKey(keyName);
                    if (vk) {
                        bool allowed = true;
                        if (vk->predicate) {
                            Predicate* p = getPredicate(vk->predicate);
                            if (!p || !p->state) allowed = false;
                        }
                        if (allowed) {
                            pressWithModifiers(vk->key, vk->modifiers, true);
                            pressWithModifiers(vk->key, vk->modifiers, false);
                        }
                    }
                }
            }
        }

        // ================= SET_PREDICATE =================
        if (pi->mode == SET_PREDICATE && state != pi->lastState && state != -1) {
            
            const char* preds[3] = {
                pi->predicatePos1,
                pi->predicatePos2,
                pi->predicatePos3
            };

            for (int j = 0; j < 3; j++) {
                if (!preds[j]) continue;

                Predicate* p = getPredicate(preds[j]);
                if (!p) continue;

                bool newState = (j == state);
                if (p->state != newState) {
                    p->state = newState;
                    onPredicateChanged(p->id, newState);  // use id
                }
            }
        }

        // ----- ALWAYS UPDATE LAST STATE -----
        if (state != pi->lastState) {
            pi->lastState = state;
        }
    }

    delay(5);
}


