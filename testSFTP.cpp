//
// Created by dmj on 24-4-16.
//

#include "testSFTP.h"
#include <utility>

SFTPClient::SFTPClient(std::string ip, int port, std::string username, std::string &password)
    :_ip(std::move(ip)), _port(port),username_(std::move(username)), password_(password),sock_(new int(-1)){
    libssh2_init(0);
}

SFTPClient::~SFTPClient() {
    // 断联
    disconnect();
    if(sock_)
    {
        delete sock_;
        sock_ = nullptr;
    }
}

bool SFTPClient::initialize() {
    *sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock_ == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }
    std::cout<<"create socket success"<<std::endl;
    return true;
}

bool SFTPClient::authenticate() {
    if (libssh2_userauth_password(session_, username_.c_str(), password_.c_str()) != 0) {
        std::cerr << "Authentication by password failed." << std::endl;
        return false;
    }
    sftpSession_ = libssh2_sftp_init(session_);
    if(!sftpSession_)
    {
        std::cerr<<"unable to initialize SFTP session."<<std::endl;
        return false;
    }
    std::cout<<"Authentication by password successfull. "<<std::endl;
    return true;
}

void SFTPClient::disconnect() {
    if(sftpSession_)
    {
        libssh2_sftp_shutdown(sftpSession_);
    }
    if(session_)
    {
        libssh2_session_disconnect(session_, "Normal Shutdown");
        libssh2_session_free(session_);
        session_ = nullptr;
    }
    if(sock_ && *sock_ != -1)
    {
        close(*sock_);
        *sock_ = -1;
    }
}

bool SFTPClient::mkdir(const std::string &remotePath) {
    if(sftpSession_ == nullptr)
    {
        std::cerr<<" mkdir sftpSession_ == nullptr."<<std::endl;
        return false;
    }
    int rc = libssh2_sftp_mkdir(sftpSession_, remotePath.c_str(), LIBSSH2_SFTP_S_IRWXU);
    if(rc != 0)
    {
        return false;
    }
    std::cout << "Directory created successfully.\n";

    return true;
}

bool SFTPClient::uploadFile(const std::string &uploadFilePath, const std::string &remoteFilePath) {
    LIBSSH2_SFTP_HANDLE* handle = libssh2_sftp_open(sftpSession_, remoteFilePath.c_str(),  LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                                                    LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR| LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
    if(handle == nullptr)
    {
        std::cerr<<"uploadFile Unable to open remote file with SFTP server:"<< remoteFilePath<<std::endl;
        return false;
    }
    std::ifstream file(uploadFilePath,std::ios::binary);
    if(!file.is_open())
    {
        std::cerr<<"failed to open local file: "<<uploadFilePath<<std::endl;
        libssh2_sftp_close(handle);
        return false;
    }
    std::vector<char> buffer (std::istreambuf_iterator<char>(file), {});
    size_t idx =0;
    const size_t chunk_size = 2048;
    while(idx < buffer.size())
    {
        size_t size_to_write = std::min(chunk_size, buffer.size() -idx);
        ssize_t written = libssh2_sftp_write(handle, buffer.data()+idx, size_to_write);
        if(written != size_to_write)
        {
            std::cerr<<"Failed to upload part of the file."<<std::endl;
            file.close();
            libssh2_sftp_close(handle);
            return false;
        }
        idx+=size_to_write;
    }
    file.close();
    libssh2_sftp_close(handle);
    std::cout << "write file successfully.\n";
    return true;
}

bool SFTPClient::deleteFile(const std::string &remoteFilePath) {
    if (libssh2_sftp_unlink(sftpSession_, remoteFilePath.c_str()) != 0) {
        std::cerr << "Failed to delete file on SFTP server: " << remoteFilePath << std::endl;
        return false;
    }
    return true;
}

bool SFTPClient::downloadFile(const std::string &remoteFilePath, const std::string &saveLocalPath) {
    LIBSSH2_SFTP_HANDLE * handle = libssh2_sftp_open(sftpSession_, remoteFilePath.c_str(),LIBSSH2_FXF_READ,0);
    if(handle == nullptr)
    {
        std::cerr<<"downloadFile unable to open file with local, local path: "<<remoteFilePath<<std::endl;
        return false;
    }
    std::ofstream  file(saveLocalPath, std::ios::binary);
    if(!file.is_open())
    {
        std::cerr<<"failed to open local file for writing: "<<saveLocalPath<<std::endl;
        libssh2_sftp_close(handle);
        return false;
    }
    char buffer[2048];
    ssize_t byteRead = 0;
    bool writeError = false;
    while((byteRead = libssh2_sftp_read(handle, buffer, sizeof(buffer)))>0)
    {
        file.write(buffer, byteRead);
        if(!file)
        {
            std::cerr<<"file size != buffer.size(). "<<std::endl;
            file.close();
            libssh2_sftp_close(handle);
            writeError = true;
            return false;
        }
    }
    file.close();
    libssh2_sftp_close(handle);
    return true;
}

bool SFTPClient::connect() {
    struct sockaddr_in sin;
    memset(&sin, 0,sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(_port);
    sin.sin_addr.s_addr = inet_addr(_ip.c_str());
    if (::connect(*sock_, (struct sockaddr*)(&sin), sizeof(sin)) != 0) {
        std::cerr << "Failed to connect." << std::endl;
        return false;
    }
    // ssh的初始化
    session_ = libssh2_session_init();
    if(session_ == nullptr)
    {
        std::cerr<<"uploadFile session_ == nullptr"<<std::endl;
        return false;
    }
    // 设置为阻塞模式
    libssh2_session_set_blocking(session_, 1);
    return libssh2_session_handshake(session_, *sock_) == 0;
}


