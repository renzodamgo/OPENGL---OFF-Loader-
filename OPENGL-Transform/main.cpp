#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <sstream>
#include <vector>



using namespace std;

vector<float> verticesVec;
vector<unsigned int> carasVec;
void loadOFF(string ruta) {
    verticesVec.clear();
    carasVec.clear();

    ifstream file(ruta);
    string line;

    int lineasVertices, lineasCaras, lineasAristas;
    float x, y, z;
    float a, b, c, d;

    if (file.is_open()){
        getline(file, line); // OFF
        file >> lineasVertices >> lineasCaras >> lineasAristas;
        
        for(int i = 0; i < lineasVertices; i++){
            file >> x >> y >> z;
            verticesVec.push_back(x);
            verticesVec.push_back(y);
            verticesVec.push_back(z);
            cout<<x<<" "<< y<<" "<<z<< endl;
        }
        
        for(int i = 0; i < lineasCaras; i++){
            file >> a >> b >> c >> d;
            //carasVec.push_back(a);
            cout<<b<<" "<< c<<" "<<d<< endl;
            carasVec.push_back(b);
            carasVec.push_back(c);
            carasVec.push_back(d);
        }
    }

    file.close();
}


void framebuffer_tamanho_callback(GLFWwindow* ventana, int ancho, int alto) {
    glViewport(0, 0, ancho, alto);
}
void procesarEntrada(GLFWwindow* ventana) {
    if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(ventana, true);
}

const unsigned int ANCHO = 600;
const unsigned int ALTO = 600;

class CProgramaShaders {
    GLuint idPrograma;
public:
    CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
        //Variables para leer los archivos de cÃ³digo
        string strCodigoShaderVertice;
        string strCodigoShaderFragmento;
        ifstream pArchivoShaderVertice;
        ifstream pArchivoShaderFragmento;
        //Mostramos excepciones en caso haya
        pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
        pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
//        try {
        //Abriendo los archivos de cÃ³digo de los shader
        pArchivoShaderVertice.open(rutaShaderVertice);
        cout << rutaShaderVertice;
        pArchivoShaderFragmento.open(rutaShaderFragmento);
        //Leyendo la informaciÃ³n de los archivos
        stringstream lectorShaderVertice, lectorShaderFragmento;
        lectorShaderVertice << pArchivoShaderVertice.rdbuf();
        lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
        //Cerrando los archivos
        pArchivoShaderVertice.close();
        pArchivoShaderFragmento.close();
        //Pasando la informaciÃ³n leida a string
        strCodigoShaderVertice = lectorShaderVertice.str();
        strCodigoShaderFragmento = lectorShaderFragmento.str();
//        }
//        catch (ifstream::failure) {
//            cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n";
//        }
        const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
        const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
        //Asociando y compilando los cÃ³digos de los shader
        GLuint idShaderVertice, idShaderFragmento;
        //Shader de VÃ©rtice
        idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
        glCompileShader(idShaderVertice);
        verificarErrores(idShaderVertice, "VÃ©rtice");
        //Shader de Fragmento
        idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, NULL);
        glCompileShader(idShaderFragmento);
        verificarErrores(idShaderVertice, "Fragmento");
        //Programa de Shaders
        this->idPrograma = glCreateProgram();
        glAttachShader(this->idPrograma, idShaderVertice);
        glAttachShader(this->idPrograma, idShaderFragmento);
        glLinkProgram(this->idPrograma);
        //Ahora ya podemos eliminar los programas de los shaders
        glDeleteShader(idShaderVertice);
        glDeleteShader(idShaderFragmento);
    }
    ~CProgramaShaders() {
        glDeleteProgram(this->idPrograma);
    }
    void usar() const {
        glUseProgram(this->idPrograma);
    }
    void setVec3(const string& nombre, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z);
    }
    void setVec3(const string& nombre, const glm::vec3 &valor) const {
        glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
    }
    void setMat4(const string& nombre, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
private:
    void verificarErrores(GLuint identificador, string tipo) {
        GLint ok;
        GLchar log[1024];

        if (tipo == "Programa") {
            glGetProgramiv(this->idPrograma, GL_LINK_STATUS, &ok);
            if (!ok) {
                glGetProgramInfoLog(this->idPrograma, 1024, NULL, log);
                cout << "Error de enlace con el programa: " << log << "\n";
            }
        }
        else {
            glGetShaderiv(identificador, GL_COMPILE_STATUS, &ok);
            if (!ok) {
                glGetShaderInfoLog(identificador, 1024, nullptr, log);
                cout << "Error de compilacion con el Shader de " << tipo << ": " << log << "\n";
            }
        }
    }
};

