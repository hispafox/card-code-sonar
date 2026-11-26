package cardcode.tarjeta;

import javacard.framework.APDU;
import javacard.framework.Applet;
import javacard.framework.ISO7816;
import javacard.framework.ISOException;
import javacard.framework.JCSystem;
import javacard.framework.Util;

/**
 * La tarjeta: el saldo, la recarga, el pago y el PIN, y el registro de las
 * operaciones que el terminal todavía no ha recogido.
 */
public class TarjetaApplet extends Applet {

    // Las instrucciones que entiende la tarjeta.
    static final byte INS_VERIFICAR_PIN = (byte) 0x20;
    static final byte INS_CAMBIAR_PIN = (byte) 0x24;
    static final byte INS_PAGAR = (byte) 0x30;
    static final byte INS_RECARGAR = (byte) 0x32;
    static final byte INS_CONSULTAR = (byte) 0x34;
    static final byte INS_VACIAR_REGISTRO = (byte) 0x36;
    static final byte INS_PERSONALIZAR = (byte) 0x50;
    static final byte INS_LEER_REGISTRO = (byte) 0xB2;

    // ISO/IEC 7816-4: «método de autenticación bloqueado». La API 3.2 lo trae
    // como ISO7816.SW_AUTHENTICATION_METHOD_BLOCKED; la 3.0.5, contra la que
    // se compila el applet, no.
    static final short SW_PIN_BLOQUEADO = (short) 0x6983;
    // ISO/IEC 7816-4: «verificación fallida», 63CX, con X los intentos que quedan.
    static final short SW_VERIFICACION_FALLIDA = (short) 0x63C0;

    static final byte PIN_LONGITUD_MINIMA = 4;
    static final byte PIN_LONGITUD_MAXIMA = 8;
    static final byte PIN_INTENTOS = 3;
    static final byte PIN_RELLENO = (byte) 0xFF;
    static final short LONGITUD_CAMBIO_PIN = 2 * PIN_LONGITUD_MAXIMA;
    static final short LONGITUD_IMPORTE = 2;
    static final short LONGITUD_SALDO = 2;

    private final Saldo saldo = new Saldo();
    private final Registro registro = new Registro();
    // Todo lo que el applet necesita se crea aquí, al instalarlo.
    private final byte[] pin = new byte[PIN_LONGITUD_MAXIMA];
    private boolean personalizada;
    private byte intentos;
    // Lo que solo vale mientras el applet está seleccionado va en memoria
    // transitoria, que se borra al deseleccionarlo y al reiniciar la tarjeta.
    private final byte[] recibido = JCSystem.makeTransientByteArray(PIN_LONGITUD_MAXIMA, JCSystem.CLEAR_ON_DESELECT);
    private final boolean[] pinVerificado = JCSystem.makeTransientBooleanArray((short) 1, JCSystem.CLEAR_ON_DESELECT);

    public static void install(byte[] parametros, short desplazamiento, byte longitud) {
        new TarjetaApplet().register();
    }

    // El despacho: mira la instrucción y llama al método de ese comando.
    public void process(APDU apdu) {
        if (selectingApplet()) {
            return;
        }
        byte[] bufer = apdu.getBuffer();

        switch (bufer[ISO7816.OFFSET_INS]) {
        case INS_PERSONALIZAR:    personalizar(apdu);  return;
        case INS_VERIFICAR_PIN:   verificarPin(apdu);  return;
        case INS_CAMBIAR_PIN:     cambiarPin(apdu);    return;
        case INS_PAGAR:           pagar(apdu);         return;
        case INS_RECARGAR:        recargar(apdu);      return;
        case INS_CONSULTAR:       consultar(apdu);     return;
        case INS_LEER_REGISTRO:   leerRegistro(apdu);  return;
        case INS_VACIAR_REGISTRO: vaciarRegistro();    return;
        default:
            ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
        }
    }

    // Los comandos, uno por método.

    private void personalizar(APDU apdu) {
        if (personalizada) {
            ISOException.throwIt(ISO7816.SW_CONDITIONS_NOT_SATISFIED);
        }
        recibirPin(apdu);
        Util.arrayCopy(recibido, (short) 0, pin, (short) 0, PIN_LONGITUD_MAXIMA);
        intentos = PIN_INTENTOS;
        personalizada = true;
    }

