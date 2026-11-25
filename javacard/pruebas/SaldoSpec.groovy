class SaldoSpec extends TarjetaSimulada {

    def 'sin el PIN no paga ni recarga'() {
        expect:
        pagar(100).SW == SW_SECURITY_STATUS_NOT_SATISFIED
        recargar(100).SW == SW_SECURITY_STATUS_NOT_SATISFIED
    }

    def 'recarga, paga y consulta el saldo'() {
        given:
        verificar()

        when:
        recargar(1000)
        pagar(250)

        then:
        saldo() == 750
    }

    def 'no paga más de lo que hay, y el saldo no cambia'() {
        given:
        verificar()
        recargar(1000)
        pagar(250)

        when:
        def respuesta = pagar(768)

        then:
        respuesta.SW == SW_CONDITIONS_NOT_SATISFIED
        saldo() == 750
    }

    def 'rechaza un pago de cero'() {
        given:
        verificar()

        expect:
        pagar(0).SW == SW_WRONG_DATA
    }

    def 'rechaza una recarga de #importe (#motivo)'() {
        given:
        verificar()

        expect:
        recargar(importe).SW == SW_WRONG_DATA

        where:
        importe | motivo
        0       | 'cero'
        30001   | 'por encima del saldo máximo'
        40000   | 'no cabe en un short'
    }
}
