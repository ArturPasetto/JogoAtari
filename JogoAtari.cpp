#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include <time.h>
#include <GL/glut.h>

#define PI 3.14159265358979323846
#define SPACEBAR 32

#define largura_janela 1200
#define altura_janela 800

#define largura_parede (largura_janela/4)
#define largura_casa 50
#define largura_jato 10
#define largura_asa_jato 50
#define largura_navio 50
#define largura_helicoptero 25
#define largura_cauda_helicoptero 50 
#define largura_posto_combustivel (largura_jato + largura_asa_jato)
#define largura_chanfro (largura_parede/2)
#define altura_painel (altura_janela/4)
#define altura_casa 30
#define altura_jato 42.5
#define altura_ponta_jato 10
#define altura_navio 10
#define altura_helicoptero 12.5
#define altura_posto_combustivel 125
#define altura_tiro 50
#define altura_ponte 150
#define altura_chanfro 150

#define lugar_primeira_ponte 3000
#define lugar_segunda_ponte 8000
#define lugar_terceira_ponte 15000

GLint pontuacao = 0;     
GLint vidas = 3;
GLint checkpoint = 0;
GLfloat velocidade_deslocamento = 3;
GLfloat velocidade_consumo_combustivel = 0.1;
GLfloat distancia_percorrida = 0;
GLfloat distancia_animacao = 0;
GLfloat timer = 0;
GLfloat espera_3d = 0;
GLfloat rotacionar = 0;

//posições
GLfloat jato_h = 0;
GLfloat casa_h = 0;
GLfloat casa_v = altura_janela;
GLfloat posto_combustivel_h = 0;
GLfloat posto_combustivel_v = altura_janela;
GLfloat tiro_h = 0;
GLfloat tiro_v = -altura_painel + altura_jato + altura_ponta_jato + 10;
GLfloat pos_tiro = 0;
GLfloat consumo_combustivel = 0;
GLfloat lugar_ponte = 0;

//Navios
GLfloat navio_h = 0;
GLfloat navio_h_2 = 0;
GLfloat navio_h_3 = 0;
GLfloat navio_v = altura_janela;
GLfloat navio_v_2 = altura_janela;
GLfloat navio_v_3 = altura_janela;
GLboolean gerou_navio = false;
GLboolean gerou_navio_2 = false;
GLboolean gerou_navio_3 = false;

//Helicópteros
GLfloat heli_h = 0;
GLfloat heli_h_2= 0;
GLfloat heli_h_3 = 0;
GLfloat heli_v = altura_janela;
GLfloat heli_v_3 = altura_janela;
GLfloat heli_v_2 = altura_janela;
GLboolean gerou_helicoptero = false;
GLboolean gerou_helicoptero_2 = false;
GLboolean gerou_helicoptero_3 = false;

 
//verificações
GLboolean desenho_3d = true;
GLboolean atirou = false;
GLboolean bateu = false;
GLboolean animacao = true;
GLboolean gerou_posto_combustivel = false;
GLboolean vitoria = false;
GLboolean gerou_casa = false;
GLboolean abastecendo = false;
GLboolean primeira_ponte = false;
GLboolean segunda_ponte = false;
GLboolean terceira_ponte = false;
GLboolean acertou_primeira_ponte = false;
GLboolean acertou_segunda_ponte = false;
GLboolean acertou_terceira_ponte = false;

