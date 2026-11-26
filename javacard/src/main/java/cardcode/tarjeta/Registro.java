package cardcode.tarjeta;

import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.Util;

/**
 * Las operaciones que el terminal todavía no ha recogido, en un array: cada
 * una ocupa tres bytes, el tipo y el importe.
 */
public class Registro {

    static final byte PAGO = 1;
    static final byte RECARGA = 2;

    private static final short CAPACIDAD = 8;
    private static final short TAMANO_ENTRADA = 3;

    private final byte[] entradas = new byte[CAPACIDAD * TAMANO_ENTRADA];
    private short ocupadas;

    public void anotar(byte tipo, short importe) {
        if (ocupadas == CAPACIDAD) {
            ISOException.throwIt(ISO7816.SW_FILE_FULL);
        }
        short posicion = (short) (ocupadas * TAMANO_ENTRADA);
        entradas[posicion] = tipo;
        Util.setShort(entradas, (short) (posicion + 1), importe);
        ocupadas++;
    }

    public short copiar(byte[] destino, short desde) {
        short longitud = (short) (ocupadas * TAMANO_ENTRADA);
        Util.arrayCopyNonAtomic(entradas, (short) 0, destino, desde, longitud);
        return longitud;
    }

    public void vaciar() {
        ocupadas = 0;
    }
}
