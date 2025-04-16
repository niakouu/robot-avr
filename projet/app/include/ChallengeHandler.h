//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du ChallengeHandler.h :
//        Déclare une interface commune partagée entre tous les défis du
//        parcours (Fork, House, Maze).

#ifndef _CHALLENGE_HANDLER_H
#define _CHALLENGE_HANDLER_H

#include <stdint.h>

class Challenge;

class ChallengeHandler {
public:
    virtual bool isDone() = 0;
    virtual void update(uint16_t deltaTimeMs, Challenge& challenge) = 0;
};

#endif /* _CHALLENGE_HANDLER_H */