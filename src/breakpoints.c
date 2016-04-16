#include <stdio.h>  // printf()
#include <stdlib.h> // malloc(), realloc(), free(), qsort()
#include <string.h> // strlen()

char __thread *pDNA;

int max(int a, int b) {
  if (a > b)
    return a;
  return b;
}//max

char complement(char c) {
  switch (c) {
    case 'a':
      return 't';
    case 'c':
      return 'g';
    case 'g':
      return 'c';
    case 't':
      return 'a';
  }//switch
  return 'x';
}//complement

int compar(const void *a, const void *b) {
  int oa = *(int*)a,
      ob = *(int*)b,
      i = 0;

  while ((pDNA[oa + i] != 'z') && (pDNA[ob + i] != 'z')) {
    if (pDNA[oa + i] > pDNA[ob + i])
      return 1;
    if (pDNA[oa + i] < pDNA[ob + i])
      return -1;
    i++;
  }//while
  return 0;
}//compar

int comparkey(char *DNA, char *k, int o) {
  int i = 0;

  while ((k[i] != 'z') || (DNA[o + i] != 'z')) { // FIXME ??
    if (k[i] > DNA[o + i])
      return i + 1;
    if (k[i] < DNA[o + i])
      return -(i + 1);
    i++;
  }//while
  return 0;
}//comparkey

int binsearch(char *DNA, int *suff, int DNA_s, int *p, char *key) {
  int l = 0,      // Left boundary of the binary search.
      r = DNA_s,  // Right boundary of the binary search.
      m,          // Average of the boundaries.
      temp,       // Return value of the comparison.
      max = 0,
      mpos = 0;

  while (l <= r) {
    m = (l + r) / 2;
    temp = comparkey(DNA, key, suff[m]);
    if (abs(temp) > max) {
      max = abs(temp);
      mpos = m;
    }//if
    if (!temp) { // We found it.
      *p = m;    // Put the position in the output variable.
      return -1; // Return no error.
    }//if
    if (temp > 0)
      l = m + 1; // DNA[suff[m]] > key.
    else
      r = m - 1; // DNA[suff[m]] < key.
  }//while

  // We didn't find it.
  //*p = l;   // Put the position right to where key should be in the output.
  *p = mpos;  // Return the best match.
  return max - 1; // Return an error.
}//binsearch

void printstuff(char *DNAf, char *DNAr, int *sufff, int *suffr, int DNA_s) {
  int i,
      j;

  // Print the input, the suffix array and the suffixes.
  for (i = 0; i < DNA_s; i++)
    printf("%c", DNAf[i]);
  for (i = 0; i < DNA_s; i++) {
    printf("\n%i	: ", sufff[i]);
    for (j = sufff[i]; j < DNA_s; j++)
      printf("%c", DNAf[j]);
  }//for
  printf("\n");

  for (i = 0; i < DNA_s; i++)
    printf("%c", DNAr[i]);
  for (i = 0; i < DNA_s; i++) {
    printf("\n%i	: ", suffr[i]);
    for (j = suffr[i]; j < DNA_s; j++)
      printf("%c", DNAr[j]);
  }//for
  printf("\n");
}//printstuff

char *read(char *filename, int *DNA_s) {
  FILE *input = fopen(filename, "r");
  char *DNA = NULL,
       c = '\0';

  *DNA_s = 0;

  while (c != '\n')          // Skip the first line of input.
    fread(&c, 1, 1, input);
  fread(&c, 1, 1, input);
  while (!feof(input)) {     // Read the file and put it in char *DNA.
    DNA = (char *)realloc(DNA, (*DNA_s + 1) * sizeof(char));
    switch (c) {
      case 'a':
      case 'A':
        DNA[*DNA_s] = 'a';
        (*DNA_s)++;
        break;
      case 'c':
      case 'C':
        DNA[*DNA_s] = 'c';
        (*DNA_s)++;
        break;
      case 'g':
      case 'G':
        DNA[*DNA_s] = 'g';
        (*DNA_s)++;
        break;
      case 't':
      case 'T':
        DNA[*DNA_s] = 't';
        (*DNA_s)++;
        break;
      case 'n':
      case 'N':
        DNA[*DNA_s] = 'x';
        (*DNA_s)++;
    }//switch 
    fread(&c, 1, 1, input);
  }//while
  fclose(input);

  DNA = (char *)realloc(DNA, (*DNA_s + 1) * sizeof(char));
  DNA[*DNA_s] = 'z'; // Terminating character.
  (*DNA_s)++;

  return DNA;
}//read

int *makesuff(char *DNA, int DNA_s) {
  int *suff = malloc(DNA_s * sizeof(int)),
      i;

  for (i = 0; i < DNA_s; i++)
    suff[i] = i;
  pDNA = DNA;
  qsort(suff, DNA_s, sizeof(int), compar);

  return suff;
}//makesuff

char *makerevcomp(char *DNAf, int DNA_s) {
  char *DNAr = (char *)malloc(DNA_s * sizeof(char));
  int i;

  for (i = 0; i < DNA_s - 1; i++)
    DNAr[i] = complement(DNAf[DNA_s - i - 2]);
  DNAr[DNA_s - 1] = 'z';

  return DNAr;
}//makerevcomp

