#define DEV

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <gl/GL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
#include <algorithm>

const int WIDTH = 700;
const int HEIGHT = 700;
const char TITLE[] = "Asteroids";



unsigned int create_vao(std::vector<float> verts, bool texCoords);
void particle_explode(int count, glm::vec2 loc, unsigned int shader, glm::vec3 color, glm::vec2 direction, float weight, float velocity);



class Asteroid {

	public:
		
		std::vector<float> point = {0.0f, 0.0f, 0.5f};
		glm::vec2 loc;
		glm::mat4 pos;

		glm::vec2 vel;

		bool alive = true;

		unsigned int VAO;
		unsigned int shaderProgram;

		float radius;
		int segments;

		void set_pos(glm::vec2 newPos) {
			loc = newPos;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

		void screen_wrap() {
			if (loc.x > 1.1+radius) {set_pos(glm::vec2(-1.05-radius, loc.y));}
			if (loc.x < -1.1-radius) {set_pos(glm::vec2(1.05+radius, loc.y));}
			if (loc.y > 1.1+radius) {set_pos(glm::vec2(loc.x, -1.05-radius));}
			if (loc.y < -1.1-radius) {set_pos(glm::vec2(loc.x, 1.05+radius));}
		}

		void change_pos(glm::vec2 changePos) {
			loc[0] += changePos[0];
			loc[1] += changePos[1];

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0f));
		}

		void move_vel(float deltaTime) {
			change_pos(vel * deltaTime);
		}

		void draw() {
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);

			glUniform1f(glGetUniformLocation(shaderProgram, "radius"), radius);
			glUniform1i(glGetUniformLocation(shaderProgram, "segments"), (int)segments);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(pos));

			glDrawArrays(GL_POINTS, 0, point.size()/3);
		}

		template <typename T>
		glm::vec2 collision(T target, float threshold) {
			glm::vec2 direction = (glm::normalize(glm::vec2((target.loc-loc).x, (target.loc-loc).y)));
			if (sqrt(pow(((target.loc-loc).x),2)+pow(((target.loc-loc).y),2))-threshold <= radius) {return direction;}
			else {return glm::vec2(0.0, 0.0);}
		}

		Asteroid(glm::vec2 location, unsigned int shaders, float setRadius, int setSegments, glm::vec2 initVel) {
			loc = location;
			shaderProgram = shaders;
			VAO = create_vao(point, false);
			radius = setRadius;
			segments = setSegments;

			vel = initVel;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

		void hit(std::vector<Asteroid>* asteroidList, unsigned int asteroidShaderProgram) {
			alive = false;
			float speedUp = 1.6;

			if (radius > 0.07) {
				Asteroid split1(loc, asteroidShaderProgram, radius/2, rand()%8+5, glm::vec2(vel.x*speedUp + glm::cross(glm::vec3(vel, 0.0), glm::vec3(0.0, 0.0, 1.0)).x*0.5, vel.y*speedUp + glm::cross(glm::vec3(vel, 0.0), glm::vec3(0.0, 0.0, 1.0)).y*0.5));
				asteroidList -> push_back(split1);
				Asteroid split2(loc, asteroidShaderProgram, radius/2, rand()%8+5, glm::vec2(vel.x*speedUp + glm::cross(glm::vec3(vel, 0.0), glm::vec3(0.0, 0.0, -1.0)).x*0.5, vel.y*speedUp + glm::cross(glm::vec3(vel, 0.0), glm::vec3(0.0, 0.0, -1.0)).y*0.5));
				asteroidList -> push_back(split2);
			}
		}

};



class Bullet {

	public:

		std::vector<float> mesh = {
				-0.013f, -0.013f, 1.0f,
				0.013f, -0.013f, 1.0f,
				0.0f, 0.013f, 1.0f,
			};

		glm::vec2 loc;
		glm::mat4 pos;

		bool alive = true;
		float spawnTime;

