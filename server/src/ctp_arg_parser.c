#include "ctp_arg_parser.h"

PCTP_HTTP_SERVER_ARGS programArgs;

const void ctp_parse_args(int argc, char **argv) {
    int opt;
    int option_index = 0;

    if ((programArgs = malloc(sizeof(CTP_HTTP_SERVER_ARGS))) == NULL) {
        fprintf(stderr, "Erro ao alocar memoria, trocar mensagem de erro ctp_arg_parser.c:10");
        exit(-1);
    }

    programArgs->host = CTP_DEFAULT_HTTP_SERVER_HOST;
    programArgs->path = CTP_DEFAULT_HTTP_SERVER_PATH;
    programArgs->port = CTP_DEFAULT_HTTP_SERVER_PORT;

    while ((opt = getopt_long_only(argc, argv,
                              "Hph",    
                              long_options,
                              &option_index)) != -1) {

        switch (opt) {
            case 'H':
                programArgs->host = strdup(optarg);
                break;

            case 'p':
                programArgs->port = strtol(optarg, NULL, 5);
                break;

            case 'h':
                programArgs->path = strdup(optarg);
                break;

            default:
                printf("Option not found!");
                exit(-1);
        }
    }
}