//Iluminação
GLfloat ambiente[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat diffuse[] = { 0.8862f, 0.1686f, 0.2588f, 1.0f };//vermelho 226,43,66 
GLfloat specular[] = { 0.8862f, 0.1686f, 0.2588f, 1.0f };//vermelho 226, 43, 66
GLfloat posicao[] = { 0.0f, 1.0f, 2.0f, 0.0f };
GLfloat padrao[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_ambiente[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat alto_brilho[] = { 1.0 };

GLint randomizaBoolean() {
    return rand() % 2;
}

GLfloat randomizar(GLfloat largura) {
    GLfloat valor = 0;
    
    do {
        valor = (rand() % largura_parede + 1);
    } while ((valor + largura) > largura_parede);

    if (randomizaBoolean()) return valor;
    return -valor;
}

GLfloat randomizarCasa(GLfloat largura) {
    GLfloat valor = 0;

    do{
        valor = (rand() % largura_parede) + largura_parede;
    } while ((valor - largura) < largura_parede);
    
    if (randomizaBoolean()) return valor;
    return -valor;
}

void configuracaoLuz(float ambiente[4], float diffuse[4], float specular[4], float posicao[4]) {

    GLfloat lmodel_ambiente[] = { 0.5f, 0.5f, 0.5f , 1.0f };
    GLfloat local_view[] = { 0.0f };

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, posicao);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambiente);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void resetar() {
    if(vidas>0 && !desenho_3d){
        vidas--;
    }
    velocidade_deslocamento = 3;
    velocidade_consumo_combustivel = 0.1;
    distancia_animacao = 0;
    timer = 0;

    if (checkpoint == 0) {
        distancia_percorrida = 0;
    }
    else if (checkpoint == 1) { 
        distancia_percorrida = lugar_primeira_ponte + altura_ponte + altura_chanfro;
        acertou_primeira_ponte = true;
        primeira_ponte = false;
    }
    else if (checkpoint == 2) {
        distancia_percorrida = lugar_segunda_ponte + altura_ponte + altura_chanfro;
        acertou_primeira_ponte = true;
        acertou_segunda_ponte = true;
        primeira_ponte = false;
        segunda_ponte = false;
    }
    else if (checkpoint == 3) {
        distancia_percorrida = lugar_terceira_ponte + altura_ponte + altura_chanfro;
        acertou_primeira_ponte = true;
        acertou_segunda_ponte = true;
        acertou_terceira_ponte = true;
        terceira_ponte = false;
        primeira_ponte = false;
        segunda_ponte = false;
    }

    jato_h = 0;
    casa_h = 0;
    casa_v = altura_janela;
    posto_combustivel_h = 0;
    posto_combustivel_v = altura_janela;
    tiro_h = 0;
    tiro_v = -altura_painel + 10 + altura_jato + altura_ponta_jato;
    pos_tiro = 0;
    consumo_combustivel = 0;
    lugar_ponte = 0;

    //Navios
    navio_h = 0;
    navio_v = altura_janela;
    navio_h_2 = 0;
    navio_v_2 = altura_janela;
    navio_h_3 = 0;
    navio_v_3 = altura_janela;
    gerou_navio = false;
    gerou_navio_2 = false;
    gerou_navio_3 = false;

    //Helicópteros
    heli_h = 0;
    heli_v = altura_janela;
    heli_h_2 = 0;
    heli_v_2 = altura_janela;
    heli_h_3 = 0;
    heli_v_3 = altura_janela;
    gerou_helicoptero = false;
    gerou_helicoptero_2 = false;
    gerou_helicoptero_3 = false;

    
    atirou = false;
    bateu = false;
    animacao = true;
    gerou_posto_combustivel = false;
    gerou_casa = false;
    abastecendo = false;
}

void desenharEntrada(GLfloat rotacionar) {
    glPushMatrix();
    glEnable(GL_LIGHTING);
    glTranslatef(0, 0, -5.0f);
    glRotatef(30 + rotacionar,1,1,0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    
        char nome_str[80];
        strcpy_s(nome_str, "Artur Pasetto");
        GLint nome_length = (GLint)strlen(nome_str);
        GLint nome_cont = 0;

        glTranslatef(-1.0, -1.5, 0);

        glPushMatrix();
        glDisable(GL_LIGHTING);
        glScalef(0.001, 0.001, 1);
        glTranslatef(0, 5.0f, -5.0f);
        
        glLineWidth(3);
        for (nome_cont = 0; nome_cont < nome_length; nome_cont++) {
            glPushMatrix();
            glColor3ub(255, 255, 255);
            glTranslatef((200 * nome_cont), 0, 0);
            glutStrokeCharacter(GLUT_STROKE_ROMAN, nome_str[nome_cont]);
            glPopMatrix();
        }
        glPopMatrix();

    glPopMatrix();
}

void desenharVitoria(GLfloat rotacionar) {
    glPushMatrix();
    glEnable(GL_LIGHTING);
    glTranslatef(0, 0, -5.0f);
    glRotatef(30 + rotacionar, 1, 1, 0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();

    char vitoria_str[80];
    char pontos_str[20];
    strcpy_s(vitoria_str, "PARABENS! VOCE VENCEU COM ");
    sprintf_s(pontos_str, "%i", pontuacao);
    strcat_s(vitoria_str, pontos_str);
    strcat_s(vitoria_str, " PONTOS");

    GLint vitoria_lenght = (GLint)strlen(vitoria_str);
    GLint vitoria_cont = 0;

    glTranslatef(-2.8f, -1.5f, 0);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glScalef(0.00075, 0.00075, 1);
    glTranslatef(0, 5.0f, -5.0f);

    glLineWidth(3);
    for (vitoria_cont = 0; vitoria_cont < vitoria_lenght; vitoria_cont++) {
        glPushMatrix();
        glColor3ub(255, 255, 255);
        glTranslatef((200 * vitoria_cont), 0, 0);
        glutStrokeCharacter(GLUT_STROKE_ROMAN, vitoria_str[vitoria_cont]);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix();
}

void desenharEstradaPonte() {
    glPushMatrix();

    //chanfro esquerdo
    glBegin(GL_QUADS);
    glColor3ub(59, 175, 154);
    glVertex2f(-largura_parede, altura_chanfro);
    glColor3ub(26, 128, 53);
    glVertex2f(-largura_chanfro, altura_chanfro);
    glVertex2f(-largura_chanfro, -altura_chanfro);
    glColor3ub(59, 175, 154);
    glVertex2f(-largura_parede, -altura_chanfro);
    glEnd();

    //chanfro direito
    glBegin(GL_QUADS);
    glColor3ub(59, 175, 154);
    glVertex2f(largura_parede, altura_chanfro);
    glColor3ub(26, 128, 53);
    glVertex2f(largura_chanfro, altura_chanfro);
    glVertex2f(largura_chanfro, -altura_chanfro);
    glColor3ub(59, 175, 154);
    glVertex2f(largura_parede, -altura_chanfro);
    glEnd();

    glPushMatrix();
        glTranslatef(0, -75, 0);
        glColor3ub(151, 151, 151);

        //estrada esquerda
        glBegin(GL_QUADS);
        glVertex2f(-largura_janela, 0);
        glVertex2f(-(largura_parede / 2), 0);
        glVertex2f(-(largura_parede / 2), altura_ponte);
        glVertex2f(-largura_janela, altura_ponte);
        glEnd();

        //estrada direita
        glBegin(GL_QUADS);
        glVertex2f(largura_janela, 0);
        glVertex2f((largura_parede / 2), 0);
        glVertex2f((largura_parede / 2), altura_ponte);
        glVertex2f(largura_janela, altura_ponte);
        glEnd();

        //faixa amarela esquerda
        glColor3ub(238, 209, 128);
        glBegin(GL_QUADS);
        glVertex2f(-largura_janela, 62.5);
        glVertex2f(-largura_parede + largura_chanfro, 62.5);
        glVertex2f(-largura_parede + largura_chanfro, altura_ponte - 62.5);
        glVertex2f(-largura_janela, altura_ponte - 62.5);
        glEnd();

        //faixa amarela direita
        glBegin(GL_QUADS);
        glVertex2f(largura_janela, 62.5);
        glVertex2f(largura_parede - largura_chanfro, 62.5);
        glVertex2f(largura_parede - largura_chanfro, altura_ponte - 62.5);
        glVertex2f(largura_janela, altura_ponte - 62.5);
        glEnd();

    glPopMatrix();

    glPopMatrix();
}

void desenharPonte() {
    GLint i = 0;

    glPushMatrix();
    glTranslatef(0, 50, 0);
    for (i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(0, -(i * (altura_ponte / 8)), 0);
        
        if(i % 2 == 0) glColor3ub(171, 135, 50);
        else glColor3ub(150,113,26);

        glBegin(GL_QUADS);
        glVertex2f(-largura_parede, 0);
        glVertex2f(largura_parede, 0);
        glVertex2f(largura_parede, (altura_ponte / 8));
        glVertex2f(-largura_parede, (altura_ponte / 8));
        glEnd();
        glPopMatrix();
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -75, 0);
    glColor3ub(238, 209, 128);
    glBegin(GL_QUADS);
    glVertex2f(-largura_parede, 62.5);
    glVertex2f(largura_parede - largura_chanfro, 62.5);
    glVertex2f(largura_parede - largura_chanfro, altura_ponte - 62.5);
    glVertex2f(-largura_parede, altura_ponte - 62.5);
    glEnd();
    glPopMatrix();
}

void desenharCasa() {

    //estrutura
    glPushMatrix();

    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(-largura_casa, 0);
    glVertex2f(largura_casa, 0);
    glVertex2f(largura_casa, altura_casa);
    glVertex2f(-largura_casa, altura_casa);
    glEnd();

    //telhado centro
    glColor3ub(255,0,0);
    glBegin(GL_QUADS);
    glVertex2f(-(largura_casa / 2), altura_casa);
    glVertex2f((largura_casa / 2), altura_casa);
    glVertex2f((largura_casa / 2), (1.5 * altura_casa));
    glVertex2f(-(largura_casa / 2), (1.5 * altura_casa));
    glEnd();
    
    //telhado metade esquerda
    glBegin(GL_QUADS);
    glVertex2f(-(largura_casa / 1.5), altura_casa);
    glVertex2f((largura_casa / 1.5), altura_casa);
    glVertex2f((largura_casa / 1.5), (1.25 * altura_casa));
    glVertex2f(-(largura_casa / 1.5), (1.25 * altura_casa));
    glEnd();
    
    //telhado metade menor 
    glBegin(GL_QUADS);
    glVertex2f(-largura_casa, altura_casa);
    glVertex2f(largura_casa, altura_casa);
    glVertex2f(largura_casa, (1.125 * altura_casa));
    glVertex2f(-largura_casa, (1.125 * altura_casa));
    glEnd();

    glPopMatrix();
    
}

void desenharPostoCombustivel() {

    glPushMatrix();
        glColor3ub(128, 0, 0);
        glBegin(GL_POLYGON);
            glVertex2f(-largura_posto_combustivel, 0);
            glVertex2f(largura_posto_combustivel, 0);
            glVertex2f(largura_posto_combustivel, altura_posto_combustivel - 25);
            glVertex2f(5, altura_posto_combustivel);
            glVertex2f(-5, altura_posto_combustivel);
            glVertex2f(-largura_posto_combustivel, altura_posto_combustivel - 25);
        glEnd();
    glPopMatrix();

    char fuel_str[80];
    strcpy_s(fuel_str, "FUEL");
    GLint fuel_length = (GLint)strlen(fuel_str);
    GLint fuel_cont = 0;

    glPushMatrix();
        glScalef(0.15, 0.15, 1);
        glTranslatef(-25, 675, 0);
        glColor3f(255,255,255);
        glLineWidth(3);
        for (fuel_cont = 0; fuel_cont < fuel_length; fuel_cont++) {
            glPushMatrix();
                glTranslatef(0, (-200 * fuel_cont), 0);
                glutStrokeCharacter(GLUT_STROKE_ROMAN, fuel_str[fuel_cont]);
            glPopMatrix();
        }
    glPopMatrix();
}

void desenharTiro() {
    glColor3ub(255, 255, 255);
    glBegin(GL_QUADS);
    glVertex2f(-5, tiro_v);
    glVertex2f(5, tiro_v);
    glVertex2f(5, tiro_v + altura_tiro);
    glVertex2f(- 5, tiro_v + altura_tiro);
    glEnd();
}

void desenharUmQuartoCirculo(GLfloat raio, GLint pontos, GLfloat translate_x, GLfloat translate_y) {
    GLint i = 0;
    GLfloat angle = 0;

    glPushMatrix();
    glTranslatef(translate_x, translate_y, 0);
    glBegin(GL_POLYGON);
    glVertex2f(0.5f, 0.5f);
    for (i = 0; i < pontos; i++) {
        angle = (PI / 2) * i / pontos;
        glVertex2f(0.5 + cos(angle) * raio, 0.5 + sin(angle) * raio);
    }
    glEnd();
    glPopMatrix();
}

void desenharAsasEsquerdaJato() {
    //asa maior esquerda
    glColor3ub(105, 98, 69);
    glBegin(GL_POLYGON);
    glVertex2f(-largura_jato, altura_jato - 10);
    glVertex2f(-largura_asa_jato, altura_jato - 10);
    glVertex2f(-largura_asa_jato, altura_jato - 30);
    glVertex2f(-largura_jato, altura_jato - 40);
    glEnd();
    
    glPushMatrix();
    glLineWidth(1);
    glTranslatef(-largura_asa_jato + 1, altura_jato - 20, 0);
    //glColor3ub(94, 49, 43);

    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    desenharUmQuartoCirculo(10, 1000, 0, 0);
    glRotatef(90, 0, 0, 1);
    desenharUmQuartoCirculo(10, 1000, 0, -1);
    glPopMatrix();

    glPopMatrix();

    //asa menor esquerda
    glColor3ub(105,98,69);
    glBegin(GL_QUADS);
    glVertex2f(0, -altura_jato);
    glVertex2f(-(largura_asa_jato / 2), -altura_jato);
    glVertex2f(-(largura_asa_jato / 2), -altura_jato + 10);
    glVertex2f(0, -altura_jato + 20);
    glEnd();

    glPushMatrix();
    glLineWidth(1);
    glTranslatef(-(largura_asa_jato / 2) + 5, -altura_jato, 0);
    glRotatef(90, 0, 0, 1);
    //glColor3ub(94, 49, 43);
    desenharUmQuartoCirculo(10, 1000, 0, 0);
    glPopMatrix();
    
}

void desenharAsasDireitaJato() {
    //asa maior direita
    glColor3ub(105, 98, 69);
    glBegin(GL_POLYGON);
    glVertex2f(largura_jato, altura_jato - 10);
    glVertex2f(largura_asa_jato, altura_jato - 10);
    glVertex2f(largura_asa_jato, altura_jato - 30);
    glVertex2f(largura_jato, altura_jato - 40);
    glEnd();

    glPushMatrix();
    glLineWidth(1);
    glTranslatef(largura_asa_jato - 1, altura_jato - 20, 0);
    //glColor3ub(94, 49, 43);
    glPushMatrix();
    desenharUmQuartoCirculo(10, 1000, 0, 0);
    glRotatef(-90, 0, 0, 1);
    desenharUmQuartoCirculo(10, 1000, -1,0);
    glPopMatrix();

    glPopMatrix();

    //asa menor direita
    glColor3ub(105, 98, 69);
    glBegin(GL_QUADS);
    glVertex2f(0, -altura_jato);
    glVertex2f((largura_asa_jato / 2), -altura_jato);
    glVertex2f((largura_asa_jato / 2), -altura_jato + 10);
    glVertex2f(0, -altura_jato + 20);
    glEnd();

    glPushMatrix();
    glLineWidth(1);
    //glColor3ub(94, 49, 43);
    glTranslatef((largura_asa_jato / 2) - 5, -altura_jato, 0);
    desenharUmQuartoCirculo(10, 1000, 0, 0);
    glPopMatrix();

}

void desenharFuselagem() {
    glColor3ub(105, 98, 69);

    //fuselagem top - asa
    glBegin(GL_QUADS);
    glVertex2f(-largura_jato, altura_jato);
    glVertex2f(largura_jato, altura_jato);
    glVertex2f(largura_jato, altura_jato - 40);
    glVertex2f(-largura_jato, altura_jato - 40);
    glEnd();

    //fuselagem asa - final
    glBegin(GL_QUADS);
    glVertex2f(-largura_jato, altura_jato - 40);
    glVertex2f(largura_jato, altura_jato - 40);
    glVertex2f((largura_jato / 2), -altura_jato);
    glVertex2f(-(largura_jato / 2), -altura_jato);
    glEnd();

}

void desenharMotor(GLfloat raio, GLint pontos) {
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    desenharUmQuartoCirculo(raio, pontos, -1, -1);
    glPopMatrix();

    glPushMatrix();
    desenharUmQuartoCirculo(raio, pontos, -1, 0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1, raio, 0);
    glColor3ub(94, 49, 43);

    glBegin(GL_TRIANGLES);
    glVertex2f(0, 5);
    glVertex2f(-5, 0);
    glVertex2f(5, 0);
    glEnd();

    glTranslatef(2.5, 0, 0);
    glColor3ub(0, 0, 0);

    glBegin(GL_QUADS);
    glVertex2f(0, 2);
    glVertex2f(0, 4);
    glVertex2f(10, 4);
    glVertex2f(10, 2);
    glEnd();

    glTranslatef(-5, 0, 0);

    glBegin(GL_QUADS);
    glVertex2f(0, 2);
    glVertex2f(0, 4);
    glVertex2f(-10, 4);
    glVertex2f(-10, 2);
    glEnd();

    glPopMatrix();
}

void desenharCockpit() {
    
    glColor3ub(138, 158, 183);
    
    glPushMatrix();
    glTranslatef(0, altura_jato - 10, 0);
    desenharUmQuartoCirculo(5, 1000, -1, -1);
    glRotatef(90, 0, 0, 1);
    desenharUmQuartoCirculo(5, 1000, -1, -1);
    glPopMatrix();

    //vidro
    glBegin(GL_QUADS);
    glVertex2f(5, altura_jato - 10);
    glVertex2f(5, altura_jato - 35);
    glVertex2f(-5, altura_jato - 35);
    glVertex2f(-5, altura_jato - 10);
    glEnd();

    glTranslatef(0, 10, 0);

    glPushMatrix();
    glRotatef(-90, 0, 0, 1);
    desenharUmQuartoCirculo(5, 1000, 1, -1);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-180, 0, 0, 1);
    desenharUmQuartoCirculo(5, 1000, -1, 1);
    glPopMatrix();

}

void desenharJato() {

    glPushMatrix();

    glPushMatrix();
    //glColor3ub(94, 49, 43);
    glColor3ub(128,0,0);
    glTranslatef(0, altura_jato, 0);
    desenharMotor(altura_ponta_jato,1000);
    glPopMatrix();
    
    desenharAsasEsquerdaJato();
    desenharAsasDireitaJato();
    desenharFuselagem();
    desenharCockpit();

    glPopMatrix();
}

void desenharParedes() {

    //parede direita pt 1
    glBegin(GL_QUADS);
    glColor3ub(59, 175, 154);
    glVertex2f(largura_parede, -altura_janela);
    glColor3ub(26, 128, 53);
    glVertex2f((largura_janela/3), -altura_janela);
    glVertex2f((largura_janela/3), altura_janela);
    glColor3ub(59, 175, 154);
    glVertex2f(largura_parede, altura_janela);
    glEnd();

    //parede direita pt 2
    glBegin(GL_QUADS);
    glColor3ub(26, 128, 53);
    glVertex2f((largura_janela / 3), -altura_janela);
    glVertex2f(largura_janela, -altura_janela);
    glVertex2f(largura_janela, altura_janela);
    glVertex2f((largura_janela / 3), altura_janela);
    glEnd();


    //parede esquerda pt 1
    glBegin(GL_QUADS);
    glColor3ub(59, 175, 154);
    glVertex2f(-largura_parede, -altura_janela);
    glColor3ub(26, 128, 53);
    glVertex2f(-(largura_janela / 3), -altura_janela);
    glVertex2f(-(largura_janela / 3), altura_janela);
    glColor3ub(59, 175, 154);
    glVertex2f(-largura_parede, altura_janela);
    glEnd();

    //parede esquerda pt 2
    glBegin(GL_QUADS);
    glColor3ub(26, 128, 53);
    glVertex2f(-(largura_janela / 3), -altura_janela);
    glVertex2f(-largura_janela, -altura_janela);
    glVertex2f(-largura_janela, altura_janela);
    glVertex2f(-(largura_janela / 3), altura_janela);
    glEnd();
}

void desenharCombustivel() {

    GLfloat pc_h = (-altura_painel * 1.5);

    glPushMatrix();
    glColor3ub(0, 0, 0);
    glLineWidth(5);

    //retangulo
    glBegin(GL_LINE_LOOP);
    glVertex2f(225, pc_h + 25);
    glVertex2f(-225, pc_h + 25);
    glVertex2f(-225, pc_h - 25);
    glVertex2f(225, pc_h - 25);
    glEnd();

    //metade combustivel
    glBegin(GL_LINES);
    glVertex2f(0, pc_h + 25);
    glVertex2f(0, pc_h + 15);
    glEnd();

    // 1/2
    glPushMatrix();
    glScalef(0.15, 0.15, 1);
    glTranslatef(0 - (5 / 0.15) * 3, ((pc_h + 25) / 0.15) - (25 / 0.15), 0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, '1');
    glutStrokeCharacter(GLUT_STROKE_ROMAN, '/');
    glutStrokeCharacter(GLUT_STROKE_ROMAN, '2');
    glPopMatrix();

    //Empty
    glBegin(GL_LINES);
    glVertex2f(-200, pc_h + 25);
    glVertex2f(-200, pc_h + 15);
    glEnd();
    
    //E
    glPushMatrix();
    glScalef(0.15, 0.15, 1);
    glTranslatef((-200 / 0.15) - (5/0.15) , ((pc_h + 25) / 0.15) - (25 / 0.15), 0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
    glPopMatrix();

    //Full
    glBegin(GL_LINES);
    glVertex2f(200, pc_h + 25);
    glVertex2f(200, pc_h + 15);
    glEnd();

    //F
    glPushMatrix();
    glScalef(0.15, 0.15, 1);
    glTranslatef((200 / 0.15) - (5 / 0.15), ((pc_h + 25) / 0.15) - (25 / 0.15), 0);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, 'F');
    glPopMatrix();

    //medidor combustivel
    glColor3ub(238, 209, 128);
    glBegin(GL_LINES);
    glVertex2f(200 - consumo_combustivel, pc_h + 25);
    glVertex2f(200 - consumo_combustivel, pc_h - 25);
    glEnd();

    glPopMatrix();
    
}

void desenharVidas() {
    //vidas
    char cvida[2];
    sprintf_s(cvida, "%d", vidas);

    glPushMatrix();
    glTranslatef(0, (-altura_janela / 2) + 10, 0);
    glScalef(0.5f, 0.5f, 1);
    glColor3ub(238, 209, 128);
    glLineWidth(5);
    glutStrokeCharacter(GLUT_STROKE_ROMAN, cvida[0]);
    glPopMatrix();
}

void desenharPontuacao() {
    //pontuação
    char pontuacao_str[50];
    sprintf_s(pontuacao_str, "%i", pontuacao);
    int pontuacao_length = (int)strlen(pontuacao_str);
    int pontuacao_cont = 0;

    glPushMatrix();
    glTranslatef(-50, (-altura_janela / 2) + altura_painel - 65, 0);
    glScalef(0.5f, 0.5f, 1);
    glColor3ub(238, 209, 128);
    glLineWidth(5);
    for (pontuacao_cont = 0; pontuacao_cont < pontuacao_length; pontuacao_cont++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, pontuacao_str[pontuacao_cont]);
    }
    glPopMatrix();
}

void desenharPainel() {  
    //painel
    glBegin(GL_QUADS);
    glColor3ub(80, 80, 80);
    glVertex2f(-largura_janela, -altura_janela);
    glVertex2f(largura_janela, -altura_janela);
    glVertex2f(largura_janela, -altura_painel);
    glVertex2f(-largura_janela, -altura_painel);
    glEnd();

    desenharCombustivel();
    desenharVidas();
    desenharPontuacao();
}

void desenharNavio() {
   
    glPushMatrix();
    glColor3ub(128, 0, 0);

    //casco
    glBegin(GL_QUADS);
    glVertex2f(-largura_navio, 0);
    glVertex2f(largura_navio, 0);
    glVertex2f(largura_navio - 5, -altura_navio);
    glVertex2f(-largura_navio + 5, -altura_navio);
    glEnd();

    //deck
    glColor3ub(244, 164, 96);
   
    glBegin(GL_QUADS);
    glVertex2f(-largura_navio, 0);
    glVertex2f(largura_navio, 0);
    glVertex2f(largura_navio, 10);
    glVertex2f(-largura_navio, 10);
    glEnd();

    //superestrutura
    glPushMatrix();
    glColor3ub(128, 128, 128);
    glTranslatef(20, 0, 0);

    glBegin(GL_QUADS);
    glVertex2f(-5, 10);
    glVertex2f(-10, 10);
    glVertex2f(-10, 20);
    glVertex2f(-5, 20);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-5, 10);
    glVertex2f(5, 10);
    glVertex2f(5, 30);
    glVertex2f(-5, 30);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(5, 10);
    glVertex2f(15, 10);
    glVertex2f(15, 20);
    glVertex2f(5, 20);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(15, 10);
    glVertex2f(20, 10);
    glVertex2f(20, 15);
    glVertex2f(15, 15);
    glEnd();
    
    glPopMatrix();

    glPopMatrix();

}

void desenharHelicoptero() {
    
    glPushMatrix();
    glColor3ub(39,57,35);

    //corpo    
    glBegin(GL_POLYGON);
    glVertex2f(-largura_helicoptero+5, -altura_helicoptero);
    glVertex2f(largura_helicoptero-5, -altura_helicoptero);
    glVertex2f(largura_helicoptero, -altura_helicoptero+5);
    glVertex2f(largura_helicoptero, altura_helicoptero-5);
    glVertex2f(largura_helicoptero-5, altura_helicoptero);
    glVertex2f(-largura_helicoptero+5, altura_helicoptero);
    glVertex2f(-largura_helicoptero, altura_helicoptero-5);
    glVertex2f(-largura_helicoptero, -altura_helicoptero+5);
    glEnd();
    
    //cauda
    glBegin(GL_QUADS);
    glVertex2f(-largura_helicoptero, 5);
    glVertex2f(-largura_helicoptero, -5);
    glVertex2f(-largura_helicoptero - largura_cauda_helicoptero, -3);
    glVertex2f(-largura_helicoptero - largura_cauda_helicoptero, 3);
    glEnd();

    //pernas
    glBegin(GL_POLYGON);
    glVertex2f(-5, -altura_helicoptero);
    glVertex2f(5, -altura_helicoptero);
    glVertex2f(5, -altura_helicoptero - 5);
    glVertex2f(15, -altura_helicoptero - 5);
    glVertex2f(15, -altura_helicoptero - 10);
    glVertex2f(-15, -altura_helicoptero - 10);
    glVertex2f(-15, -altura_helicoptero - 5);
    glVertex2f(-5, -altura_helicoptero - 5);
    glEnd();

    //helices
    glColor3ub(0, 0, 0);

    glBegin(GL_QUADS);
    glVertex2f(-5, altura_helicoptero);
    glVertex2f(5, altura_helicoptero);
    glVertex2f(5, altura_helicoptero + 15);
    glVertex2f(-5, altura_helicoptero + 15);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(5, altura_helicoptero + 5);
    glVertex2f(15, altura_helicoptero + 5);
    glVertex2f(15, altura_helicoptero + 10);
    glVertex2f(5, altura_helicoptero + 10);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(-5, altura_helicoptero + 15);
    glVertex2f(-15, altura_helicoptero + 15);
    glVertex2f(-15, altura_helicoptero + 10);
    glVertex2f(-5, altura_helicoptero + 10);
    glEnd();

    glPopMatrix();
}

void desenharAnimacaoEntrada() {
    glPushMatrix();
    glColor3ub(26, 128, 53);
    desenharParedes();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, distancia_animacao, 0);
    desenharEstradaPonte();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -altura_painel + 50, 0);
    desenharJato();
    glPopMatrix();
}

