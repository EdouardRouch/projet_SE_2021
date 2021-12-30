#ifndef CLIENT_RESOURCES__H
#define CLIENT_RESOURCES__H

// info_proc : affiche les caractéristiques du processus du pid donné en arg
extern void info_proc(pid_t pid);


// info_user_uid : affiche les caractéristiques d'un utilisateur donné
//      par son uid
extern void info_user_uid(uid_t uid) {

// info_user_name : affiche les caractéristiques d'un utilisateur donné
//      par son nom
extern void info_user_name(const char *name) {


#endif
