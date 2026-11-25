import com.licel.jcardsim.base.SimulatorRuntime

import java.lang.reflect.Array
import java.lang.reflect.Field
import java.lang.reflect.Modifier

/**
 * jcardsim lleva la cuenta de si hay una transacción abierta, pero al
 * abortarla no deshace nada. Este simulador hace lo que dice la
 * especificación del entorno de ejecución (JCRE, capítulo 7): al empezar una
 * transacción guarda los campos y los arrays persistentes de los objetos del
 * applet; si la transacción se aborta, o si el comando termina con ella
 * abierta, los deja como estaban. Los arrays transitorios no se guardan,
 * porque no participan en las transacciones.
 *
 * Es una herramienta de las pruebas, no del applet: en la tarjeta lo hace el
 * entorno de ejecución.
 */
class SimuladorConTransacciones extends SimulatorRuntime {

    private Map<Object, Object> copia

    @Override
    void beginTransaction() {
        super.beginTransaction()
        copia = new IdentityHashMap<>()
        guardar(getApplet(getAID()))
    }

    @Override
    void commitTransaction() {
        super.commitTransaction()
        copia = null
    }

    @Override
    void abortTransaction() {
        super.abortTransaction()
        restaurar()
    }

    // Si el comando termina con la transacción abierta, el entorno la aborta
    // (JCRE 7.6.2); si terminó sin excepción, responde 6F00 (JCRE 3.3).
    @Override
    byte[] transmitCommand(byte[] comando) {
        byte[] respuesta = super.transmitCommand(comando)
        if (getTransactionDepth() != 0) {
            abortTransaction()
            int n = respuesta.length
            if (respuesta[n - 2] == (byte) 0x90 && respuesta[n - 1] == (byte) 0x00) {
                respuesta = [0x6F, 0x00] as byte[]
            }
        }
        respuesta
    }

    private void guardar(Object objeto) {
        if (objeto == null || copia.containsKey(objeto)) {
            return
        }
        Class clase = objeto.getClass()
        if (clase.isArray()) {
            if (clase.componentType.isPrimitive() && transientMemory.isTransient(objeto) == 0) {
                copia.put(objeto, clonar(objeto))
            }
            return
        }
        if (!clase.name.startsWith('cardcode.')) {
            return
        }
        Map<Field, Object> campos = [:]
        copia.put(objeto, campos)
        for (Class c = clase; c.name.startsWith('cardcode.'); c = c.superclass) {
            for (Field campo in c.declaredFields) {
                if (Modifier.isStatic(campo.modifiers)) {
                    continue
                }
                campo.accessible = true
                Object valor = campo.get(objeto)
                if (campo.type.isPrimitive()) {
                    campos.put(campo, valor)
                } else {
                    guardar(valor)
                }
            }
        }
    }

    private void restaurar() {
        copia?.each { objeto, guardado ->
            if (objeto.getClass().isArray()) {
                System.arraycopy(guardado, 0, objeto, 0, Array.getLength(objeto))
            } else {
                guardado.each { Field campo, valor ->
                    if (!Modifier.isFinal(campo.modifiers)) {
                        campo.set(objeto, valor)
                    }
                }
            }
        }
        copia = null
    }

    private static Object clonar(Object array) {
        int longitud = Array.getLength(array)
        Object copiado = Array.newInstance(array.getClass().componentType, longitud)
        System.arraycopy(array, 0, copiado, 0, longitud)
        copiado
    }
}