void verificarBatidaNavio(GLfloat navio_h, GLfloat navio_v) {
    GLfloat posicaoFuselagem = -altura_painel + 50 + altura_ponta_jato + altura_jato;
    GLfloat posicaoAsa = -altura_painel + 50 - 10 + altura_jato - altura_ponta_jato;

    if ( posicaoAsa >= navio_v) {
        //asa esquerda
        if ((jato_h - largura_jato - largura_asa_jato) >= (navio_h - largura_navio) &&
            (jato_h - largura_jato - largura_asa_jato) <= (navio_h + largura_navio)) {

            bateu = true;
        }
        //asa direita
        else if ((jato_h + largura_jato + largura_asa_jato) >= (navio_h - largura_navio) &&
                 (jato_h + largura_jato + largura_asa_jato) <= (navio_h + largura_navio)) {

            bateu = true;
        }
    }
    if (posicaoFuselagem >= navio_v) {
        //fuselagem
        if ((jato_h - largura_jato) >= (navio_h - largura_navio) &&
            (jato_h + largura_jato) <= (navio_h + largura_navio)) {

            bateu = true;
        }
    }

}

void verificarBatidaHelicoptero(GLfloat heli_h, GLfloat heli_v) {
    GLfloat posicaoFuselagem = -altura_painel + 50 + altura_ponta_jato + altura_jato;
    GLfloat posicaoAsa = -altura_painel + 50 - 10 + altura_jato - altura_ponta_jato;
    
    //verificar batida corpo (asa e, fuselagem, asa d)
    if (posicaoFuselagem >= (heli_v - altura_helicoptero)) {
        //fuselagem
        if ((jato_h - largura_jato) >= (heli_h - largura_helicoptero) &&
            (jato_h + largura_jato) <= (heli_h + largura_helicoptero)) {
           
            bateu = true;
        }
    }
    if (posicaoAsa >= (heli_v - altura_helicoptero)) {

        //asa esquerda
        if ((jato_h - largura_jato - largura_asa_jato) >= (heli_h - largura_helicoptero) &&
            (jato_h - largura_jato - largura_asa_jato) <= (heli_h + largura_helicoptero)) {
           
            bateu = true;
        }
        //asa direita
        if ((jato_h + largura_jato + largura_asa_jato) >= (heli_h - largura_helicoptero) &&
            (jato_h + largura_jato + largura_asa_jato) <= (heli_h + largura_helicoptero)) {
           
            bateu = true;
        }

    }
    
    //verificar batida cauda (asa e, fuselagem, asa d)
    if (posicaoFuselagem >= heli_v) {
        //fuselagem
        if ((jato_h - largura_jato) >= (heli_h - largura_helicoptero - largura_cauda_helicoptero) &&
            (jato_h + largura_jato) <= (heli_h - largura_helicoptero)) {
           
            bateu = true;
        }
    }
    if (posicaoAsa >= heli_v) {

        //asa esquerda
        if ((jato_h - largura_jato - largura_asa_jato) >= (heli_h - largura_helicoptero - largura_cauda_helicoptero) &&
            (jato_h - largura_jato - largura_asa_jato) <= (heli_h - largura_helicoptero)) {
           
            bateu = true;
        }
        //asa direita
        if ((jato_h + largura_jato + largura_asa_jato) >= (heli_h - largura_helicoptero - largura_cauda_helicoptero) &&
            (jato_h + largura_jato + largura_asa_jato) <= (heli_h - largura_helicoptero)) {
          
            bateu = true;
        }
    } 
}

