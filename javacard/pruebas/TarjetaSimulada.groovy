import cardcode.tarjeta.TarjetaApplet
import com.licel.jcardsim.smartcardio.CardSimulator
import com.licel.jcardsim.utils.AIDUtil
import javacard.framework.ISO7816
import javax.smartcardio.CommandAPDU
import javax.smartcardio.ResponseAPDU
import spock.lang.Specification

/** La tarjeta en el simulador, ya personalizada, y lo que le manda el terminal. */
abstract class TarjetaSimulada extends Specification {

    // Los códigos de estado con el nombre de la norma, como en el applet.
    static final int SW_NO_ERROR = ISO7816.SW_NO_ERROR & 0xFFFF
    static final int SW_WRONG_LENGTH = ISO7816.SW_WRONG_LENGTH & 0xFFFF
    static final int SW_SECURITY_STATUS_NOT_SATISFIED = ISO7816.SW_SECURITY_STATUS_NOT_SATISFIED & 0xFFFF
    static final int SW_CONDITIONS_NOT_SATISFIED = ISO7816.SW_CONDITIONS_NOT_SATISFIED & 0xFFFF
    static final int SW_WRONG_DATA = ISO7816.SW_WRONG_DATA & 0xFFFF
    static final int SW_FILE_FULL = ISO7816.SW_FILE_FULL & 0xFFFF
    static final int SW_INS_NOT_SUPPORTED = ISO7816.SW_INS_NOT_SUPPORTED & 0xFFFF
    // Los dos que la API 3.0.5 no trae con nombre; la prueba no los copia del applet.
    static final int SW_PIN_BLOQUEADO = 0x6983
    static final int SW_VERIFICACION_FALLIDA = 0x63C0

    static final byte[] PIN = [1, 2, 3, 4] as byte[]
    static final byte[] PIN_INCORRECTO = [9, 9, 9, 9] as byte[]

    static final int INS_VERIFICAR_PIN = 0x20
    static final int INS_PAGAR = 0x30
    static final int INS_RECARGAR = 0x32
    static final int INS_CONSULTAR = 0x34
    static final int INS_PERSONALIZAR = 0x50

    static final int CAPACIDAD_DEL_REGISTRO = 8

    def aid = AIDUtil.create('F000000002')
    def tarjeta = new CardSimulator(new SimuladorConTransacciones())

    def setup() {
        tarjeta.installApplet(aid, TarjetaApplet)
        tarjeta.selectApplet(aid)
        assert personalizar(PIN).SW == SW_NO_ERROR
    }

    ResponseAPDU enviar(int ins, byte[] datos) {
        tarjeta.transmitCommand(new CommandAPDU(0x80, ins, 0, 0, datos))
    }

    ResponseAPDU personalizar(byte[] pin) { enviar(INS_PERSONALIZAR, pin) }
    ResponseAPDU verificar(byte[] pin = PIN) { enviar(INS_VERIFICAR_PIN, pin) }
    ResponseAPDU pagar(int importe) { enviar(INS_PAGAR, dosBytes(importe)) }
    ResponseAPDU recargar(int importe) { enviar(INS_RECARGAR, dosBytes(importe)) }

    int saldo() {
        def respuesta = tarjeta.transmitCommand(new CommandAPDU(0x80, INS_CONSULTAR, 0, 0, 256))
        assert respuesta.SW == SW_NO_ERROR
        ((respuesta.data[0] & 0xFF) << 8) | (respuesta.data[1] & 0xFF)
    }

    /** Recarga el importe y paga hasta que no cabe ningún movimiento más en el registro. */
    void llenarElRegistroConUnSaldoDe(int saldoFinal) {
        int pagos = CAPACIDAD_DEL_REGISTRO - 1
        int pago = 10
        assert recargar(saldoFinal + pagos * pago).SW == SW_NO_ERROR
        pagos.times { assert pagar(pago).SW == SW_NO_ERROR }
    }

    static byte[] dosBytes(int valor) {
        [(byte) (valor >> 8), (byte) valor] as byte[]
    }
}
