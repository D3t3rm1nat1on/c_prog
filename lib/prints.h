
void* print_args(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        fprintf(stderr, "argv[%d] = [%s]\n", i, argv[i]);
    }
}

void* print_id()
{
    fprintf(stderr, "My PPID = %d\n", getppid());
    fprintf(stderr, "My PID = %d\n", getpid());
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    fprintf(stderr, "My TID = %d\n", gettid());
#pragma GCC diagnostic pop
    fprintf(stderr, "My GID = %d\n", getgid());
}