int breakpoints(int ***ret, char *DNAf, int *sufff, char *DNAr, int *suffr, 
                int DNA_s, char *DNAm, int DNAm_s, int d) {
  int x = ((d - 1) / -2) * (-DNAm_s + 1),
      of,
      or,
      pf,
      pr,
      length,
      offset = 0,
      oldoffset = 0,
      rlen = 0;

  *ret = NULL;

  of = binsearch(DNAf, sufff, DNA_s, &pf, &DNAm[offset]);
  or = binsearch(DNAr, suffr, DNA_s, &pr, &DNAm[offset]);
  while ((of != -1) && (or != -1)) {
    offset += max(of, or);
    length = offset - oldoffset;

    rlen++;
    *ret = (int **)realloc(*ret, rlen * sizeof(int *));
    (*ret)[rlen - 1] = malloc(4 * sizeof(int));

    if (of > or) {
      (*ret)[rlen - 1][0] = sufff[pf];
      (*ret)[rlen - 1][2] = sufff[pf] + length;
      /*
      printf("%i,%i %i,%i ", sufff[pf], x + oldoffset, 
                             sufff[pf] + length, x + offset);
      */
    }//if
    else {
      (*ret)[rlen - 1][0] = -DNA_s + 1 + suffr[pr];
      (*ret)[rlen - 1][2] = -DNA_s + 1 + suffr[pr] + length;
      /*
      printf("%i,%i %i,%i ", -DNA_s + 1 + suffr[pr], x + oldoffset,
                             -DNA_s + 1 + suffr[pr] + length, x + offset);
      */
    }//else
    (*ret)[rlen - 1][1] = x + oldoffset;
    (*ret)[rlen - 1][3] = x + offset;
    of = binsearch(DNAf, sufff, DNA_s, &pf, &DNAm[offset]);
    or = binsearch(DNAr, suffr, DNA_s, &pr, &DNAm[offset]);
    oldoffset = offset;
  }//while

  rlen++;
  *ret = (int **)realloc(*ret, rlen * sizeof(int*));
  (*ret)[rlen - 1] = malloc(4 * sizeof(int));

  if (d == 1) {
    (*ret)[rlen - 1][0] = sufff[pf];
    (*ret)[rlen - 1][1] = oldoffset;
    (*ret)[rlen - 1][2] = DNA_s - 1;
    (*ret)[rlen - 1][3] = DNAm_s - 1;
    //printf("%i,%i %i,%i\n", sufff[pf], oldoffset, DNA_s - 1, DNAm_s - 1);
  }//if
  else {
    (*ret)[rlen - 1][0] = -DNA_s + 1 + suffr[pr];
    (*ret)[rlen - 1][1] = -DNAm_s + 1 + oldoffset;
    (*ret)[rlen - 1][2] = 0;
    (*ret)[rlen - 1][3] = 0;
    //printf("%i,%i %i,%i\n", -DNA_s + 1 + suffr[pr], -DNAm_s + 1 + oldoffset, 0, 0);
  }//else

  return rlen;
}//breakpoints

/*
int pplus(int i, int m) {
  if (i < m - 1)
    return i + 1;
  return i;
}//pplus
*/

void bpoverlap(int **bp1, int bp1len, int **bp2, int bp2len) {
  int i,
      j;

  for (i = 0; i < bp1len; i++)
    for (j = 0; j < bp2len; j++) {

/*
      if ((bp1[i][0] == bp2[j][0]) ||
          (bp1[i][2] == bp2[j][2]))
        printf("%i %i, 	%i %i: 	", 
          bp1[i][0], bp2[j][0], bp1[i][2], bp2[i][2]);
          */

      if (bp1[i][0] == bp2[j][0]) {
        if (abs(abs(bp1[i][0]) - abs(bp1[i][2])) > 
            abs(abs(bp1[i][0]) - abs(bp2[j][2])))
          printf("%i,%i %i,%i\n", bp1[i][0], bp1[i][1], bp1[i][2], bp1[i][3]);
        else
          printf("%i,%i %i,%i\n", bp1[i][0], bp1[i][1], bp2[j][2], bp2[j][3]);
      }//if
      else {
        if (bp1[i][2] == bp2[j][2]) {
          if (abs(abs(bp1[i][0]) - abs(bp2[j][2])) > 
              abs(abs(bp2[j][0]) - abs(bp2[j][2])))
            printf("%i,%i %i,%i\n", bp1[i][0], bp1[i][1], bp1[i][2], bp1[i][3]);
          else
            printf("%i,%i %i,%i\n", bp2[j][0], bp2[j][1], bp1[i][2], bp1[i][3]);
        }//if
      }//else
    }//for

/*
  while ((i < bp1len - 1) && (j < bp2len - 1)) {
    if (bp1[i][0] < bp2[j][0])
      i = pplus(i, bp1len);
    if (bp1[i][0] > bp2[j][0])
      j = pplus(j, bp1len);
    if ((bp1[i][0] == bp2[j][0]) ||
        (bp1[i][3] == bp2[i][3])){
      printf("%i %i, %i %i\n", bp1[i][0], bp2[j][0], bp1[i][3], bp2[i][3]);
      i = pplus(i, bp1len);
      j = pplus(j, bp1len);
    }//if
  }//while
  */
}//bpoverlap

