class RegistroSpec extends TarjetaSimulada {

    def 'con el registro lleno, un pago se rechaza y el saldo no cambia'() {
        given:
        verificar()
        llenarElRegistroConUnSaldoDe(930)

        when:
        def respuesta = pagar(10)

        then:
        respuesta.SW == SW_FILE_FULL
        saldo() == 930
    }

    def 'con el registro lleno, una recarga se rechaza y el saldo no cambia'() {
        given:
        verificar()
        llenarElRegistroConUnSaldoDe(930)

        when:
        def respuesta = recargar(500)

        then:
        respuesta.SW == SW_FILE_FULL
        saldo() == 930
    }
}