int main() {
    
    
    loadOFF("/Users/damian-mac/Cursos/Computación Grafica/OPENGL-Transform/OPENGL-Transform/avion.off");

    float vertices[verticesVec.size()];
    copy(verticesVec.begin(), verticesVec.end(), vertices);
    long totalVertices = sizeof(vertices)/sizeof(float);

    unsigned int indices[carasVec.size()];
    copy(carasVec.begin(), carasVec.end(), indices);
    long totalIndices = sizeof(indices)/sizeof(unsigned int);
    
  
    
    
    //Inicializar glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //Creando la ventana
    GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL, NULL);
    if (ventana == NULL) {
        cout << "Problemas al crear la ventana\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(ventana);
    glfwSetFramebufferSizeCallback(ventana, framebuffer_tamanho_callback);

    //Cargar Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Problemas al cargar GLAD\n";
        return -1;
    }

    CProgramaShaders programa_shaders = CProgramaShaders("/Users/damian-mac/Cursos/Computación Grafica/OPENGL-Transform/OPENGL-Transform/GLSL/codigo.vs", "/Users/damian-mac/Cursos/Computación Grafica/OPENGL-Transform/OPENGL-Transform/GLSL/codigo.fs");

//    Definiendo la geometrÃ­a de la figura en funciÃ³n de vÃ©rtices
//    float vertices[] = {
//        -0.2, -0.2, 1.0,//inferior izquierdo
//         0.2, -0.2, 0.0, //inferior derecho
//        -0.2,  0.2, 0.0,//superior izquierdo
//         0.2,  0.2, 0.0, //superior derecho
//    };
//
//    unsigned int indices[] = {
//        0, 1, 2,
//        1, 3, 2
//    };

    //Enviando la geometrÃ­a al GPU: Definiendo los buffers (Vertex Array Objects y Vertex Buffer Objects)
    unsigned int id_array_vertices, id_array_buffers, id_element_buffer;
    glGenVertexArrays(1, &id_array_vertices);
    glGenBuffers(1, &id_array_buffers);
    glGenBuffers(1, &id_element_buffer);

    //anexando los buffers para su uso en la tarjeta grÃ¡fica
    glBindVertexArray(id_array_vertices);

    //Anexando buffers y cargandolos con los datos
    glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Indicando las especificaciones de los atributos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    float dx = 0.01;
    float dy = 0.01;
    float x = 0.5;
    float y =0;
    while (!glfwWindowShouldClose(ventana)) {
        procesarEntrada(ventana);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        programa_shaders.usar();
        // Transformacion holografica
        
        glm::mat4 transformacion = glm::mat4(1.0);
        
        
        
        transformacion = glm::translate(transformacion, glm::vec3(x, y, 0));
        
        transformacion = glm::rotate(transformacion, (float)glfwGetTime(),glm::vec3(0,0,1));
        
        transformacion = glm::scale(transformacion, glm::vec3(glm::sin((float)glfwGetTime())/10, glm::sin((float)glfwGetTime())/10,glm::sin((float)glfwGetTime())/10));
        
        //transformacion = glm::scale(transformacion, glm::vec3(0.1, 0.1, 0.1));
        
        //Para que de vueltas:
        //programa_shaders.setVec3("traslacion", glm::vec3(glm::sin((float)glfwGetTime()), glm::cos((float)glfwGetTime()), 0.0));
        programa_shaders.setVec3("color", glm::vec3(-glm::sin((float)glfwGetTime()),glm::sin((float)glfwGetTime())+0.5,-glm::sin((float)glfwGetTime())));
        programa_shaders.setMat4("transformacion", transformacion);
        //programa_shaders.setVec3("traslacion", glm::vec3(x, y, 0.0));
        glBindVertexArray(id_array_vertices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 22321*3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(ventana);
        glfwPollEvents();
        
        if (x > 0.8 +  abs(0.2*cos((float)glfwGetTime()))|| x< -(0.8 + abs(0.2*cos((float)glfwGetTime())))){
            dx = dx*-1;
        }
        if (y > 0.8 +  abs(0.2*cos((float)glfwGetTime()))|| y< -(0.8 + abs(0.2*cos((float)glfwGetTime())))){
            dy = dy*-1;
        }
        
        x += dx;
        y += dy;
        //cout<<x<< " "<<abs(0.2*cos((float)glfwGetTime()))<<endl;
        
    }
    //Liberando memoria
    glDeleteVertexArrays(1, &id_array_vertices);
    glDeleteBuffers(1, &id_array_buffers);
    glfwTerminate();
    return 0;
}