		float rot = 0;
		glm::vec2 vel;

		glm::vec3 color;

		unsigned int VAO;
		unsigned int shaderProgram;

		void set_pos(glm::vec2 newPos) {
			loc = newPos;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

		void screen_wrap() {
			if (loc.x > 1.1) {set_pos(glm::vec2(-1.05, loc.y));}
			if (loc.x < -1.1) {set_pos(glm::vec2(1.05, loc.y));}
			if (loc.y > 1.1) {set_pos(glm::vec2(loc.x, -1.05));}
			if (loc.y < -1.1) {set_pos(glm::vec2(loc.x, 1.05));}
		}

		void change_pos(glm::vec2 changePos) {
			loc[0] += changePos[0];
			loc[1] += changePos[1];

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0f));
			pos = glm::rotate(pos, rot, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void move_vel(float deltaTime) {
			change_pos(vel * deltaTime);
		}

		void change_rotate(float angle) {
			rot += angle;
		}

		void draw() {
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(pos));
			glUniform3fv(glGetUniformLocation(shaderProgram, "pColor"), 1, glm::value_ptr(color));

			glDrawArrays(GL_LINE_LOOP, 0, mesh.size()/3);
		}

		void despawn(float time) {
			if (time-1.2 > spawnTime) {alive = false;}
		}

		Bullet(glm::vec3 setColor, glm::vec2 location, unsigned int shaders, glm::vec2 initialVel) {
			loc = location;
			shaderProgram = shaders;
			VAO = create_vao(mesh, false);

			color = setColor;

			spawnTime = glfwGetTime();

			vel = initialVel;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

};



class Particle {

	public:

		std::vector<float> mesh = {
				-0.01f, -0.01f, 1.0f,
				0.01f, -0.01f, 1.0f,
				0.0f, 0.01f, 1.0f,
			};

		glm::vec2 loc;
		glm::mat4 pos;

		float rot = 0;
		glm::vec2 vel;

		glm::vec3 color;

		unsigned int VAO;
		unsigned int shaderProgram;

		void set_pos(glm::vec2 newPos) {
			loc = newPos;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

		void screen_wrap() {
			if (loc.x > 1.1) {set_pos(glm::vec2(-1.05, loc.y));}
			if (loc.x < -1.1) {set_pos(glm::vec2(1.05, loc.y));}
			if (loc.y > 1.1) {set_pos(glm::vec2(loc.x, -1.05));}
			if (loc.y < -1.1) {set_pos(glm::vec2(loc.x, 1.05));}
		}

		void change_pos(glm::vec2 changePos) {
			loc[0] += changePos[0];
			loc[1] += changePos[1];

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0f));
			pos = glm::rotate(pos, rot, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void move_vel(float deltaTime) {
			change_pos(vel * deltaTime);
		}

		void change_rotate(float angle) {
			rot += angle;
		}

		void draw() {
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(pos));
			glUniform3fv(glGetUniformLocation(shaderProgram, "pColor"), 1, glm::value_ptr(color));

			glDrawArrays(GL_LINE_LOOP, 0, mesh.size()/3);
		}

		static int particleCount;

		static void particleInc(bool dir) {
			if (dir) 	{particleCount++;}
			else 		{particleCount--;}
		}

		Particle(glm::vec3 setColor, glm::vec2 location, unsigned int shaders, glm::vec2 initialVel) {
			loc = location;
			shaderProgram = shaders;
			VAO = create_vao(mesh, false);

			color = setColor;

			vel = initialVel;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));

			particleInc(true);
		}

		~Particle() {
			particleInc(false);
			// std::cout << particleCount << std::endl;
		}

};



class Player {

	public:
		
		bool alive = true;

		std::vector<float> mesh = {
				-0.04f, -0.05f, 0.0f,
				0.04f, -0.05f, 0.0f,
				0.0f, 0.05f, 0.0f,
			};

		glm::vec2 loc;
		glm::mat4 pos;

