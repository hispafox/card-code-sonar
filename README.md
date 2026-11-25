# card-code-sonar

El código del repositorio de ejemplo del curso, en una sola versión y en una sola rama, para analizarlo con SonarQube. El repositorio de ejemplo (`card-code`) separa cada submódulo en su rama, con un antes y un después; aquí está el código tal como queda al final, más el módulo heredado, para que un análisis lo vea entero.

## Qué hay

| Carpeta | Qué es | De dónde sale |
|---|---|---|
| `c/src`, `c/include` | La parte en C: el procesado de comandos, el PIN con su contador, la memoria persistente simulada, el registro, el saldo, el CRC y las contramedidas del módulo `seguridad` | El después del submódulo 3.5 |
| `c/src/liquidacion.c` | El módulo heredado, sin refactorizar | El antes del submódulo 5.2 |
| `c/src/extracto.c` | El extracto de los movimientos, ya refactorizado | El después del submódulo 5.1 |
| `c/pruebas` | Las pruebas de C, con Unity (`terceros/unity`) | Las mismas ramas |
| `javacard/src` | El applet de Java Card | El después del submódulo 4.5 |
| `javacard/pruebas` | Las pruebas del applet, en Groovy con Spock, sobre jcardsim (`lib/`) | El después del submódulo 5.3 |

El código redundante a propósito del submódulo 3.5 (la comprobación doble del PIN, la comparación en tiempo constante, la barrera del optimizador y el borrado seguro) está tal cual, con sus comentarios `CONTRAMEDIDA`.

## El análisis

`.sonarcloud.properties` dice qué carpetas son código y cuáles son pruebas. El código C del módulo heredado (`liquidacion.c`) se compila sin `-Wconversion`, como en el repositorio de ejemplo.
