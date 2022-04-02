
# IPP małe zadanie

Decyzje co do błędów (świadome):

- Bardzo duże testy zużywają mało pamięci, więc program próbuje liczyć wynik nawet w przypadku gdzie niepraktycznie długo zajmie go rzeczywiście znalezienie.

- W przykładzie

      1
  
  błąd to `ERROR 1` (nie `ERROR 2`), ponieważ znak nowej linii, który powinien się pojawić na wejściu, jest częścią pierwszej linijki.

- Jeśli błędem jest, że początek/koniec szukanej ścieżki jest na zajętym polu, to błędem jest numer linii z tym początkiem/końcem.

