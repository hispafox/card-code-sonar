package cardcode.tarjeta;

import javacard.framework.ISO7816;
import javacard.framework.ISOException;

/** El saldo de la tarjeta y sus reglas: nunca por debajo de cero ni por encima del máximo. */
public class Saldo {

    static final short MAXIMO = 30000;

    private short valor;

    public short getValor() {
        return valor;
    }

    public void descontar(short importe) {
        if (importe <= 0) {
            ISOException.throwIt(ISO7816.SW_WRONG_DATA);
        }
        if (importe > valor) {
            ISOException.throwIt(ISO7816.SW_CONDITIONS_NOT_SATISFIED);
        }
        valor -= importe;
    }

    // Lo que cabe se calcula con una resta, que no se sale de un short.
    public void cargar(short importe) {
        if (importe <= 0 || importe > (short) (MAXIMO - valor)) {
            ISOException.throwIt(ISO7816.SW_WRONG_DATA);
        }
        valor += importe;
    }
}
