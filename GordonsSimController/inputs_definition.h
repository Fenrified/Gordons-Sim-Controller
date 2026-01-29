#pragma once
#include <Keyboard.h>

/* -----------------------------
   PREDICATES
----------------------------- */
struct Predicate {
    const char* id;
    bool state;
};

#define PRED(name) \
    {name, false}

/* -----------------------------
   VIRTUAL KEYS
----------------------------- */
struct VirtualKey {
    const char* id;
    uint8_t key;
    const uint8_t* modifiers; // pointer to modifier array
    const char* predicate;    // Only fires if predicate true
    bool isDown = false;
};

// Modifier arrays
const uint8_t MOD_NONE[3]           = {0, 0, 0};
const uint8_t MOD_CTRL[3]           = {KEY_LEFT_CTRL, 0, 0};
const uint8_t MOD_SHIFT[3]          = {KEY_LEFT_SHIFT, 0, 0};
const uint8_t MOD_ALT[3]            = {KEY_LEFT_ALT, 0, 0};
const uint8_t MOD_CTRL_SHIFT[3]     = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 0};
const uint8_t MOD_CTRL_ALT[3]       = {KEY_LEFT_CTRL, KEY_LEFT_ALT, 0};
const uint8_t MOD_SHIFT_ALT[3]      = {KEY_LEFT_SHIFT, KEY_LEFT_ALT, 0};
const uint8_t MOD_CTRL_SHIFT_ALT[3] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT};

// VirtualKey macros
#define VIRTUAL_KEY(name, key) \
    {name, key, MOD_NONE, nullptr}

#define VIRTUAL_KEY_PRED(name, key, pred) \
    {name, key, MOD_NONE, pred}

#define VIRTUAL_KEY_MOD(name, key, mods) \
    {name, key, mods, nullptr}

#define VIRTUAL_KEY_FULL(name, key, mods, pred) \
    {name, key, mods, pred}

/* -----------------------------
   PHYSICAL INPUTS
----------------------------- */
enum InputMode { SEND_KEY, SET_PREDICATE };
enum ToggleType { BUTTON, TOGGLE_2WAY, TOGGLE_3WAY };

struct PhysicalInput {
    const char* id;
    ToggleType type;
    uint8_t pinA;
    uint8_t pinB;
    InputMode mode;

    // SEND_KEY fields
    const char* keyOn;
    const char* keyCenter;
    const char* keyOff;

    // SET_PREDICATE fields
    const char* predicatePos1;
    const char* predicatePos2;
    const char* predicatePos3;

    VirtualKey* heldVK;
    int8_t lastState;
    int8_t stableState;
    uint32_t lastChangeTime;
};

// Button sending a key
#define BUTTON(id,pin,key) \
    {id, BUTTON, pin, 255, SEND_KEY, key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, -1, -1, 0}

// 2-way toggle sending keys
#define TOGGLE_2WAY_KEY(id,pin,keyUp,keyDown) \
    {id, TOGGLE_2WAY, pin, 255, SEND_KEY, keyUp, nullptr, keyDown, nullptr, nullptr, nullptr, nullptr, -1, -1, 0}

// 3-way toggle sending keys
#define TOGGLE_3WAY_KEY(id,pinA,pinB,keyUp,keyCenter,keyDown) \
    {id, TOGGLE_3WAY, pinA, pinB, SEND_KEY, keyUp, keyCenter, keyDown, nullptr, nullptr, nullptr, nullptr, -1, -1, 0}

// 2-way toggle setting predicates (UP, DOWN)
#define TOGGLE_2WAY_PRED(id,pin,predUp,predDown) \
    {id, TOGGLE_2WAY, pin, 255, SET_PREDICATE, nullptr, nullptr, nullptr, predUp, predDown, nullptr, nullptr, -1, -1, 0}

// 3-way toggle setting predicates (UP, CENTER, DOWN)
#define TOGGLE_3WAY_PRED(id,pinA,pinB,predUp,predCenter,predDown) \
    {id, TOGGLE_3WAY, pinA, pinB, SET_PREDICATE, nullptr, nullptr, nullptr, predUp, predCenter, predDown, nullptr, -1, -1, 0}
