# Packet_Filtering

## 파일 설명 - packet filter

mybasic.ko: mybasic을 컴파일한 모듈 파일, insmod 명령어로 해당 모듈을 커널에 올릴 수 있다. 

mybasic.c: 모듈 소스 파일. 후크 포인터에 후크 함수들을 register한다. 후크 함수들과 다음의 함수들이 포함되어 있다. 
- custom_write() : proc write operation. 유저 프로세스 공간으로부터 입력 받은 값을 나중에 커널에서 사용할 수 있도록 copy_from_user 함수를 통해 커널로 데이터를 복사한다.
- isStringToNumber(): x.x.x.x 형태의 string으로 된 ip주소를 int 타입으로 바꿔준다. 
- my_module_init(), my_module_exit(): 모듈을 올리고 내릴 때 불리는 함수. 

log.txt: 각 후크 포인터에서 hook으로 캡처한 패킷들을 로그로 만든 파일. main실행 후 ctrl+c 를 통해 main을 중지시키고 터미널에 dmesg를 입력하면 볼 수 있는 내용이다. 

Makefile: KDIR에 커널 소스 경로를 저장하여 mybasic을 컴파일 하는데 쓰인다.

sj : socket pthread에 있는 main 실행 파일과 동일 (ctrl+c를 통해 중지 필요)

god : socket pthread에 있는 main 실행 파일과 비슷 (3초 후에 자동으로 실행 중지)



## 파일 설명 - Socket Pthread

main.c
 클라이언트 소켓을 생성하고, 서버와 연결, 서버에서 보내는 패킷 등을 수신하는 등의 기능을 구현한 c 파일. 수신한 패킷에 대한 시간 정보, 내용, 길이 등을 별도의 파일에 저장할 수 있도록 하였다.  

main:
 main.c 를 컴파일한 실행파일. 해당 파일이 있는 디렉토리에서 ./main 명령어를 통해 실행시킬 수 있다. 패킷을 계속 수신하기 때문에 Ctrl+C를 통해 종료시켜줘야 한다. 실행 후에 포트번호.txt 파일들이 생성되며, 이 파일들에 각 포트로 들어온 패킷에 대한 시간, 내용 등의 정보가 담겨있다. 

1111.txt 포트번호 1111로 들어온 패킷에 대한 정보를 출력한 txt 파일

2222.txt 포트번호 2222로 들어온 패킷에 대한 정보를 출력한 txt 파일

3333.txt 포트번호 3333로 들어온 패킷에 대한 정보를 출력한 txt 파일

4444.txt 포트번호 4444로 들어온 패킷에 대한 정보를 출력한 txt 파일

5555.txt 포트번호 5555로 들어온 패킷에 대한 정보를 출력한 txt 파일