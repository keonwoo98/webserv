# **webserv**

> Contributor : keokim, minjkim2, junyopar, dkim2, jihoh, seungle

### **Contents**
------------
> [Subject](#subject)

> [Functions](#Functions)

> [Commit-Rule](#commit-message-rule)
### **Subject**
---
* A HTTP server in C++ 98
* This project is about writing your ow HTTP server.
* You will be able to test it with an actual browser.
* HTTP is one of the most used protocols on the internet.
* Knowing its arcane will be useful, even if you won’t be working on a website.

### **Functions**
---
* Everything in C++ 98
* `htons`, `htonl`, `ntohs`, `ntohl`, `select`, `poll`, `epoll`(`epoll_create`, `epoll_ctl`, `epoll_wait`), `kqueue`(`kqueue`, `kevent`), `socket`, `accept`, `listen`, `send`, `recv`, `bind`, `connect`, `inet_addr`, `setsockopt`, `getsockname`, `fcntl`

### **Commit Message Rule**
---
* `Type : 내용` 형식으로 작성할 것.
* Add : 파일, 폴더 추가
* Feat : 새로운 기능 추가
* Remove : 필요 없는 코드, 기능 제거
* Refactor : 변수, 함수 이름 수정, 코드 최적화
* Docs : 문서 파일 수정
* Fix : Norm, bug 수정
* etc : 기타 업무
