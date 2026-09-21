# Log

## 2026-09-03 Kompilera källkod i C

Commit: 6a733a4

För att kunna köra ett program som är skrivet i C behöver texten, koden,
kompileras till maskinkod. För att åstadkomma detta använder jag GCC, GNU Compiler Collection 1
https://gcc.gnu.org/.

```bash
$ gcc main.c -o out
```
*Figur 1: kommandot för att kompilera programmer med GCC*

Allt fungerade mycket bra i WSL som kör Linux (Debian), men inte i Windows, som är måloperativsystemet.
Problemet var att koden kompilerades för att köra på Linux och GCC använder operativsystemets bibliotek vid
kompilering och är inte designad för något annat operativsystem 2.

Källor:
- https://gcc.gnu.org/
- https://clang.llvm.org/docs/CrossCompilation.html


## 2026-09-04 Kompilera källkod i C för att köras på Windows

Commit: 6a733a4

För att kompilera källkoden till binärt format som körs i Windows användes x86_64-w64-mingw32-gcc.
Det är samma utgivare och organisation som underhåller kompilatorn GCC, Free Software Foundation.

```bash
$ x86_64-w64-mingw32-gcc main.c -o out

```
*Figur 2: kommandot för att kompilera programmer med MinGW32-GCC*

mingw32-gcc producerade binären out.exe som gick att exekvera från Windows.


## 2026-09-05 Skapa ett fönster

Commit: 28f2b95

Skapa ett fönser i Windows genom att definiera en *windows class* som är en datastruktur, struct,
och varje fönster i windows måste ha minst en sådan. Datastrukturen definieras enligt dokumenation från
Microsoft, https://learn.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window, och registreras
sedan i Windows.


```shell
$ cl .\main.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30159 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.cpp
Microsoft (R) Incremental Linker Version 14.29.30159.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:main.exe
main.obj
main.obj : error LNK2019: unresolved external symbol __imp_GetMessageW referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_TranslateMessage referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_DispatchMessageW referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_DefWindowProcW referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
main.obj : error LNK2019: unresolved external symbol __imp_PostQuitMessage referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
main.obj : error LNK2019: unresolved external symbol __imp_RegisterClassW referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_CreateWindowExW referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_ShowWindow referenced in function wWinMain
main.obj : error LNK2019: unresolved external symbol __imp_BeginPaint referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
main.obj : error LNK2019: unresolved external symbol __imp_EndPaint referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
main.obj : error LNK2019: unresolved external symbol __imp_FillRect referenced in function "__int64 __cdecl WindowProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" (?WindowProc@@YA_JPEAUHWND__@@I_K_J@Z)
main.exe : fatal error LNK1120: 11 unresolved externals
```
*Figur 3: utskrift från kompilering med saknade bibliotek*

Vid import av *windows.h* fanns inte vissa datatyper definierade i Linux. Istället användes
Microsofts MSVC från Visual Studio 2019. Efter att bytt miljö till Windows fanns biblioteken tillgängliga.

I dokumentationen finns en kodsnutt som är ett "hello world" exempel på att skapa ett fönster. Vid
första försöket att kompilera exemplet som C-kod gav kompilatorn utslag på hur vissa struktar instansieras.
För att lyckas kompilera behövde filnamnet ändras från main.c till main.cpp. Då tolkade MSVC, Microsoft
Visual C++, texten som C++kod. Vid nästa kompilering blev det problem med länkningen, se Figur 2.


## 2026-09-05 Skapa ett fönster (2) - länka bibliotek

Commit: 28f2b95

För att skapa fönstret behövs rätt bibliotek inkluderas vid kompileringstiden. Det görs genom att mata *cl*,
kompilatorprogrammet från MSVC, argument för att lokalisera och inkludera filerna vid kompileringen.