		float lastShot;

		float rot = 0;
		glm::vec2 vel;

		unsigned int VAO;
		unsigned int shaderProgram;

		void set_pos(glm::vec2 newPos) {
			loc = newPos;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

		void change_pos(glm::vec2 changePos) {
			loc[0] += changePos[0];
			loc[1] += changePos[1];

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0f));
			pos = glm::rotate(pos, rot, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void move_vel(float deltaTime) {
			change_pos(vel * deltaTime);
		}

		void change_rotate(float angle) {
			rot += angle;
		}

		void shoot(std::vector<Bullet>* bulletList, unsigned int bulletShader, glm::vec2 front) {
			Bullet newBullet(glm::vec3(0.0f, 1.0f, 1.0f), loc, bulletShader, glm::vec2(front.x*1.7, front.y*1.7)+vel);
			newBullet.change_rotate(rot);
			bulletList -> push_back(newBullet);
		}

		int frameCount = 0;
		float setDeltaTime = 0;

		void controller(GLFWwindow* window, float deltaTime, unsigned int thrustShader, unsigned int bulletShader, std::vector<Bullet>* bulletList) {
			glm::vec2 front = glm::normalize(glm::cross(glm::vec3(cos(rot), sin(rot), 0.0), glm::vec3(0.0, 0.0, -1.0)));

			if (frameCount == 0) {setDeltaTime = deltaTime;}

			const float acc = 0.6;
			const float decel = 0.08;
			const float rotSpeed = 3.0;
			
			if (glfwGetKey(window, GLFW_KEY_W)) {
				vel += glm::vec2(front.x*(acc * deltaTime), front.y*(acc * deltaTime));
				if (frameCount % (int)(0.04/setDeltaTime) == 0) {
					particle_explode(2, glm::vec2(loc - (glm::vec2(front.x*0.05, front.y*0.05))), thrustShader, glm::vec3(1.0, 0.2, 0.0), -front, 40, 35);
				}
			}
			if (glfwGetKey(window, GLFW_KEY_A)) {change_rotate(rotSpeed * deltaTime);}
			if (glfwGetKey(window, GLFW_KEY_D)) {change_rotate(-(rotSpeed * deltaTime));}
			if (glfwGetKey(window, GLFW_KEY_SPACE)) {
				if (bulletList -> size() < 7) {
					if (glfwGetTime() - lastShot > 0.1) {
						shoot(bulletList, bulletShader, front);
						lastShot = glfwGetTime();
					}
				}
			}

			glm::vec2 velNormalized = glm::normalize(vel);
			if (vel.x > 0) {vel.x -= velNormalized.x * (decel * deltaTime);}
			if (vel.y > 0) {vel.y -= velNormalized.y * (decel * deltaTime);}
			if (vel.x < 0) {vel.x += velNormalized.x * (-decel * deltaTime);}
			if (vel.y < 0) {vel.y += velNormalized.y * (-decel * deltaTime);}
			
			frameCount += 1;
			if (frameCount > 100) {frameCount = 0;}
		}

		void screen_wrap() {
			if (loc.x > 1.1) {set_pos(glm::vec2(-1.05, loc.y));}
			if (loc.x < -1.1) {set_pos(glm::vec2(1.05, loc.y));}
			if (loc.y > 1.1) {set_pos(glm::vec2(loc.x, -1.05));}
			if (loc.y < -1.1) {set_pos(glm::vec2(loc.x, 1.05));}
		}

		void draw() {
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(pos));

			glDrawArrays(GL_LINE_LOOP, 0, mesh.size()/3);
		}

		Player(glm::vec2 location, unsigned int shaders, glm::vec2 initialVel) {
			loc = location;
			shaderProgram = shaders;
			VAO = create_vao(mesh, false);

			vel = initialVel;

			lastShot = glfwGetTime()-3;

			pos = glm::mat4(1.0f);
			pos = glm::translate(pos, glm::vec3(loc, 0.0));
		}

};



