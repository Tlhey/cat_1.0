#pragma once
// AUTO-GENERATED sequence table.  Each CatSeq is a (frames[], count) pair.
#include "cat_frames.h"

struct CatSeq { const CatFrame* const* f; uint8_t n; };

// Walk (8 directions)
extern const CatSeq SEQ_WALK_DOWN, SEQ_WALK_UP;
extern const CatSeq SEQ_WALK_RIGHT, SEQ_WALK_LEFT;
extern const CatSeq SEQ_WALK_DOWN_LEFT, SEQ_WALK_DOWN_RIGHT;
extern const CatSeq SEQ_WALK_UP_RIGHT,  SEQ_WALK_UP_LEFT;

// Rest
extern const CatSeq SEQ_REST_1, SEQ_REST_2, SEQ_REST_3, SEQ_REST_4;

// Sleep (8 poses)
extern const CatSeq SEQ_SLEEP_1_L, SEQ_SLEEP_1_R;
extern const CatSeq SEQ_SLEEP_2_L, SEQ_SLEEP_2_R;
extern const CatSeq SEQ_SLEEP_3_L, SEQ_SLEEP_3_R;
extern const CatSeq SEQ_SLEEP_4_L, SEQ_SLEEP_4_R;

// Eat (8 directions)
extern const CatSeq SEQ_EAT_DOWN,      SEQ_EAT_UP;
extern const CatSeq SEQ_EAT_LEFT,      SEQ_EAT_RIGHT;
extern const CatSeq SEQ_EAT_DOWN_RIGHT,SEQ_EAT_DOWN_LEFT;
extern const CatSeq SEQ_EAT_UP_RIGHT,  SEQ_EAT_UP_LEFT;

// Meow
extern const CatSeq SEQ_MEOW_SIT, SEQ_MEOW_STAND, SEQ_MEOW_SIT_2, SEQ_MEOW_LIE;

// Yawn
extern const CatSeq SEQ_YAWN_SIT, SEQ_YAWN_STAND, SEQ_YAWN_SIT_2, SEQ_YAWN_LIE;

// Wash
extern const CatSeq SEQ_WASH_SIT, SEQ_WASH_STAND, SEQ_WASH_LIE;

// Scratch / hiss
extern const CatSeq SEQ_SCRATCH_L, SEQ_SCRATCH_R;
extern const CatSeq SEQ_HISS_L, SEQ_HISS_R;

// Attack
extern const CatSeq SEQ_ATTACK_READY;
extern const CatSeq SEQ_PAW_DOWN,       SEQ_PAW_UP;
extern const CatSeq SEQ_PAW_LEFT,       SEQ_PAW_RIGHT;
extern const CatSeq SEQ_PAW_DOWN_RIGHT, SEQ_PAW_DOWN_LEFT;
extern const CatSeq SEQ_PAW_UP_RIGHT,   SEQ_PAW_UP_LEFT;

// On hind legs
extern const CatSeq SEQ_HIND;

// Walk seq lookup by direction index (0=DOWN 1=UP 2=RIGHT 3=LEFT
//                                    4=DOWN_LEFT 5=DOWN_RIGHT 6=UP_RIGHT 7=UP_LEFT)
extern const CatSeq* const WALK_SEQ[8];

// Eat seq lookup by same direction index
extern const CatSeq* const EAT_SEQ[8];

// Paw-attack seq lookup by same direction index
extern const CatSeq* const PAW_SEQ[8];

// Sleep pose pool
extern const CatSeq* const SLEEP_SEQ[8];