void reverse(int ***bp, int bplen) {
  int i,
      a[4];

  for (i = 0; i < bplen / 2; i++) {
    a[0] = (*bp)[i][0];
    a[1] = (*bp)[i][1];
    a[2] = (*bp)[i][2];
    a[3] = (*bp)[i][3];
    (*bp)[i][0] = -(*bp)[bplen - i - 1][2];
    (*bp)[i][1] = -(*bp)[bplen - i - 1][3];
    (*bp)[i][2] = -(*bp)[bplen - i - 1][0];
    (*bp)[i][3] = -(*bp)[bplen - i - 1][1];
    (*bp)[bplen - i - 1][0] = -a[2];
    (*bp)[bplen - i - 1][1] = -a[3];
    (*bp)[bplen - i - 1][2] = -a[0];
    (*bp)[bplen - i - 1][3] = -a[1];
  }//for
}//reverse

int main(int argc, char **argv) {
  char *DNAf, // Input string.
       *DNAr,
       *DNAmf,
       *DNAmr;
  int *sufff = NULL, // Truncated suffix array.
      *suffr = NULL,
      DNA_s,
      DNAm_s,
      **bp1,
      **bp2,
      bp1len,
      bp2len,
      i,
      of,
      or,
      pf,
      pr;

  // Read the reference genome.
  fprintf(stderr, "Reading reference genome... ");
  fflush(stderr);
  DNAf = read(argv[1], &DNA_s);
  fprintf(stderr, "done\n");

  // Make the reverse complement strand.
  fprintf(stderr, "Generating reverse complement... ");
  fflush(stderr);
  DNAr = makerevcomp(DNAf, DNA_s);
  fprintf(stderr, "done\n");

  // Make the suffix arrays.
  fprintf(stderr, "Generating forward suffix array... ");
  fflush(stderr);
  sufff = makesuff(DNAf, DNA_s);
  fprintf(stderr, "done\n");
  fprintf(stderr, "Generating reverse suffix array... ");
  fflush(stderr);
  suffr = makesuff(DNAr, DNA_s);
  fprintf(stderr, "done\n");
  fflush(stderr);

  // START.

  fprintf(stderr, "Reading mutated genome... ");
  fflush(stderr);
  DNAmf = read(argv[2], &DNAm_s);
  fprintf(stderr, "done\n");

  fprintf(stderr, "Generating reverse complement... ");
  fflush(stderr);
  DNAmr = makerevcomp(DNAmf, DNAm_s);
  fprintf(stderr, "done\n");

  fprintf(stderr, "Starting analysis...\n");
  fflush(stderr);
  
  for (i = 0; i < DNAm_s; i++) {
    of = binsearch(DNAf, sufff, DNA_s, &pf, &DNAmf[i]);
    printf("%i %i\n", i, sufff[pf]);
  }
  for (i = 0; i < DNAm_s; i++) {
    or = binsearch(DNAr, suffr, DNA_s, &pr, &DNAmf[i]);
    printf("%i %i\n", i, DNA_s - suffr[pr] - 1);
  }

  for (i = 0; i < DNAm_s; i++) {
    of = binsearch(DNAf, sufff, DNA_s, &pf, &DNAmr[i]);
    printf("%i %i\n", DNAm_s - i - 1, sufff[pf]);
  }
  for (i = 0; i < DNAm_s; i++) {
    or = binsearch(DNAr, suffr, DNA_s, &pr, &DNAmr[i]);
    printf("%i %i\n", DNAm_s - i - 1, DNA_s - suffr[pr] - 1);
  }
  //printf("reference: %i;  mutated %i\n", DNA_s, DNAm_s);

/*
  bp1len = breakpoints(&bp1, DNAf, sufff, DNAr, suffr, DNA_s, DNAmf, DNAm_s, 1);
  bp2len = breakpoints(&bp2, DNAf, sufff, DNAr, suffr, DNA_s, DNAmr, DNAm_s, -1);
  reverse(&bp2, bp2len);

  for (i = 0; i < bp1len; i++)
    printf("%i,%i %i,%i ; ", bp1[i][0], bp1[i][1], bp1[i][2], bp1[i][3]);
  printf("\n");

  for (i = 0; i < bp2len; i++)
    printf("%i,%i %i,%i ; ", bp2[i][0], bp2[i][1], bp2[i][2], bp2[i][3]);
  printf("\n");

  bpoverlap(bp1, bp1len, bp2, bp2len);
  */

  fprintf(stderr, "done\n");

/*
  for (i = 0; i < bp2len; i++)
    free(bp2[i]);
  free(bp2);

  for (i = 0; i < bp1len; i++)
    free(bp1[i]);
  free(bp1);
  */


  free(DNAmr);
  free(DNAmf);

  // END.

  free(suffr);
  free(sufff);
  free(DNAr);
  free(DNAf);

  return 0;
}//main
