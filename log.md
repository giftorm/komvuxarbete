# Log

## 2026-09-03 Kompilera källkod i C

Commit: 6a733a4

För att kunna köra ett program som är skrivet i C behöver texten, koden,
kompileras till maskinkod. För att åstadkomma detta använder jag GCC, GNU Compiler Collection
(https://gcc.gnu.org/).

```bash
$ gcc main.c -o out
```

Allt fungerade mycket bra i WSL som kör Linux (Debian), men inte i Windows, som är måloperativsystemet.
Problemet var att koden kompilerades för att köra på Linux och GCC använder operativsystemets bibliotek vid
kompilering och är inte designad för något annat operativsystem, https://clang.llvm.org/docs/CrossCompilation.html.


## 2026-09-04 Kompilera källkod i C för att köras på Windows

Commit: 6a733a4

För att kompilera källkoden till binärt format som körs i Windows användes x86_64-w64-mingw32-gcc.
Det är samma utgivare och organisation som underhåller kompilatorn GCC, Free Software Foundation.

```bash
$ x86_64-w64-mingw32-gcc main.c -o out

```

mingw32-gcc producerade binären out.exe som gick att exekvera från Windows.


## 2026-09-05 Skapa ett fönster

Commit: 28f2b95

Skapa ett fönser i Windows genom att definiera en *windows class* som är en datastruktur, struct,
och varje fönster i windows måste ha minst en sådan. Datastrukturen definieras enligt dokumenation från
Microsoft, https://learn.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window, och registreras
sedan i Windows.


```bash
unresolved external symbol __imp_PostQuitMessage referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
```
*Figur 2*

I dokumentationen finns en kodsnutt som är ett "hello world" exempel på att skapa ett fönster. När jag
försökte kompilera med *cl* och *main.c* som C-kod gav kompilatorn utslag på hur vissa struktar instansieras.
Det löste sig genom att ändra filnamnet på main.c till main.cpp för att *cl* skulle tolka syntaxen som C++ men
stötte på problem med länkningen istället, se figur 2.

LSP, Language Server Protocol, i Linux gav utslag för den inte hittade bibliotek som förväntades så då
satte jag upp en ny miljö i Windows och installerade kompilator med hjälp av Visual Studio Installer 2019
som använder MSVC, Microsoft Visual C++, en kompilator för bland annat C och C++.



## 2026-09-06 Skapa ett fönster (2) - länka bibliotek




Skriv mer om GCC i rapporten:
- https://gcc.gnu.org/ - gcc webbsida
- https://en.wikipedia.org/wiki/GNU_Compiler_Collection - GNU


Skriv mer om C:
- https://en.wikipedia.org/wiki/C_(programming_language) - språket i sig
- https://clang.llvm.org/docs/CrossCompilation.html - komilering på olika platformar
- https://www.fsf.org/ - Free Software Foundation