void verificarBatidaChanfro(GLfloat posicao_ponte) {

    //chanfro esquerdo - fuselagem
    if ((distancia_percorrida - altura_ponta_jato) > (posicao_ponte - (altura_chanfro / 2))) {

        if ((jato_h - largura_jato) >= -largura_parede && (jato_h + largura_jato) <= -largura_chanfro) bateu = true;

        if ((jato_h - largura_jato) >= largura_chanfro && (jato_h + largura_jato) <= largura_parede) bateu = true;

    }

    //chanfro esquerdo - asa
    if ((distancia_percorrida - altura_ponta_jato - 30) > (posicao_ponte - (altura_chanfro / 2))) {
        
        //esquerda
        if ((jato_h - largura_jato - largura_asa_jato) >= -largura_parede &&
            (jato_h - largura_jato - largura_asa_jato) <= -largura_chanfro) {

            bateu = true;
        }

        //direita
        if ((jato_h + largura_jato + largura_asa_jato) >= largura_chanfro &&
            (jato_h + largura_jato + largura_asa_jato) <= largura_parede) {

            bateu = true;
        }
    }
}

void verificarBatidaPonte(GLfloat posicao_ponte, GLboolean acertou) {
    if ((distancia_percorrida - altura_ponta_jato) > posicao_ponte) if (!acertou) bateu = true;
    verificarBatidaChanfro(posicao_ponte);
}

