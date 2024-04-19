//
// Created by dmj on 24-4-17.
//

#include "testSFTP.h"
#include <iostream>

int main()
{
    std::string ip="145.192.1.67";
    int port = 22;
    std::string username = "gdu";
    std::string password = "PZwhpzWH6688.";
    SFTPClient mysftp(ip,port,username,password);
    mysftp.initialize();
    mysftp.connect();
    mysftp.authenticate();
    mysftp.mkdir("/home/gdu/test");
    mysftp.uploadFile("/home/gdu/1.cpp","/home/gdu/test/2.cpp");
//    mysftp.deleteFile("/data/test/1.cpp");

    return 1;
}