// unsigned int create_texture(std::string imagePath, GLenum imageFormat) {

//     stbi_set_flip_vertically_on_load(true);

//     int width, height, colorChannelCount;
//     unsigned char *image = stbi_load(imagePath.c_str(), &width, &height, &colorChannelCount, 0);

//     unsigned int texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, image);
//     glGenerateMipmap(GL_TEXTURE_2D);

//     stbi_image_free(image);

//     return texture;

// }



class ScreenQuad {

	public:

		std::vector<float> mesh = {
				-1.0f, -1.0f, 0.0f,		0.0, 0.0,
				-1.0f, 1.0f, 0.0f,		0.0, 1.0,
				1.0f, 1.0f, 0.0f,		1.0, 1.0,

				-1.0f, -1.0f, 0.0f,		0.0, 0.0,
				1.0f, -1.0f, 0.0f,		1.0, 0.0,
				1.0f, 1.0f, 0.0f,		1.0, 1.0,
			};

		unsigned int VAO;
		unsigned int shaderProgram;

		void draw(unsigned int texture) {
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);
			glBindTexture(GL_TEXTURE_2D, texture);

			glDrawArrays(GL_TRIANGLES, 0, mesh.size()/5);
		}

		ScreenQuad(unsigned int shaders) {
			shaderProgram = shaders;
			VAO = create_vao(mesh, true);
		}

};



// class Character {

// 	public:

// 		std::vector<float> mesh = {
// 				-0.1f, -0.1f, 0.0f,		0.0, 0.0,
// 				-0.1f, 0.1f, 0.0f,		0.0, 1.0,
// 				0.1f, 0.1f, 0.0f,		1.0, 1.0,

// 				-0.1f, -0.1f, 0.0f,		0.0, 0.0,
// 				0.1f, -0.1f, 0.0f,		1.0, 0.0,
// 				0.1f, 0.1f, 0.0f,		1.0, 1.0,
// 			};

// 		unsigned int VAO;
// 		unsigned int shaderProgram;
// 		unsigned int texture;

// 		glm::vec2 loc;
// 		glm::mat4 pos;

// 		void set_pos(glm::vec2 newPos) {
// 			loc = newPos;

// 			pos = glm::mat4(1.0f);
// 			pos = glm::translate(pos, glm::vec3(loc, 0.0));
// 		}

// 		void draw() {
// 			glBindVertexArray(VAO);
// 			glUseProgram(shaderProgram);
// 			glBindTexture(GL_TEXTURE_2D, texture);

// 			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "world"), 1, GL_FALSE, glm::value_ptr(pos));

// 			glDrawArrays(GL_TRIANGLES, 0, mesh.size()/5);
// 		}

// 		Character(unsigned int shaders, glm::vec2 initPos) {
// 			shaderProgram = shaders;
// 			VAO = create_vao(mesh, true);

// 			texture = create_texture("../resources/fontmaps/whiteQueen.png", GL_RGBA);

// 			set_pos(initPos);
// 		}

// };



void framebuffer_size_callback(GLFWwindow* window, int w, int h) {

	glViewport(0, 0, w, h);

}



unsigned int create_vao(std::vector<float> verts, bool texCoords) {

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	if (!texCoords) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
		glEnableVertexAttribArray(0);
	}
	else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float)*3));
		glEnableVertexAttribArray(1);
	}

	return VAO;

}



std::string import_shader_source(std::string inputPath) {

    std::ifstream input(inputPath);
    std::string inputText = "";

    for(std::string line; getline(input, line);) {
        inputText = inputText + "\n" + line;
    }

    inputText += "\0";

    input.close();
    input.clear();

    return inputText;

}