void verificarAcertoObjeto(GLfloat *objeto_h, GLfloat *objeto_v, GLfloat largura_objeto_e, GLfloat largura_objeto_d, GLboolean *gerou_objeto, GLboolean op) {
    if (tiro_h >= (*objeto_h - largura_objeto_e) && tiro_h <= (*objeto_h + largura_objeto_d)) {
        
        if (*objeto_v <= pos_tiro) {
            *gerou_objeto = false;
            *objeto_h = 0;
            *objeto_v = altura_janela;
            atirou = false;
            pontuacao += 500;
            pos_tiro = 0;
            if (op) {
                abastecendo = false;
                pontuacao += 500;
            }
        }
    }
}

void verificarAcertoPonte() {
    if (tiro_h >= (-largura_parede + largura_chanfro) && tiro_h <= (largura_parede - largura_chanfro)) {
        if (pos_tiro >= lugar_ponte) {
            
            if (primeira_ponte) {
                primeira_ponte = false;
                acertou_primeira_ponte = true;
            }
            else if (segunda_ponte) {
                segunda_ponte = false;
                acertou_segunda_ponte = true;
            }
            else if (terceira_ponte) {
                terceira_ponte = false;
                acertou_terceira_ponte = true;
            }

            atirou = false;
            checkpoint++;
            if (checkpoint == 3) {
                vitoria = true;
            }
        }
    }

}

