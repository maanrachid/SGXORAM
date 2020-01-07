#Both enclave and client use the same include files
INC= inc
IPPinc = IPPSGX/external/ippcp_internal/inc
IPPPath= /opt/intel/compilers_and_libraries_2018.0.128/linux/ippcp/lib/intel64_and
IPP_LIB_Client = ippcp
IPP_LIB_Enclave = sgx_tcrypto
WOLF_DIR = /home/sgx/wolfssl  
WOLF_CRYPT = /home/sgx/wolfssl/wolfcypt
WOLF_LIB_Enclave_PATH = /home/sgx/wolfssl/IDE/LINUX-SGX
WOLF_LIB_Enclave = wolfssl.sgx.static.lib
WOLF_LIB_Client = wolfssl
WOLF_D = WOLFSSL_SGX
TARGET_LIB_CLIENT = $(IPP_LIB_Client)
TARGET_LIB_ENCLAVE = $(IPP_LIB_Enclave)


client:	server
	g++  -I$(INC) -I$(IPPinc)   -I$(IPPinc)  -I$(WOLF_DIR) -I$(WOLF_CRYPT) -c client.c -o client.o
	g++  -I$(INC) -I$(IPPinc)   -I$(IPPinc)  -I$(WOLF_DIR) -I$(WOLF_CRYPT) -c clientOneByOne.c -o clientOneByOne.o
	g++  client.o -o  client  -L $(IPPPath)   -l$(TARGET_LIB_CLIENT) 
	g++ clientOneByOne.o -o clientOneByOne -L $(IPPPath)  -l$(TARGET_LIB_CLIENT)


server: clean
	sgxsdk/bin/x64/sgx_edger8r --untrusted Enclave.edl --search-path ./ --search-path sgxsdk/include
	g++ -m64 -O2 -fPIC -Wno-attributes  -Isgxsdk/include -I$(WOLF_DIR) -I$(WOLF_CRYPT) -D$(WOLF_D) -DNDEBUG -UEDEBUG -UDEBUG -c  Enclave_u.c -o Enclave_u.o
	g++ -m64 -O2 -fPIC -Wno-attributes  -Isgxsdk/include -I$(WOLF_DIR) -I$(WOLF_CRYPT) -D$(WOLF_D) -DNDEBUG -UEDEBUG -UDEBUG -std=c++11   -c server.c -o server.o
	g++ Enclave_u.o server.o  -o server -m64 -O2 -Lsgxsdk/lib64 -lsgx_urts_sim -lpthread  -lsgx_uae_service_sim  
	sgxsdk/bin/x64/sgx_edger8r --trusted Enclave.edl --search-path ./ --search-path sgxsdk/include
	g++ -m64 -O2 -nostdinc -fvisibility=hidden -fpie -fstack-protector  -Isgxsdk/include -Isgxsdk/include/tlibc -Isgxsdk/include/stlport -c Enclave_t.c -o Enclave_t.o
	g++ -m64 -O2 -nostdinc -fvisibility=hidden -fpie -fstack-protector -I$(INC) -I$(IPPinc)  -IInclude -IEnclave -Isgxsdk/include -Isgxsdk/include/tlibc -Isgxsdk/include/stlport -I$(WOLF_DIR) -I$(WOLF_CRYPT) -D$(WOLF_D) -std=c++03 -nostdinc++ -c Enclave.cpp -o Enclave.o
	g++ Enclave_t.o Enclave.o -o enclave.so -m64 -O2 -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -Lsgxsdk/lib64 -Wl,--whole-archive -lsgx_trts_sim  -Wl,--no-whole-archive -Wl,--start-group -lsgx_tstdc -lsgx_tstdcxx -lsgx_tcrypto -lsgx_tservice_sim -L$(WOLF_LIB_Enclave_PATH) -l$(TARGET_LIB_ENCLAVE)  -Wl,--end-group -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined -Wl,-pie,-eenclave_entry -Wl,--export-dynamic -Wl,--defsym,__ImageBase=0 -Wl,--version-script=Enclave.lds
	sgxsdk/bin/x64/sgx_sign sign -key Enclave_private.pem -enclave enclave.so -out enclave.signed.so -config Enclave.config.xml

clean:
	rm -f *~ *# client clientOneByOne middle server *.o Enclave_t* Enclave_u* enclave.so enclave.signed.so 
