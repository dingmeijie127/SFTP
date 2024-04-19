//
// Created by dmj on 24-4-16.
//

#ifndef TESTXML2_SFTPClient_H
#define TESTXML2_SFTPClient_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

class SFTPClient {
public:
    SFTPClient(std::string  ip, int port,std::string  username, std::string& password);
    ~SFTPClient();
    bool initialize();
    bool authenticate();
    bool connect();
    void disconnect();
    bool mkdir(const std::string& remotePath);
    bool uploadFile(const std::string &uploadFilePath, const std::string &remoteFilePath);
    bool downloadFile(const std::string &remoteFilePath, const std::string &saveLocalPath);
    bool deleteFile(const std::string &remoteFilePath);

private:
    // sftp所需要的socket信息
    std::string _ip;
    int _port;
    std::string username_;
    std::string password_;
    int* sock_ = nullptr;
    LIBSSH2_SESSION* session_ = nullptr;
    LIBSSH2_SFTP* sftpSession_ = nullptr;
};


#endif //TESTXML2_SFTPClient_H
