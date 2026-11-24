# Robot Rolniczy (Łazik) — Symulacja 3D w OpenGL

Interaktywna wizualizacja oraz symulacja ruchu sześcio-kołowego łazika w środowisku 3D, napisana w języku C++ z wykorzystaniem nowoczesnego potoku renderowania OpenGL 3.3 Core.

## Technologie
* **Język:** C++ (Visual Studio)
* **Grafika:** OpenGL 3.3 Core, GLSL (Vertex & Fragment Shaders)
* **Biblioteki:** GLEW, GLFW, GLM
* **Modelowanie 3D:** Blender (eksport do formatu Wavefront `.obj`)

## Architektura projektu
* `main.cpp` — inicjalizacja okna, kompilacja shaderów, obsługa wejścia, oświetlenie kierunkowe oraz kamera śledząca pojazd.
* `ObjLoader.hpp` — parser plików `.obj` z automatyczną triangulacją wachlarzową wielokątów.
* `Mesh.hpp` — zarządzanie buforami karty graficznej (`VAO`, `VBO`), wyliczanie lokalnego środka obrotu (`pivot`) oraz transformacje brył.
* `Rover.hpp` — logika kinematyki łazika, automatyczne wykrywanie 6 kół jezdnych, dopasowanie do podłoża oraz płynny skręt przedniej osi.

## Sterowanie
* `W` / `S` — jazda do przodu / do tyłu (z synchronizacją obrotu wszystkich kół)
* `A` / `D` — skręt w lewo / w prawo (z animacją skrętu przednich kół)
* `ESC` — zamknięcie aplikacji
