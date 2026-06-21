#include "fermier.h"

ActionFermier decider_action_fermier(Fermier *f, PerceptionFermier p) {
    if (p.input_y == 1) {
        f->action_choisi = ACTION_FERMIER_BOUGER_HAUT;
    } else if (p.input_y == -1) {
        f->action_choisi = ACTION_FERMIER_BOUGER_BAS;
    } else if (p.input_x == 1) {
        f->action_choisi = ACTION_FERMIER_BOUGER_GAUCHE;
    } else if (p.input_x == -1) {
        f->action_choisi = ACTION_FERMIER_BOUGER_DROITE;
    } else {
        f->action_choisi = ACTION_FERMIER_IMMOBILE;
    }
    
    return f->action_choisi;
}