unsigned int create_shader(std::string vertexShaderPath, std::string geometryShaderPath, std::string fragmentShaderPath, bool incGeoShader) {

    std::string vertexShaderSourceString = import_shader_source(vertexShaderPath);
    const char* vertexShaderSource = vertexShaderSourceString.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    #ifdef DEV
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::cout << vertexShaderSource << "\n" << std::endl;
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR VERTEX SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        };
    #endif


	unsigned int geometryShader;
	if (incGeoShader) {
		std::string geometryShaderSourceString = import_shader_source(geometryShaderPath);
		const char* geometryShaderSource = geometryShaderSourceString.c_str();
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
		glCompileShader(geometryShader);

		#ifdef DEV
			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				std::cout << geometryShaderSource << "\n" << std::endl;
				glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
				std::cout << "ERROR GEOMETRY SHADER COMPILATION FAILED\n" << infoLog << std::endl;
			};
		#endif
	}

    std::string fragmentShaderSourceString = import_shader_source(fragmentShaderPath);
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    #ifdef DEV
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::cout << fragmentShaderSource << "\n" << std::endl;
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR FRAGMENT SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        };
    #endif

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
	if (incGeoShader) {
		glAttachShader(shaderProgram, geometryShader);
	}
	glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}



std::vector<Particle*> particleList;
std::vector<Bullet> bulletList;



void particle_explode(int count, glm::vec2 loc, unsigned int shader, glm::vec3 color, glm::vec2 direction, float weight, float velocity) {
	for (int j=0; j < count; j++) {
		float multiply = weight;
		float divide = velocity;
		float low = 10;
		float high = 10;
		float velX = 0; while (velX == 0) {velX = ((direction.x*multiply)-low)/divide + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((((direction.x*multiply)+high)/divide)-((direction.x*multiply)-low)/divide)));}
		float velY = 0; while (velY == 0) {velY = ((direction.y*multiply)-low)/divide + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((((direction.y*multiply)+high)/divide)-((direction.y*multiply)-low)/divide)));}
		Particle* newParticle = new Particle(color, loc, shader, glm::vec2(velX, velY));
		newParticle -> change_rotate(0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(360-0))));
		particleList.push_back(newParticle);
	}
} 



void kill_particle(std::vector<Particle*>* particleList) {
	std::vector<Particle*> newParticleList = {};
	for (int i=0; i < particleList -> size(); i++) {
		if (!(particleList -> at(i) -> loc.x < -1.1 || particleList -> at(i) -> loc.x > 1.1 || particleList -> at(i) -> loc.y < -1.1 || particleList -> at(i) -> loc.y > 1.1)) {
			newParticleList.push_back(particleList -> at(i));
		}
		else {
			//std::cout << "else reached" << std::endl;
			delete particleList->at(i);
		}
	}

	*particleList = newParticleList;

	// std::cout << newParticleList.size() << std::endl;

	/*for (int j = 0; j < newParticleList.size(); j++) {
		delete newParticleList[j];
	}*/
}



void kill_bullet(std::vector<Bullet>* bulletList) {
	std::vector<Bullet> newBulletList = {};
	for (int i=0; i < bulletList -> size(); i++) {
		if (bulletList -> at(i).alive) {
			newBulletList.push_back(bulletList -> at(i));
		}
	}

	*bulletList = newBulletList;
}



void kill_asteroid(std::vector<Asteroid>* asteroidList) {
	std::vector<Asteroid> newAsteroidList = {};
	for (int i=0; i < asteroidList -> size(); i++) {
		if (asteroidList -> at(i).alive) {
			newAsteroidList.push_back(asteroidList -> at(i));
		}
	}

	*asteroidList = newAsteroidList;
}



