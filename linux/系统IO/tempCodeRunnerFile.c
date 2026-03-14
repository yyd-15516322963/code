  char *p;
    p = strtok(time, "-");
    printf("%s\n", p);
    while((p = strtok(NULL,"-")) != NULL)
    {
        printf("%s\n", p);
    }