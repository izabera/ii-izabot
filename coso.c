#include <stdio.h>
#include <string.h>
int main () {
  char buf [8];
  char temp [8];
  FILE *fp;
  int i;
  fp = fopen ("fil","r");
  for (i=0;i<8;i++) {
    fgets(buf,10,fp);
    if (ferror(fp)) printf("ferror\n");
    if (feof(fp)) printf("feof\n");
    strncpy(temp, buf, 8);
    printf("%d = '%s' strncpy\n",i,temp);
  }
  if (ferror(fp)) printf("error\n");
}