void spawn_level(std::vector<Asteroid>* asteroidList, unsigned int asteroidShaderProgram, Player player, int noOfAsteroids) {
	for (int i=0; i < noOfAsteroids; i++) {
		
		const float DIST = 0.6;

		float randX = player.loc.x;
		float randY = player.loc.y;
		while (fabs(player.loc.x - randX) < DIST && fabs(player.loc.y - randY) < DIST) {
			randX = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1+1)));
			randY = -1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1+1)));
		}
		
		float randRadius = 0.1 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.2-0.1)));
		
		float randVelX = 0;
		float randVelY = 0;
		while (randVelX == 0) {randVelX = -0.3 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.3+0.3)));}
		while (randVelY == 0) {randVelY = -0.3 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(0.3+0.3)));}
		
		Asteroid newAsteroid(glm::vec2(randX, randY), asteroidShaderProgram, randRadius, rand()%8+5, glm::vec2(randVelX, randVelY));
		asteroidList -> push_back(newAsteroid);
	}
}



GLFWwindow* window;
unsigned int pass1;
unsigned int pass1Tex;
unsigned int pass2;
unsigned int pass2Tex;
unsigned int postPass1ShaderProgram;
unsigned int postPass2ShaderProgram;
unsigned int asteroidShaderProgram;
unsigned int playerShaderProgram;
unsigned int particleShaderProgram;
// unsigned int characterShaderProgram;
std::vector<Asteroid> asteroidList;

float deathDelay = 0.0; 



bool start_level(float* currentFrame, float* deltaTime, float* lastFrame, Player* player, ScreenQuad postPass1, ScreenQuad postPass2, int lvlNum) {

	spawn_level(&asteroidList, asteroidShaderProgram, *player, 3+lvlNum);

	while (!glfwWindowShouldClose(window)) {

		glBindFramebuffer(GL_FRAMEBUFFER, pass1);

		glClear(GL_COLOR_BUFFER_BIT);

		*currentFrame = glfwGetTime();
		*deltaTime = *currentFrame - *lastFrame;
		*lastFrame = *currentFrame;

		if (player -> alive) {
			player -> controller(window, *deltaTime, particleShaderProgram, particleShaderProgram, &bulletList);
			player -> move_vel(*deltaTime);
			player -> screen_wrap();
		}

		for (int i=0; i < asteroidList.size(); i++) {
			asteroidList[i].move_vel(*deltaTime);
			asteroidList[i].draw();
			asteroidList[i].screen_wrap();

			// Player collision
			if (player -> alive) {
				glm::vec2 direction = asteroidList[i].collision<Player>(*player, 0.05);
				if (direction != glm::vec2(0.0, 0.0)) {
					player -> alive = false;
					asteroidList[i].hit(&asteroidList, asteroidShaderProgram);

					float playerSpeed = sqrt(pow(((player -> vel).x),2)+pow(((player -> vel).y),2));
					if (playerSpeed > 2.3) {playerSpeed = 2.3;}
					float weight = pow(playerSpeed, 3);
					float velocity = 10-pow(playerSpeed, 2);
					particle_explode(50, player -> loc, particleShaderProgram, glm::vec3(0.0, 1.0, 0.0), direction, weight, velocity);
					particle_explode(40, asteroidList[i].loc, particleShaderProgram, glm::vec3(1.0, 1.0, 1.0), -direction + asteroidList[i].vel, weight, velocity);
				}
			}

			// Bullet collision
			for (int j=0; j < bulletList.size(); j++) {
				if (bulletList[j].alive) {
					glm::vec2 direction = asteroidList[i].collision<Bullet>(bulletList[j], 0.0013);
					if (direction != glm::vec2(0.0, 0.0)) {
						bulletList[j].alive = false;
						asteroidList[i].hit(&asteroidList, asteroidShaderProgram);

						float bulletSpeed = sqrt(pow(((bulletList[j].vel).x),2)+pow(((bulletList[j].vel).y),2));
						if (bulletSpeed > 2.3) {bulletSpeed = 2.3;}
						float weight = pow(bulletSpeed/1.2, 3);
						float velocity = 10-pow(bulletSpeed/1.2, 2);
						particle_explode(40, asteroidList[i].loc, particleShaderProgram, glm::vec3(1.0, 1.0, 1.0), -direction + asteroidList[i].vel, weight, velocity);
					}
				}
			}
		}

		kill_asteroid(&asteroidList);

		for (int i=0; i < particleList.size(); i++) {
			particleList[i] -> move_vel(*deltaTime);
			particleList[i] -> draw();
		}

		kill_particle(&particleList);

		// particle_explode(100, glm::vec2(0.0, 0.0), particleShaderProgram, glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0), 0.0, 1.0);

		for (int i=0; i < bulletList.size(); i++) {
			bulletList[i].move_vel(*deltaTime);
			bulletList[i].draw();
			bulletList[i].screen_wrap();
			bulletList[i].despawn(*currentFrame);
		}

		kill_bullet(&bulletList);

		if (player -> alive) {
			player -> draw();
		}

		// testChar.draw();

		glBindFramebuffer(GL_FRAMEBUFFER, pass2);

		postPass1.draw(pass1Tex);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		postPass2.draw(pass2Tex);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (!player -> alive) {
			if (deathDelay == 0.0) {
				deathDelay = glfwGetTime();
			}
			else {
				if (deathDelay < glfwGetTime()-3) {
					return false;
				}
			}
		}

		if (asteroidList.size() == 0) {
			return true;
		}

	}

	return false;

}

