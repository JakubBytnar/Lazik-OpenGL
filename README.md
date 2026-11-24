\# Robot Rolniczy (Łazik) — Symulacja 3D w OpenGL



Interaktywna wizualizacja oraz symulacja ruchu sześcio-kołowego łazika w środowisku 3D, napisana w języku C++ z wykorzystaniem nowoczesnego potoku renderowania OpenGL 3.3 Core\[cite: 17, 20].



\## Technologie

\* \*\*Język:\*\* C++ (Visual Studio)

\* \*\*Grafika:\*\* OpenGL 3.3 Core, GLSL (Vertex \& Fragment Shaders)\[cite: 17]

\* \*\*Biblioteki:\*\* GLEW, GLFW, GLM\[cite: 17]

\* \*\*Modelowanie 3D:\*\* Blender (eksport do formatu Wavefront `.obj`)\[cite: 17]



\## Architektura projektu

\* `main.cpp` — inicjalizacja okna, kompilacja shaderów, obsługa wejścia, oświetlenie kierunkowe oraz kamera śledząca pojazd\[cite: 17].

\* `ObjLoader.hpp` — parser plików `.obj` z automatyczną triangulacją wachlarzową wielokątów\[cite: 19].

\* `Mesh.hpp` — zarządzanie buforami karty graficznej (`VAO`, `VBO`), wyliczanie lokalnego środka obrotu (`pivot`) oraz transformacje brył\[cite: 18].

\* `Rover.hpp` — logika kinematyki łazika, automatyczne wykrywanie 6 kół jezdnych, dopasowanie do podłoża oraz płynny skręt przedniej osi.



\## Sterowanie

\* `W` / `S` — jazda do przodu / do tyłu (z synchronizacją obrotu wszystkich kół)\[cite: 17, 20]

\* `A` / `D` — skręt w lewo / w prawo (z animacją skrętu przednich kół)\[cite: 17, 20]

\* `ESC` — zamknięcie aplikacji\[cite: 17]