```shell
$ cl .\main.cpp /link user32.lib
Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30159 for x64
Copyright (C) Microsoft Corporation.  All rights reserved.

main.cpp
Microsoft (R) Incremental Linker Version 14.29.30159.0
Copyright (C) Microsoft Corporation.  All rights reserved.

/out:main.exe
user32.lib
main.obj
```
*Figur 4: utskrift från programmet cl*

För att kunna kompilera programmer behövde kompilatorn kunna länka mot User32.lib biblioteket. *CreateWindowEx*
existerar i User32.dll som är en systemfil i Windows och för att kunna kalla på den behöver User32.lib. När
kompilatorn fick rätt länkningsinstruktioner skapades en körbar fil, main.exe, som skapar ett fönster.

Referens: https://learn.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-creation


## 2026-09-07 Allokera byte buffert

Commit: 250081a

Allokera en byte buffert, sekvens av bytes, för att "rita" till.

```c++
static void* wBitmapMemory;

static void wAllocateDIBSection(int width, int height) {
    if (wBitmapMemory) {
        VirtualFree(wBitmapMemory, 0, MEM_RELEASE);
    }
    int BytesPerPixel = 4;
    int BitmapMemorySize = (width * height) * BytesPerPixel;
    wBitmapMemory = VirtualAlloc(NULL, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}
```
*figur 5:* C++ kod för att allokera en byte buffert

Minnet allokeras globalt i programmet för att göras tillgängligt för återallokering samt för framtida rendering.
Vid skapandet av en minnesbuffert används VirtualAlloc. Den funktionen allokerar minne och storleken bestäms
av hur många pixlar som finns i fönstret. För att få plats med all information för vilken färg som varje pixel
ska ritas i behövs 4 bytes allokeras per pixel. 1 vardera för röd, grön och blå samt 1 för alpha, som beskriver
mängden ogenomskinlighet.

Referens:
- https://en.wikipedia.org/wiki/RGBA_color_model
- https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc


## 2026-09-07 Rendera byte bufferten i fönstret

Commit: 573bd18

Uppdatera innehållet i bufferten och rendera i skärmen

```c++
static void wUpdate(HDC hdc, RECT *wRect, int x, int y, int width, int height) {
    int wWidth = wRect->right - wRect->left;
    int wHeight = wRect->bottom - wRect->top;
    StretchDIBits(
        hdc,
        0, 0, wBitmapWidth, wBitmapHeight,
        0, 0, wWidth, wHeight,
        wBitmapMemory,
        &wBitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}
```
*figur 6: metod som kopierar en rektangel av pixlar från minnesbufferten till fönstret*
För att rendera pixlar skärmen behöver man utföra en "bit-block transfer", som innebär att kopiera en rektangel med pixlar från en källa till en annan. När fönstret skapas allokeras i programmet en bytebuffer, se figur 5. Efter allokering loopar vi över alla pixlar och sätter RGB, red, green and blue primary colors, värden för den färgen som pixeln ska ha. En pixel representeras av 4 bytes, en för varje färg (röd, grön och blå), men en extra byte för padding. Efteråt anropas metoden i figur 6 där minnesbufferten kopieras över till enhetskontexten som hålls av HDC variabeln och renderas på skärmen.

Referens:
 - https://en.wikipedia.org/wiki/RGB_color_model


##  2026-09-07 Rita en fyrkant

Commit: 1f43a2f

Rita ett fyrkant i fönstret

```c++
static void cSquare(int y0, int x0, int size) {
    int pitch = wBitmapWidth * bytesPerPixel;

    for (int y = y0; y < y0 + size; ++y) {
        uint8_t* row = (uint8_t*)wBitmapMemory + (pitch * y);
        uint32_t* pixel = (uint32_t*)row + x0;
        for (int x = x0; x < x0 + size; ++x) {
            *pixel = 0x0000FF00;
            pixel++;
        }
        row += pitch;
    }
}
```

Att rita en ruta i en bytearray görs genom att loopa över den delen av bytebufferten som är inom arean angiven av vilken punkt som det börjas rita från x och y punkten och ritar alla pixlar som är inom storleken plus startpunkt i både stående och liggande riktning.
