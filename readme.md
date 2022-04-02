
# IPP małe zadanie

### Testowanie

Jest `test.sh` spełniający warunki z zadania, ale tak naprawdę jest dużo fajniejszy `test.py` (`test.py --help` żeby zobaczyć jak bardzo fajny), którego pisanie w bashu by było marnowaniem czasu.

### Budowanie

Jest sobie `makefile`. Domyślnie buduje w trybie takim release jakby, z flagami jak w treści zadania. Można dać `make -B DEBUG=yes` żeby się kompilowało w trybie żeby działał valgrind i debugowanie. Potem trzeba `make -B` żeby wrócić do kompilowania bez debugu.

### Decyzje co do błędów (świadome)

- Bardzo duże testy zużywają mało pamięci, więc program próbuje liczyć wynik nawet w przypadku gdzie niepraktycznie długo zajmie go rzeczywiście obliczenie.

- W przykładzie

      1
  
  błąd to `ERROR 1` (nie `ERROR 2`), ponieważ znak nowej linii, który powinien się pojawić na wejściu, jest częścią pierwszej linijki.

- Jeśli błędem jest, że początek/koniec szukanej ścieżki jest na zajętym polu, to błędem jest numer linii z tym początkiem/końcem.
