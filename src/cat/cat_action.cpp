#include "cat_action.h"

const char* catActionName(CatActionId id) {
    switch (id) {
        case ACTION_NONE:              return "NONE";
        case ACTION_WALK_DOWN:         return "WALK_DOWN";
        case ACTION_WALK_UP:           return "WALK_UP";
        case ACTION_WALK_RIGHT:        return "WALK_RIGHT";
        case ACTION_WALK_LEFT:         return "WALK_LEFT";
        case ACTION_WALK_RIGHT_UP:     return "WALK_RIGHT_UP";
        case ACTION_WALK_LEFT_UP:      return "WALK_LEFT_UP";
        case ACTION_WALK_RIGHT_DOWN:   return "WALK_RIGHT_DOWN";
        case ACTION_WALK_LEFT_DOWN:    return "WALK_LEFT_DOWN";
        case ACTION_SLEEP_1_LEFT:      return "SLEEP_1_LEFT";
        case ACTION_SLEEP_1_RIGHT:     return "SLEEP_1_RIGHT";
        case ACTION_SLEEP_2_LEFT:      return "SLEEP_2_LEFT";
        case ACTION_SLEEP_2_RIGHT:     return "SLEEP_2_RIGHT";
        case ACTION_SLEEP_3_LEFT:      return "SLEEP_3_LEFT";
        case ACTION_SLEEP_3_RIGHT:     return "SLEEP_3_RIGHT";
        case ACTION_SLEEP_4_LEFT:      return "SLEEP_4_LEFT";
        case ACTION_SLEEP_4_RIGHT:     return "SLEEP_4_RIGHT";
        case ACTION_EAT_DOWN:          return "EAT_DOWN";
        case ACTION_MEOW_SIT:          return "MEOW_SIT";
        case ACTION_YAWN_SIT:          return "YAWN_SIT";
        case ACTION_WASH_SIT:          return "WASH_SIT";
        case ACTION_SCRATCH_LEFT:      return "SCRATCH_LEFT";
        case ACTION_HISS_LEFT:         return "HISS_LEFT";
        case ACTION_PAW_ATTACK_DOWN:   return "PAW_ATTACK_DOWN";
        case ACTION_ON_HIND_LEGS:      return "ON_HIND_LEGS";
        default:                       return "UNKNOWN";
    }
}

const char* catCategoryName(CatActionCategory cat) {
    switch (cat) {
        case CAT_ACTION_REST:          return "REST";
        case CAT_ACTION_WALK:          return "WALK";
        case CAT_ACTION_SLEEP:         return "SLEEP";
        case CAT_ACTION_EAT:           return "EAT";
        case CAT_ACTION_MEOW:          return "MEOW";
        case CAT_ACTION_YAWN:          return "YAWN";
        case CAT_ACTION_WASH:          return "WASH";
        case CAT_ACTION_ITCH:          return "ITCH";
        case CAT_ACTION_HISS:          return "HISS";
        case CAT_ACTION_PAW_ATTACK:    return "PAW_ATTACK";
        case CAT_ACTION_ON_HIND_LEGS:  return "ON_HIND_LEGS";
        default:                       return "UNKNOWN";
    }
}

const char* catEmotionName(CatEmotion emotion) {
    switch (emotion) {
        case EMOTION_NEUTRAL:   return "NEUTRAL";
        case EMOTION_CURIOUS:   return "CURIOUS";
        case EMOTION_SLEEPY:    return "SLEEPY";
        case EMOTION_RELAXED:   return "RELAXED";
        case EMOTION_HUNGRY:    return "HUNGRY";
        case EMOTION_HAPPY:     return "HAPPY";
        case EMOTION_BORED:     return "BORED";
        case EMOTION_ANNOYED:   return "ANNOYED";
        case EMOTION_ANGRY:     return "ANGRY";
        case EMOTION_PLAYFUL:   return "PLAYFUL";
        case EMOTION_SURPRISED: return "SURPRISED";
        default:                return "UNKNOWN";
    }
}
