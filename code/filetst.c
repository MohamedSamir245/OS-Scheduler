#include <stdio.h>
int main()
{
    FILE *fp;

    char con[1000];
    fp = fopen("processes.txt", "r");

    if (!fp)
    {
        return 1;
    }
    int id, arTime, runTime, Prio;
    // fgets(con, 1000, fp);
    while (fscanf(fp, "%d %d %d %d", &id, &arTime, &runTime, &Prio) == 4)
    {
        // printf("%s", con);

        printf("%d %d %d %d\n", id, arTime, runTime, Prio);
        // here create a process and add it to the array
    }

    // while (fscanf(fp, "%d", &x))
    // {
    // }
    fclose(fp);

    return 0;
}