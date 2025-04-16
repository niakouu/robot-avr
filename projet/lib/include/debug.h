//
// Travail : Projet Final
// Section # : 4
// Équipe # : 8290
// Correcteur : Dorine Dantrun
// ----------------------------------------------------------
// Développeurs : Edelina Alieva, Kyle Bouchard, Ilyes Belhaddad et Mohamed
// Elyes Bradai
//
// Description du debug.h :
//        Fournit des macros de logging conditionnelle (INFO, WARNING,
//        ERROR) activées uniquement en mode DEBUG.

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdio.h>

#include "Board.h"
#include "Uart.h"

#ifndef DEBUG
#define _DEBUG(x, ...)                                                         \
    /* NOLINTBEGIN(cppcoreguidelines-avoid-do-while) */                        \
    do {                                                                       \
    } while (0) /* NOLINTEND(cppcoreguidelines-avoid-do-while) */
#else
// Inspired from https://stackoverflow.com/a/27351464
#define _DEBUG(severity, fmt, ...)                                             \
    /* NOLINTBEGIN(cppcoreguidelines-avoid-do-while) */                        \
    do {                                                                       \
        fprintf(Board::get().getUart0().getEmulatedFile(),                     \
                "%s: %s:%d:%s: ", severity, __FILE__, __LINE__, __func__);     \
        fprintf(Board::get().getUart0().getEmulatedFile(),                     \
                fmt __VA_OPT__(, ) __VA_ARGS__);                               \
    } while (0) /* NOLINTEND(cppcoreguidelines-avoid-do-while) */
#endif /* DEBUG */

#define INFO(fmt, ...) _DEBUG("INFO", fmt, __VA_ARGS__)
#define WARNING(fmt, ...) _DEBUG("WARNING", fmt, __VA_ARGS__)
#define ERROR(fmt, ...) _DEBUG("ERROR", fmt, __VA_ARGS__)

#endif /* _DEBUG_H */