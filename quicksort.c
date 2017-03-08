#include <stdio.h>
#include <stdlib.h>

void quicksort(int *a, int n) {
  if (n == 0)
    return;

  int i, c, r = n-1, len;
  int *temp = (int*)malloc(n * sizeof(int));

  for (i = 1, c = 0; i < n; i++) {
    if (a[i] < a[0])
      temp[c++] = a[i];
    else if (a[i] >= a[0])
      temp[r--] = a[i];
  }

  len = c;
  temp[c++] = a[0];

  for (i = 0; i < n; i++)
    a[i] = temp[i];

  quicksort(a, len);
  quicksort(a + len + 1, n - len - 1);
}

int main() {
  /*  Markers used to bound trace regions of interest */
  volatile char MARKER_START, MARKER_END;
  FILE* marker_fp = fopen("quicksort.marker", "w");
  if (marker_fp == NULL) {
    perror("Could not write to file quicksort.marker");
    exit(1);
  }
  fprintf(marker_fp, "%p %p", &MARKER_START, &MARKER_END );
  fclose(marker_fp);

  MARKER_START = 33;
  srand(0);
  int i, k;
  int *nums = (int*)malloc((1000) * sizeof(int));

  for (i = 0; i < 1000; i++)
    nums[i] = rand();

  quicksort(nums, 1000);

  for (k = 0; k < 1000; k++)
    printf("%d ", nums[k]);
  printf("\n");
  MARKER_END = 34;

  return 0;
}
