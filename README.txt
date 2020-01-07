Introduction
-------------

This is an implementation for Linear, SQRT or Path Oblivious RAMs using Intel SGX technology. 


Installation:
-------------

In this document, we assume that you have already installed Intel(R) SGX SDK. if you haven't done that yet, please install it from:

https://01.org


- cd to the directory of SGX SDK folder and run :

  make sdk_install_pkg


This will create a new bin file in linux/installer/bin


Wolfssl OR IPP should be installed for encryption and decryption
between the client and the enclave.



Compiling SGX ORAM
-----------------


- cd to SGXORAM/Linear (or SQRT or PATH)  folder and run 

  ${path_to_SGX_SDK_FOLDER}/sgx_linux_x64_sdk_${version}.bin

sgx_linux_x64_sdk_${version}.bin is the file that you have created before when installing SGX   

- Answer yes for question: Do you want to install in current directory?

- Copy and run the source command to set the environment variables (as indicated)

- Run make (run make clean first if necessary)


Usage
-------

To run the server:

server port number_of_items

Example : server 3333 100000




To run the client:

client server_ip server_port number_of_items number_of_requests

Example : client 127.0.0.1 3333 100000 10000



Encryption & Decryption
-----------------------
Encryption is done using wolfssl or using IPP. you can choose one of them. Each of them has 
2 versions: ONE for the enclave, and ONE for the untrusted code. In our paper, we use IPP.

-To choose one: open the makefile, if you want to use Wolfssl, make sure you have:

TARGET_LIB_CLIENT = $(WOLF_LIB_Client) 
TARGET_LIB_ENCLAVE = $(WOLF_LIB_Enclave) 

Otherwise (IPP):

TARGET_LIB_CLIENT = $(IPP_LIB_Client) 
TARGET_LIB_ENCLAVE = $(IPP_LIB_Enclave) 


- IMPORTANT: make sure that all pathes for the specified library are correct in the makefile. 
For IPP:

IPPinc is the path of the header files (used by untrusted code)
IPPPath is the path of the library to be used (used by untrusted code)
IPP_LIB_Client is the name of the librart to be used (used by untrusted code)
IPP_LIB_Enclave is the name of the library used sgx_tcrypto


- in ext.h, make sure that ENCRYPT_LIB = 3 (wolfssl) or 2 (ipp).

You can also implement your own encryption and use it. For testing, we were uing simple xor. 


Publication
------------
Enclave-Based Oblivious RAM Using Intel's SGX. Maan Haj Rachid, Ryan Riley and Qutaibah Malluhi. Computers & Security, 2020 


Contact
--------

Maan Haj Rachid
maanhajrachid@hotmail.com
