#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_BLOCK_SIZE 4096

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [-b block_size] output\n", prog);
    fprintf(stderr, "       %s [-b block_size] input_file output_file\n", prog);
    exit(EXIT_FAILURE);
}

static int is_zero_block(const unsigned char *buf, ssize_t n) {
    for (ssize_t i = 0; i < n; i++) {
        if (buf[i] != 0)
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int block_size = DEFAULT_BLOCK_SIZE;
    int opt;

    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
            case 'b':
                block_size = atoi(optarg);
                if (block_size <= 0) {
                    fprintf(stderr, "Invalid block size: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                usage(argv[0]);
        }
    }

    int remaining = argc - optind;
    if (remaining != 1 && remaining != 2)
        usage(argv[0]);

    int in_fd = STDIN_FILENO;
    const char *out_name;

    if (remaining == 2) {
        in_fd = open(argv[optind], O_RDONLY);
        if (in_fd < 0)
            die("open input");
        out_name = argv[optind + 1];
    } else {
        out_name = argv[optind];
    }

    int out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd < 0)
        die("open output");

    unsigned char *buf = malloc(block_size);
    if (!buf)
        die("malloc");

    off_t pos = 0;
    ssize_t r;

    while ((r = read(in_fd, buf, block_size)) > 0) {
        if (is_zero_block(buf, r)) {
            if (lseek(out_fd, r, SEEK_CUR) == -1)
                die("lseek");
        } else {
            if (write(out_fd, buf, r) != r)
                die("write");
        }
        pos += r;
    }

    if (r < 0)
        die("read");

    if (ftruncate(out_fd, pos) < 0)
        die("ftruncate");

    free(buf);
    close(out_fd);
    if (in_fd != STDIN_FILENO)
        close(in_fd);

    return 0;
}
