int init_socket(int *sock_fd) {

    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &info) != 0)
        return -1;

    *sock_fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (*sock_fd == -1)
        return -1;
    if (bind(*sock_fd, info->ai_addr, info->ai_addrlen) == -1)
        return -1;
    if (listen(*sock_fd, BACKLOG) == -1)
        return -1;
    
    return 0;
}
