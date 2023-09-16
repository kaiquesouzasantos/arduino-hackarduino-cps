#include <Servo.h>

/* VARIAVEIS DO MOTOR SERVO - DEDO */

const int POLEGAR = 2,
    INDICADOR = 3,
    MEDIO = 4,
    ANELAR = 5,
    MINIMO = 6;

Servo polegar, indicador, medio, anelar, minimo;

Servo mao[] = {
    polegar,
    indicador,
    medio,
    anelar,
    minimo
};

int portasMao[] = {
    POLEGAR,
    INDICADOR,
    MEDIO,
    ANELAR,
    MINIMO
};

/* VARIAVEIS SENSORIAIS */

const int SENSOR_UNITARIO = A0;

const int TRIGGER_ULTRASONICO = 8,
    ECHO_ULTRASONICO = 9;

const int SENSOR_POLEGAR = A1,
    SENSOR_INDICADOR = A2,
    SENSOR_MEDIO = A3,
    SENSOR_ANELAR = A4,
    SENSOR_MINIMO = A5;

int portasSensor[] = {
    SENSOR_POLEGAR,
    SENSOR_INDICADOR,
    SENSOR_MEDIO,
    SENSOR_ANELAR,
    SENSOR_MINIMO
};

/* CONTROLE DE DECLARACAO DE PORTAS */

bool SENSOR_DEFINIDO = false;

/* CONTROLE QUANTITATIVO */

const int LIMITE_DEDOS = 5;

/* INICIALIZACAO */

void setup() {
    Serial.begin(9600);
    setupMao();

    escreve("################# Programa Inicializado #################");
    escreve("Selecao de Modalidade de Captacao de Informacao");
    escreve("- ultrasonico\n- potenciometro\n- potenciometros\n- flexivel\n- terminal\nDigite a opcao desejada: ");
}

void setupMao() {
    for (int dedo = 0; dedo < LIMITE_DEDOS; dedo++) {
        mao[dedo].attach(portasMao[dedo]);
        mao[dedo].write(0);
    }
}

void setupSensor() {
    SENSOR_DEFINIDO = true;
    for (int sensor = 0; sensor < LIMITE_DEDOS; sensor++) {
        pinMode(portasSensor[sensor], INPUT);
    }
}

void setupUltrasonico() {
    SENSOR_DEFINIDO = true;
    pinMode(TRIGGER_ULTRASONICO, OUTPUT);
    pinMode(ECHO_ULTRASONICO, INPUT);
}

void setupUnitario() {
    SENSOR_DEFINIDO = true;
    pinMode(SENSOR_UNITARIO, INPUT);
}

/* CONTINUO */

void loop() {
    while (Serial.available() > 0) {
        String entrada = Serial.readString();

        if (entrada == "ultrasonico") {
            entradaUltrasonica();
        } else if (entrada == "potenciometro") {
            entradaPotenciometro();
        } else if (entrada == "potenciometros") {
            entradaMultiplaPotenciometro();
        } else if (entrada == "flexivel") {
            entradaMultiplaFlexivel();
        } else if (entrada == "terminal") {
            entradaTerminal();
        } else {
            escreve("Comando Invalido!");
        }
    }
}

/* ENVIO DE INFORMACAO */

void escreve(String texto) {
    Serial.println(texto);
}

/* CAPTACAO DE INFORMACAO */

void entradaUltrasonica() {
    verificaSetup("ultrasonico");

    while (true) {
        int angulo = getAngulo(getLeituraUltrasonica(), 1, 30);

        for (int dedo = 0; dedo < LIMITE_DEDOS; dedo++) {
            setPosicaoDedo(dedo, angulo);
        }
    }
}

void entradaMultiplaPotenciometro() {
    verificaSetup("multiplo");

    while (true) {
        for (int sensor = 0; sensor < LIMITE_DEDOS; sensor++) {
            setPosicaoDedo(
                sensor,
                getAngulo(getLeitura(portasSensor[sensor]), 1023)
            );
        }
    }
}

void entradaMultiplaFlexivel() {
    verificaSetup("multiplo");

    while (true) {
        for (int sensor = 0; sensor < LIMITE_DEDOS; sensor++) {
            setPosicaoDedo(
                sensor,
                getAngulo(getLeitura(portasSensor[sensor]), 384, 783)
            );
        }
    }
}

void entradaPotenciometro() {
    verificaSetup("unitario");

    while (true) {
        for (int dedo = 0; dedo < LIMITE_DEDOS; dedo++) {
            setPosicaoDedo(
                dedo,
                getAngulo(getLeitura(SENSOR_UNITARIO), 1023)
            );
        }
    }
}

void entradaTerminal() {
    escreve("Digite os angulos dos dedos sequencialmente, no seguinte formato:\nxxx xxx xxx xxx xxx");

    while (true) {
        while (Serial.available() > 0) {
            int * entradas = getValores(Serial.readString());

            for (int valor = 0; valor < LIMITE_DEDOS; valor++) {
                int angulo = entradas[valor];

                if (angulo < 0 || angulo > 179) {
                    continue;
                }

                setPosicaoDedo(valor, angulo);
            }
        }
    }
}

/* SETUP DE SENSOR */

void verificaSetup(String tipo) {
    if (SENSOR_DEFINIDO) {
        return;
    }

    if (tipo == "multiplo") {
        setupSensor();
    } else if (tipo == "unitario") {
        setupUnitario();
    } else if (tipo == "ultrasonico") {
        setupUltrasonico();
    }

    escreve("OK");
}

/* LEITURA */

int getLeitura(int portaSensorial) {
    return analogRead(portaSensorial);
}

double getLeituraUltrasonica() {
    digitalWrite(TRIGGER_ULTRASONICO, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_ULTRASONICO, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_ULTRASONICO, LOW);

    return pulseIn(ECHO_ULTRASONICO, HIGH) / 58.2;
}

/* CONVERSAO */

int* getValores(String entrada) {
    int * valores = new int[LIMITE_DEDOS];
    int index = 0;

    while (entrada.length() > 0 && index < LIMITE_DEDOS) {
        int pos = entrada.indexOf(' ');

        if (pos != -1) {
            String valorStr = entrada.substring(0, pos);
            valores[index] = valorStr.toInt();
            index++;

            entrada = entrada.substring(pos + 1);
        } else {
            valores[index] = entrada.toInt();
            break;
        }
    }

    return valores;
}

/* CALIBRACAO */

int getAngulo(int valor, int limite_valor) {
    return map(valor, 0, limite_valor, 0, 179);
}

int getAngulo(int valor, int inicial_valor, int limite_valor) {
    return map(valor, inicial_valor, limite_valor, 0, 179);
}

/* MOVIMENTACAO */

void setPosicaoDedo(int dedo, int angulo) {
    mao[dedo].write(angulo);
}