void verificarTiro() {
    if (gerou_navio) {
        verificarAcertoObjeto(&navio_h, &navio_v, largura_navio, largura_navio, &gerou_navio, false);
    }
    if (gerou_navio_2) {
        verificarAcertoObjeto(&navio_h_2, &navio_v_2, largura_navio, largura_navio, &gerou_navio_2, false);
    }
    if (gerou_navio_3) {
        verificarAcertoObjeto(&navio_h_3, &navio_v_3, largura_navio, largura_navio, &gerou_navio_3, false);
    }
    if (gerou_helicoptero) {
        verificarAcertoObjeto(&heli_h, &heli_v, (largura_helicoptero + largura_cauda_helicoptero), largura_helicoptero, &gerou_helicoptero, false);
    } 
    if (gerou_helicoptero_2) {
        verificarAcertoObjeto(&heli_h_2, &heli_v_2, (largura_helicoptero + largura_cauda_helicoptero), largura_helicoptero, &gerou_helicoptero_2, false);
    }
    if (gerou_helicoptero_3) {
        verificarAcertoObjeto(&heli_h_3, &heli_v_3, (largura_helicoptero + largura_cauda_helicoptero), largura_helicoptero, &gerou_helicoptero_3, false);
    }
    if (gerou_posto_combustivel) {
        verificarAcertoObjeto(&posto_combustivel_h, &posto_combustivel_v, largura_posto_combustivel, largura_posto_combustivel, &gerou_posto_combustivel, true);
    }
    if (primeira_ponte || segunda_ponte || terceira_ponte) verificarAcertoPonte();
}

void verificarAbastecimento() {
    if (tiro_v >= posto_combustivel_v && tiro_v <= altura_posto_combustivel) {

        //fuselagem
        if (jato_h - largura_jato >= posto_combustivel_h - largura_posto_combustivel &&
            jato_h + largura_jato <= posto_combustivel_h + largura_posto_combustivel) {

            if (consumo_combustivel > 0) {
                consumo_combustivel -= 1;
            }
            abastecendo = true;
        }
        //asa esquerda
        else if (jato_h - largura_jato - largura_asa_jato >= posto_combustivel_h - largura_posto_combustivel &&
            jato_h - largura_jato - largura_asa_jato <= posto_combustivel_h + largura_posto_combustivel) {

            if (consumo_combustivel > 0) {
                consumo_combustivel -= 1;
            }
            abastecendo = true;
        }
        //asa direita
        else if (jato_h + largura_jato + largura_asa_jato >= posto_combustivel_h - largura_posto_combustivel &&
            jato_h + largura_jato + largura_asa_jato <= posto_combustivel_h + largura_posto_combustivel) {

            if (consumo_combustivel > 0) {
                consumo_combustivel -= 1;
            }
            abastecendo = true;
        }
        else {
            abastecendo = false;
        }

    }
}

void deslocarObjeto(GLfloat *objeto_h, GLfloat *objeto_v, GLfloat altura_objeto, GLboolean *gerou_objeto, GLboolean op) {
    if (*objeto_v > (-(altura_janela / 2) + altura_painel) - altura_objeto) {
        *objeto_v -= velocidade_deslocamento;
    }
    else {
        *gerou_objeto = false;
        *objeto_h = 0;
        *objeto_v = altura_janela;
        if (op) abastecendo = false;
    }
}

