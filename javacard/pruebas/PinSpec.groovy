class PinSpec extends TarjetaSimulada {

    def 'rechaza un PIN de longitud incorrecta'() {
        expect:
        verificar([1, 2, 3] as byte[]).SW == SW_WRONG_LENGTH
    }

    def 'intentos que quedan tras el fallo #fallo: #quedan'() {
        given:
        (fallo - 1).times { verificar(PIN_INCORRECTO) }

        when:
        def respuesta = verificar(PIN_INCORRECTO)

        then:
        respuesta.SW == (SW_VERIFICACION_FALLIDA | quedan)

        where:
        fallo || quedan
        1     || 2
        2     || 1
        3     || 0
    }

    def 'bloqueada, no acepta ni el PIN correcto'() {
        given:
        3.times { verificar(PIN_INCORRECTO) }

        expect:
        verificar(PIN).SW == SW_PIN_BLOQUEADO
    }
}
