#pragma once
#include <Keyboard.h>
#include "inputs_definition.h"

/* -----------------------------
   PREDICATES
----------------------------- */
Predicate predicates[] = {
    PRED("normal"),
    PRED("combat")
};


/* -----------------------------
   VIRTUAL KEYS
----------------------------- */
VirtualKey virtualKeys[] = {
    VIRTUAL_KEY("power_on", 'p'),
    VIRTUAL_KEY_MOD("power_off", 'p', MOD_ALT),
    VIRTUAL_KEY("gear_down", '0'),
    VIRTUAL_KEY("gear_up", '9'),
    VIRTUAL_KEY("nav_lights_on", 'n'),
    VIRTUAL_KEY_MOD("nav_lights_off", 'n', MOD_ALT),
    VIRTUAL_KEY_PRED("boost", 'b', "normal"),
    VIRTUAL_KEY_PRED("autoland", 'l', "normal"),
    VIRTUAL_KEY_PRED("camera", KEY_F4, "normal"),
    VIRTUAL_KEY_PRED("eject", KEY_BACKSPACE, "normal"),
    VIRTUAL_KEY_PRED("weapon_1", '1', "combat"),
    VIRTUAL_KEY_PRED("weapon_2", '2', "combat"),
    VIRTUAL_KEY_PRED("weapon_3", '3', "combat"),
    VIRTUAL_KEY_PRED("weapon_4", '4', "combat") 
};


/* -----------------------------
   PHYSICAL INPUTS
----------------------------- */
PhysicalInput physicalInputs[] = {
    TOGGLE_2WAY_KEY("power", 9, "power_on", "power_off"),
    TOGGLE_2WAY_KEY("gear", 8, "gear_up", "gear_down"),
    TOGGLE_2WAY_KEY("nav_lights", 7, "nav_lights_on", "nav_lights_off"),
    TOGGLE_2WAY_PRED("mode_select", 6, "normal", "combat"),
    BUTTON("boost", 10, "boost"),
    BUTTON("wpn_1", 10, "weapon_1"),
    BUTTON("autoland", 11, "autoland"),
    BUTTON("wpn_2", 11, "weapon_2"),
    BUTTON("camera", 12, "camera"),
    BUTTON("wpn_3", 12, "weapon_3"),
    BUTTON("eject", 13, "eject"),
    BUTTON("wpn_4", 13, "weapon_4")
};

/* --- Counts --- */
const uint8_t PHYS_COUNT = sizeof(physicalInputs)/sizeof(physicalInputs[0]);
const uint8_t VK_COUNT   = sizeof(virtualKeys)/sizeof(virtualKeys[0]);
const uint8_t PRED_COUNT = sizeof(predicates)/sizeof(predicates[0]);
