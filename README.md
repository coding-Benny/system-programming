# system-programming
2020-2 System Programming 과제물입니다.
## 🏫 학생 레코드 관리 프로그램 재작성
강의자료에 있는 학생레코드관리 프로그램을 리눅스의 시스템 콜을 이용한 프로그램으로 수정하기
<details>
  <summary>실행 결과</summary>
  <ol>
    <li>초기화면</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/init.png" width="400" height="300">
    <li>학생 레코드 생성</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/create.png" width="400" height="300">
    <li>모든 학생 레코드 출력</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/display.png" width="400" height="300">
    <li>학번이 1871408인 학생의 레코드 검색</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/search.png" width="400" height="200">
    <li>학번이 1810241인 학생의 레코드 변경</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/change.png" width="400" height="400">
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/updated.png" width="400" height="250">
    <li>학번이 1810242인 학생의 레코드 삭제</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/delete.png" width="400" height="200">
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/deleted.png" width="400" height="300">
    <li>종료</li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/StudentRecordsManagement/exit.png" width="400" height="300">
  </ol>
</details>

## 💲 myls
ls 명령을 수행하여 동일한 결과(내용)가 나오는 myls 프로그램을 작성하기(출력 형식 무관)<br>
- $ `myls [option] [file]`<br>
  option: `-l` `-a` `-al` <br>
  file : file or directory name<br>
- 예시<br>
  $ `myls`<br>
  $ `myls -l`<br>
  $ `myls -al  /etc/passwd`<br>
  $ `myls  a.out`<br>
  $ `myls -a  ..`<br>
<details>
  <summary>실행 결과</summary>
  <ol>
    <li><code>myls</code></li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/myls/ls.png" width="400" height="200">
    <li><code>myls -l</code></li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/myls/ls-l.png" width="400" height="300">
    <li><code>myls -al /etc/passwd</code></li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/myls/ls-al-file.png" width="400" height="80">
    <li><code>myls a.out</code></li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/myls/ls-file.png" width="400" height="80">
    <li><code>myls -a ..</code></li>
    <img src="https://github.com/coding-Benny/system-programming/blob/main/images/myls/ls-a-dir.png" width="400" height="250">
  </ol>
</details>