int Particle::particleCount = 0;

int main() {

	srand(time(NULL));

	glfwInit();

	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	// Postprocessing framebuffer
	glGenFramebuffers(1, &pass1);
	glBindFramebuffer(GL_FRAMEBUFFER, pass1);

	glGenTextures(1, &pass1Tex);
	glBindTexture(GL_TEXTURE_2D, pass1Tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass1Tex, 0);



	glGenFramebuffers(1, &pass2);
	glBindFramebuffer(GL_FRAMEBUFFER, pass2);

	glGenTextures(1, &pass2Tex);
	glBindTexture(GL_TEXTURE_2D, pass2Tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pass2Tex, 0);



	postPass1ShaderProgram = create_shader("../resources/shaders/postProcessing/pass1/vertexShader.vert", "", "../resources/shaders/postProcessing/pass1/fragmentShader.frag", false);
	postPass2ShaderProgram = create_shader("../resources/shaders/postProcessing/pass2/vertexShader.vert", "", "../resources/shaders/postProcessing/pass2/fragmentShader.frag", false); 
	ScreenQuad postPass1(postPass1ShaderProgram);
	ScreenQuad postPass2(postPass2ShaderProgram);

	asteroidShaderProgram = create_shader("../resources/shaders/asteroids/vertexShader.vert", "../resources/shaders/asteroids/geometryShader.geom", "../resources/shaders/asteroids/fragmentShader.frag", true);
	playerShaderProgram = create_shader("../resources/shaders/player/vertexShader.vert", "", "../resources/shaders/player/fragmentShader.frag", false);
	particleShaderProgram = create_shader("../resources/shaders/particles/vertexShader.vert", "", "../resources/shaders/particles/fragmentShader.frag", false);



	Player player(glm::vec2(0.0, 0.0), playerShaderProgram, glm::vec2(0.0f, 0.0f));



	// characterShaderProgram = create_shader("../resources/shaders/characters/vertexShader.vert", "", "../resources/shaders/characters/fragmentShader.frag", false);
	// Character testChar(characterShaderProgram, glm::vec2(0.0, 0.0));



	float lastFrame = glfwGetTime();
	float currentFrame = glfwGetTime();
	float deltaTime;



	int lvlNum = 0;
	float levelResult = 1;
	glClearColor(0.005f, 0.005f, 0.005f, 1.0f);
	while (levelResult) {
		levelResult = start_level(&currentFrame, &deltaTime, &lastFrame, &player, postPass1, postPass2, lvlNum);
		lvlNum ++;
	}



	glfwSetWindowShouldClose(window, true);
	glfwTerminate();
	return 0;

}