void moverObjeto(GLfloat *objeto_h, GLfloat largura_objeto, GLfloat deslocamento) {
    if (*objeto_h >= -largura_parede && *objeto_h <= largura_parede) {
        *objeto_h += deslocamento;
    }
}

void deslocarTiro() {
    if (pos_tiro < (altura_janela/2) + altura_tiro)  pos_tiro += 30;
    else {
        atirou = false;
        pos_tiro = 0;
    }
}

void consumirCombustivel() {
    if (consumo_combustivel <= 400) {
        if (!abastecendo) consumo_combustivel += velocidade_consumo_combustivel;
    }
    else bateu = true;
}

void atirar() {
    atirou = true;
    tiro_h = jato_h;
}

void gerarObjeto(GLfloat *objeto_h, GLboolean *gerou_objeto, GLfloat largura_objeto) {
    *objeto_h = randomizar(largura_objeto);
    *gerou_objeto = true;
}

void gerarCasa() {
    casa_h = randomizarCasa(largura_casa);
    gerou_casa = true;
}

void verificarAnimacaoHelicoptero() {
    if (gerou_helicoptero) {
        verificarBatidaHelicoptero(heli_h, heli_v);
        deslocarObjeto(&heli_h, &heli_v, altura_helicoptero, &gerou_helicoptero, false);
    }
    else {
        gerarObjeto(&heli_h, &gerou_helicoptero, (largura_helicoptero + largura_cauda_helicoptero));
    }

    if (gerou_helicoptero_2) {
        verificarBatidaHelicoptero(heli_h_2, heli_v_2);
        deslocarObjeto(&heli_h_2, &heli_v_2, altura_helicoptero, &gerou_helicoptero_2, false);
    }
    else {
        if (segunda_ponte || acertou_segunda_ponte) {
            gerarObjeto(&heli_h_2, &gerou_helicoptero_2, (largura_helicoptero + largura_cauda_helicoptero));
        }
    }

    if (gerou_helicoptero_3 || acertou_terceira_ponte) {
        verificarBatidaHelicoptero(heli_h_3, heli_v_3);
        deslocarObjeto(&heli_h_3, &heli_v_3, altura_helicoptero, &gerou_helicoptero_3, false);
    }
    else {
        if (terceira_ponte) {
            gerarObjeto(&heli_h_3, &gerou_helicoptero_3, (largura_helicoptero + largura_cauda_helicoptero));
        }
    }
}

void verificarAnimacaoNavio() {
    if (gerou_navio) {
        verificarBatidaNavio(navio_h, navio_v);
        deslocarObjeto(&navio_h, &navio_v, altura_navio, &gerou_navio, false);
    }
    else {
        gerarObjeto(&navio_h, &gerou_navio, largura_navio);
    }

    if (gerou_navio_2) {
        verificarBatidaNavio(navio_h_2, navio_v_2);
        deslocarObjeto(&navio_h_2, &navio_v_2, altura_navio, &gerou_navio_2, false);
    }
    else {
        if (segunda_ponte || acertou_segunda_ponte) {
            gerarObjeto(&navio_h_2, &gerou_navio_2, largura_navio);
       }
    }

    if (gerou_navio_3) {
        verificarBatidaNavio(navio_h_3, navio_v_3);
        deslocarObjeto(&navio_h_3, &navio_v_3, altura_navio, &gerou_navio_3, false);
    }
    else {
        if (terceira_ponte || acertou_terceira_ponte) {
            gerarObjeto(&navio_h_3, &gerou_navio_3, largura_navio);
       }
    }
}

void anima(GLint valor) {
    if (desenho_3d) {
        espera_3d += 10;
        rotacionar += 0.36;
        if (espera_3d >= 5000) {
            desenho_3d = false;
        }
    }
    else if (vitoria) {
        rotacionar += 0.36;
    }
    else {
        if (animacao) {
            distancia_animacao -= velocidade_deslocamento;
            if (distancia_animacao <= -(altura_ponte + altura_chanfro + altura_chanfro)) animacao = false;
        }
        else {
            if (bateu) {
                timer += 10;
                if (vidas > 0 && timer >= 3000) {
                    resetar();
                }
            }
            else {
                if (atirou) {
                    verificarTiro();
                    deslocarTiro();
                }
                
                verificarAnimacaoNavio();
                verificarAnimacaoHelicoptero();

                if (gerou_posto_combustivel) {
                    verificarAbastecimento();
                    deslocarObjeto(&posto_combustivel_h, &posto_combustivel_v, altura_posto_combustivel, &gerou_posto_combustivel, true);
                }
                else {
                    if (!primeira_ponte || !segunda_ponte || !terceira_ponte) gerarObjeto(&posto_combustivel_h, &gerou_posto_combustivel, largura_posto_combustivel);
                }

                if (gerou_casa) deslocarObjeto(&casa_h, &casa_v, (altura_casa - (altura_casa * 1.5)), &gerou_casa, false);
                else  gerarCasa();

                consumirCombustivel();
                distancia_percorrida += velocidade_deslocamento;


                if (distancia_percorrida < (lugar_primeira_ponte + altura_ponte) && !acertou_primeira_ponte) {
                    primeira_ponte = true;
                    lugar_ponte = lugar_primeira_ponte - distancia_percorrida;
                    verificarBatidaPonte(lugar_primeira_ponte, acertou_primeira_ponte);
                }
                else if (distancia_percorrida < (lugar_segunda_ponte + altura_ponte) && !acertou_segunda_ponte) {
                    segunda_ponte = true;
                    lugar_ponte = lugar_segunda_ponte - distancia_percorrida;
                    verificarBatidaPonte(lugar_segunda_ponte, acertou_segunda_ponte);
                }
                else if (distancia_percorrida < (lugar_terceira_ponte + altura_ponte) && !acertou_terceira_ponte) {
                    terceira_ponte = true;
                    lugar_ponte = lugar_terceira_ponte - distancia_percorrida;
                    verificarBatidaPonte(lugar_terceira_ponte, acertou_terceira_ponte);
                }
            }
        }

    }

    glutPostRedisplay();
    glutTimerFunc(10, anima, 1);
}

void tela(GLsizei largura, GLsizei altura) {
    if (altura == 0) altura = 1; // previne a divisão por zero
    GLfloat aspecto = (GLfloat)largura / (GLfloat)altura;
    glViewport(0, 0, largura, altura);
    glMatrixMode(GL_PROJECTION); // manipulando matriz de projeção
    glLoadIdentity(); // zerando a matriz
    gluPerspective(45, aspecto, 0.1, 100);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH); // acabamento com suavização
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // correcao de perspectiva
    glMatrixMode(GL_MODELVIEW);
}

