class ComandosSpec extends TarjetaSimulada {

    def 'se personaliza una sola vez'() {
        expect:
        personalizar(PIN_INCORRECTO).SW == SW_CONDITIONS_NOT_SATISFIED
    }

    def 'rechaza una instrucción que no conoce'() {
        expect:
        enviar(0x40, new byte[0]).SW == SW_INS_NOT_SUPPORTED
    }
}
