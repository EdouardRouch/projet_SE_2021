#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif


#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#define PATH_SUFFIX_SIZE 6


// print_content_dir_pid : lis le contenu du dossier
void print_content_dir_pid(const char *f_path, struct dirent *pdir);

// not_selected_caract_pid : sélectionne les dossiers que l'on veut lire pour
//    la lecture des caractéristiques du processus
bool not_selected_caract_pid(const char *s);

void info_proc(pid_t pid) {
  if (pid <= 0) {
    printf("Affichage de processus courant"
        " dû à un argument non strictement positif. \n");
    pid = getpid();
  }
  // Ouvrir le flot vers le dossier
  char dir_path[PATH_MAX];
  size_t pid_length = (size_t) ((ceil(log10(pid)) + 1) * sizeof(char));
  snprintf(dir_path, pid_length + PATH_SUFFIX_SIZE, "/proc/%d", pid);

  DIR *d = opendir(dir_path);
  if (d == NULL) {
    perror("opendir : ");
    exit(EXIT_FAILURE);
  }

  // Gérer le fichier
  struct dirent *pdir;
  while ((pdir = readdir(d)) != NULL) {
    // Lecture de fichier
    char f_path[PATH_MAX + FILENAME_MAX];
    snprintf(f_path, strlen(dir_path) + strlen(
        pdir->d_name) + 3, "%s/%s", dir_path, pdir->d_name);

    if (not_selected_caract_pid(pdir->d_name)) {
      print_content_dir_pid(f_path, pdir);
    }
  }
  closedir(d);
}

void print_content_dir_pid(const char *f_path, struct dirent *pdir) {
  // Lecture de fichier
  FILE *f = fopen(f_path, "r");
  if (f == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  printf(" =========== CONTENU DE %s ==================== \n", pdir->d_name);
  while ((read = getline(&line, &len, f)) != -1) {
    printf("%s \n", line);
  }
  printf(" ============================================== \n");
  free(line);
  fclose(f);
}

bool not_selected_caract_pid(const char *s) {
  if (s == NULL) {
    return false;
  }
  if (strcmp(s, ".") == 0
      || strcmp(s, "..") == 0
      || strcmp(s, "exe") == 0
      || strcmp(s, "mounts") == 0
      || strcmp(s, "mountinfo") == 0
      || strcmp(s, "mountstats") == 0) {
    return false;
  }
  return true;
}