void especiais(GLint tecla, GLint x, GLint y) {

    if (tecla == GLUT_KEY_RIGHT) {
        if ((jato_h + largura_jato + largura_asa_jato) < largura_parede) {

            if(!bateu) jato_h += largura_jato * 2;
        }
        else bateu = true;
    }
    else if (tecla == GLUT_KEY_LEFT) {
        if ((jato_h - largura_jato - largura_asa_jato) > -largura_parede) {

            if (!bateu) jato_h -= largura_jato * 2;
        }
        else bateu = true;
    }
    else if (tecla == GLUT_KEY_DOWN) {
        if (!animacao) {
            velocidade_deslocamento = 1.5;
            velocidade_consumo_combustivel = 0.05;
        }
    }
    else if (tecla == GLUT_KEY_UP) {
        if (!animacao) {
            velocidade_deslocamento = 6;
            velocidade_consumo_combustivel = 0.2;
        }
    }
    glutPostRedisplay();
}

void keyboardUp(GLint tecla, GLint x, GLint y) {

    if (tecla == GLUT_KEY_DOWN) {
        velocidade_deslocamento = 3;
        velocidade_consumo_combustivel = 0.1;
    }
    else if (tecla == GLUT_KEY_UP) {
        velocidade_deslocamento = 3;
        velocidade_consumo_combustivel = 0.1;
    }

    glutPostRedisplay();
}

void keyboard(unsigned char tecla, GLint x, GLint y) {

    if (tecla == SPACEBAR) atirar();
    else if (tecla == 'r') {
        desenho_3d = true;
        espera_3d = 0;
        rotacionar = 0;
        animacao = false;
        distancia_animacao = 0;
        timer = 0;
        vidas = 3;
        checkpoint = 0;
        pontuacao = 0;
        configuracaoLuz(ambiente, diffuse, specular, posicao);
        resetar();
    }

    glutPostRedisplay();
}

void display() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (desenho_3d) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 1, 1, 30);
        glClearColor(0.2666f, 0.4f, 0.9294f, 1);//azul 68,102,237
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambiente);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, alto_brilho);
        glMaterialfv(GL_FRONT, GL_EMISSION, padrao);

        desenharEntrada(rotacionar);
    }
    else if (vitoria) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, 1, 1, 30);

        GLfloat diffusenovo[] = { 1.0f, 0.7490f, 0, 1.0f };//amber 255,191,0
        GLfloat specularnovo[] = { 1.0f, 0.7490f, 0, 1.0f };//amber 255,191,0

        configuracaoLuz(ambiente, diffusenovo, specularnovo, posicao);

        glClearColor(0.2666f, 0.4f, 0.9294f, 1);//azul 68,102,237
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

       
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambiente);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, alto_brilho);
        glMaterialfv(GL_FRONT, GL_EMISSION, padrao);

        desenharVitoria(rotacionar);
    }
    else {
        glMatrixMode(GL_PROJECTION); // Tell opengl that we are doing project matrix work
        glLoadIdentity(); // Clear the matrix
        gluOrtho2D(0, largura_janela, 0, altura_janela); // Setup an Ortho view
        glTranslatef(largura_janela / 2, altura_janela / 2, 0.0f);
        glClearColor(0.1960f, 0.6078f, 0.5529f, 1.0f);
        glMatrixMode(GL_MODELVIEW); // Tell opengl that we are doing model matrix work. (drawing)
        glLoadIdentity(); // Clear the model matrix

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        if (animacao) {
            desenharAnimacaoEntrada();
        }
        else {


            if (gerou_posto_combustivel) {
                glPushMatrix();
                glTranslatef(posto_combustivel_h, posto_combustivel_v, 0);
                desenharPostoCombustivel();
                glPopMatrix();
            }


            if (gerou_navio) {
                glPushMatrix();
                glTranslatef(navio_h, navio_v, 0);
                desenharNavio();
                glPopMatrix();
            }
            if (gerou_navio_2) {
                glPushMatrix();
                glTranslatef(navio_h_2, navio_v_2, 0);
                desenharNavio();
                glPopMatrix();
            }
            if (gerou_navio_3) {
                glPushMatrix();
                glTranslatef(navio_h_3, navio_v_3, 0);
                desenharNavio();
                glPopMatrix();
            }


            if (gerou_helicoptero) {
                glPushMatrix();
                glTranslatef(heli_h, heli_v, 0);
                desenharHelicoptero();
                glPopMatrix();

            }
            if (gerou_helicoptero_2) {
                glPushMatrix();
                glTranslatef(heli_h_2, heli_v_2, 0);
                desenharHelicoptero();
                glPopMatrix();
            }
            if (gerou_helicoptero_3) {
                glPushMatrix();
                glTranslatef(heli_h_3, heli_v_3, 0);
                desenharHelicoptero();
                glPopMatrix();
            }


            if (atirou) {
                glPushMatrix();
                glTranslatef(jato_h, pos_tiro, 0);
                desenharTiro();
                glPopMatrix();
            }

            glPushMatrix();
            desenharParedes();
            glPopMatrix();

            if (!acertou_primeira_ponte) {
                glPushMatrix();
                glTranslatef(0, lugar_primeira_ponte - distancia_percorrida, 0);
                desenharPonte();
                glPopMatrix();
            }
            if (!acertou_segunda_ponte) {
                glPushMatrix();
                glTranslatef(0, lugar_segunda_ponte - distancia_percorrida, 0);
                desenharPonte();
                glPopMatrix();
            }
            if (!acertou_terceira_ponte) {
                glPushMatrix();
                glTranslatef(0, lugar_terceira_ponte - distancia_percorrida, 0);
                desenharPonte();
                glPopMatrix();
            }


            glPushMatrix();
            glTranslatef(0, lugar_primeira_ponte - distancia_percorrida, 0);
            desenharEstradaPonte();
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0, lugar_segunda_ponte - distancia_percorrida, 0);
            desenharEstradaPonte();
            glPopMatrix();

            glPushMatrix();
            glTranslatef(0, lugar_terceira_ponte - distancia_percorrida, 0);
            desenharEstradaPonte();
            glPopMatrix();

            if (gerou_casa) {
                glPushMatrix();
                glTranslatef(casa_h, casa_v, 0);
                desenharCasa();
                glPopMatrix();
            }

            glPushMatrix();
            desenharPainel();
            glPopMatrix();

            glPushMatrix();
            glTranslatef(jato_h, -altura_painel + 50, 0);
            desenharJato();
            glPopMatrix();

        }
    }
   
    glutSwapBuffers();
}

GLint main(GLint argc, char** argv) {

    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));
    
    glutInit(&argc, argv);  // controla se o sistema operacional tem suporte a janelas.
    glutInitDisplayMode(GLUT_DOUBLE);  // quantidade de buffer de cores e que o padrao de cores é RGB ou RGBA
    glutInitWindowSize(largura_janela, altura_janela);  // tamanho da janela
    glutInitWindowPosition(300, 100); // posicao inicial da janela
    glutCreateWindow("Artur Pasetto - Jogo Atari");   /// cria a janela

    configuracaoLuz(ambiente, diffuse, specular, posicao);

    glutReshapeFunc(tela);
    glutDisplayFunc(display);
    glutKeyboardFunc(&keyboard);
    glutSpecialUpFunc(&keyboardUp);
    glutSpecialFunc(&especiais);
    glutTimerFunc(10, anima, 1);
    glutMainLoop();

    return EXIT_SUCCESS;
}
