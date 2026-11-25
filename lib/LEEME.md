# lib

Las librerías de Java que usan los scripts, dentro del repositorio para que nada se descargue al compilar ni al probar. Descargadas de Maven Central el 14-11-2026, con el SHA-1 de cada fichero comprobado contra el que publica Maven Central.

| Fichero | Para qué | Licencia |
|---|---|---|
| `jcardsim-3.0.6.0.jar` (`com.klinec`) | Simulador de Java Card para probar el applet sin tarjeta. Trae dentro la API de Java Card 3.0.5, contra la que se compila el applet | Apache 2.0 |
| `asm-7.2.jar`, `asm-commons-7.2.jar`, `asm-tree-7.2.jar`, `asm-util-7.2.jar`, `asm-analysis-7.2.jar` | Dependencias de jcardsim | BSD de 3 cláusulas (OW2) |
| `groovy-4.0.32.jar` | Groovy, sin instalarlo | Apache 2.0 |
| `spock-core-2.5-groovy-4.0.jar` | Spock, para las pruebas en Groovy | Apache 2.0 |
| `hamcrest-3.0.jar`, `geantyref-1.3.16.jar` | Dependencias de Spock | BSD de 3 cláusulas · Apache 2.0 |
| `junit-platform-console-standalone-1.14.4.jar` | Lanzador de las pruebas de Spock desde PowerShell | EPL 2.0 |

`licencias/` guarda el texto de las licencias que traen dentro los propios `.jar`. Unity, el framework de pruebas de C, está en `terceros/unity/` (v2.7.0, licencia MIT, de ThrowTheSwitch).
