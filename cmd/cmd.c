#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#define PATH_SUFFIX_SIZE 6

void info_proc(pid_t pid) {
  if (pid == 0) {
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
    printf("%s \n", pdir->d_name);
    if (strcmp(pdir->d_name, "cmdline") == 0) {
      // Lecture de fichier
      printf(" =========== CONTENU DE CMDLINE =============== \n");
      char f_path[PATH_MAX + FILENAME_MAX];
      snprintf(f_path, strlen(dir_path) + strlen(
          pdir->d_name) + 3, "%s/%s", dir_path, pdir->d_name);
      FILE *f = fopen(f_path, "r");
      if (f == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
      }
      char *line = NULL;
      size_t len = 0;
      ssize_t read;
      while ((read = getline(&line, &len, f)) != -1) {
        printf("%s \n", line);
      }
      free(line);
      fclose(f);
      printf(" ============================================= \n");
    }
  }
  closedir(d);
}
