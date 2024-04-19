#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int main() {
    const char* hostname = "192.168.146.135";
    const char* username = "sftpuser";
    const char* password = "888";
    const int port = 22;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(hostname);
    if (connect(sock, (struct sockaddr*)(&sin), sizeof(sin)) != 0) {
        std::cerr << "Failed to connect: " << strerror(errno) << std::endl;
        return -1;
    }

    LIBSSH2_SESSION* session = libssh2_session_init();
    if (libssh2_session_handshake(session, sock)) {
        std::cerr << "Failed to establish SSH session: " << strerror(errno) << std::endl;
        return -1;
    }

    if (libssh2_userauth_password(session, username, password)) {
        std::cerr << "Authentication by password failed.\n";
        return -1;
    }

    LIBSSH2_SFTP* sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        std::cerr << "Failed to initialize SFTP session.\n";
        return -1;
    }

    int rc = libssh2_sftp_mkdir(sftp_session, "/data/new_folder", LIBSSH2_SFTP_S_IRWXU);
    if (rc) {
        std::cerr << "Failed to create directory. Error: " << rc << "\n";
        return -1;
    }

    libssh2_sftp_shutdown(sftp_session);
    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    close(sock);

    std::cout << "Directory created successfully.\n";

    return 0;
}