    private void verificarPin(APDU apdu) {
        recibirPin(apdu);
        comprobarPin();
        pinVerificado[0] = true;
    }

    // Los datos: el PIN actual y el nuevo, rellenos hasta ocho bytes cada uno.
    // Sin transacción: el intento se gasta fuera de ella, y el PIN nuevo se
    // escribe con Util.arrayCopy, que ya es atómico.
    private void cambiarPin(APDU apdu) {
        byte[] bufer = apdu.getBuffer();
        recibir(apdu, LONGITUD_CAMBIO_PIN);
        Util.arrayCopyNonAtomic(bufer, ISO7816.OFFSET_CDATA, recibido, (short) 0, PIN_LONGITUD_MAXIMA);
        comprobarPin();
        Util.arrayCopy(bufer, (short) (ISO7816.OFFSET_CDATA + PIN_LONGITUD_MAXIMA), pin, (short) 0, PIN_LONGITUD_MAXIMA);
    }

    // El saldo y su anotación en el registro van juntos o no va ninguno: si
    // algo falla antes de cerrar la transacción, el entorno la deshace.
    private void pagar(APDU apdu) {
        exigirPinVerificado();
        short importe = recibirImporte(apdu);
        JCSystem.beginTransaction();
        saldo.descontar(importe);
        registro.anotar(Registro.PAGO, importe);
        JCSystem.commitTransaction();
    }

    private void recargar(APDU apdu) {
        exigirPinVerificado();
        short importe = recibirImporte(apdu);
        JCSystem.beginTransaction();
        saldo.cargar(importe);
        registro.anotar(Registro.RECARGA, importe);
        JCSystem.commitTransaction();
    }

    private void consultar(APDU apdu) {
        Util.setShort(apdu.getBuffer(), (short) 0, saldo.getValor());
        apdu.setOutgoingAndSend((short) 0, LONGITUD_SALDO);
    }

    private void leerRegistro(APDU apdu) {
        exigirPinVerificado();
        short longitud = registro.copiar(apdu.getBuffer(), (short) 0);
        apdu.setOutgoingAndSend((short) 0, longitud);
    }

    // Cuando el terminal ya lo ha recogido.
    private void vaciarRegistro() {
        exigirPinVerificado();
        registro.vaciar();
    }

    // Lo que comparten los comandos.

    private void exigirPinVerificado() {
        if (!pinVerificado[0]) {
            ISOException.throwIt(ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED);
        }
    }

    private void recibir(APDU apdu, short longitud) {
        if (apdu.setIncomingAndReceive() != longitud) {
            ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
        }
    }

    private short recibirImporte(APDU apdu) {
        recibir(apdu, LONGITUD_IMPORTE);
        return Util.getShort(apdu.getBuffer(), ISO7816.OFFSET_CDATA);
    }

    // El PIN que llega, relleno hasta ocho bytes en el búfer de trabajo.
    private void recibirPin(APDU apdu) {
        short longitud = apdu.setIncomingAndReceive();
        if (longitud < PIN_LONGITUD_MINIMA || longitud > PIN_LONGITUD_MAXIMA) {
            ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
        }
        Util.arrayFillNonAtomic(recibido, (short) 0, PIN_LONGITUD_MAXIMA, PIN_RELLENO);
        Util.arrayCopyNonAtomic(apdu.getBuffer(), ISO7816.OFFSET_CDATA, recibido, (short) 0, longitud);
    }

    // El intento se gasta antes de comparar, como en el C del equipo (1.3), y
    // nunca dentro de una transacción: si se deshiciera, el intento volvería.
    private void comprobarPin() {
        if (!personalizada || intentos == 0) {
            ISOException.throwIt(SW_PIN_BLOQUEADO);
        }
        intentos--;
        if (Util.arrayCompare(recibido, (short) 0, pin, (short) 0, PIN_LONGITUD_MAXIMA) != 0) {
            ISOException.throwIt((short) (SW_VERIFICACION_FALLIDA | intentos));
        }
        intentos = PIN_INTENTOS;
    }
}
