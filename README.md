# CUI 기반 DLL 인젝터
**내부 로직 설명은 [여기](https://jeongje.vercel.app/blog/post-18)** <br /><br />
![cmd_result](https://github.com/user-attachments/assets/22ed542e-a238-45bb-b7be-9b4711e3c417)

# GUI 기반 DLL 인젝터
**내부 로직 설명은 [여기](https://jeongje.vercel.app/blog/post-19)** <br /><br />
![waiting](https://github.com/user-attachments/assets/b2e004ac-4a4e-49d4-a7aa-39b53dafb518)
![success](https://github.com/user-attachments/assets/b23ff1fa-d417-4991-a661-cdd595a1c0c7)

# API 후킹을 위한 테스트 DLL
자세한 설명은 [여기](https://jeongje.vercel.app/blog/post-20) <br />
[MinHook](https://github.com/TsudaKageyu/minhook)을 이용하여 채팅 기능에 흔히 사용되는 **ws2_32.dll**의 `WSASend`함수를 후킹하여 WSASend 호출시 `OutputDebugString`을 이용하여 **DebugVie**w에 로그를 남기는 역할을 함
