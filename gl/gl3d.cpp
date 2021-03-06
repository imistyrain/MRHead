#include "opencv2/opencv.hpp"
#include "mrgl.h"
#include "mrutil.h"
#include "vector"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

const aiScene* g_scene = NULL;
GLuint scene_list = 0;
GLFWwindow *g_window;
glm::mat4 g_proj;
glm::mat4 g_model;
glm::mat4 g_view;
aiVector3D scene_min, scene_max, scene_center;
int modes[] = {GL_POINTS,GL_LINE_STRIP,GL_TRIANGLES};
int mode_index = 2;
std::map<std::string, GLuint*> textureIdMap;
GLuint*	textureIds;

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]= { 0.0f, 0.0f, 15.0f, 1.0f };
float angles[3] = {0, 0, 0};
float eye_pos[3] = {0, 0, 1.8};
float scale = 1.0f;

GLint g_lastX = 0;
GLint g_lastY = 0;
GLint prev_time = 0;
bool g_bLbutton= false;
bool g_bShowModel = true;
bool g_bShowAxes = true;
bool g_bAutoRotating = false;
bool g_bflip = false;
int g_screen_width = 600;
int g_screen_height = 600;

void get_bounding_box_for_node (const C_STRUCT aiNode* nd, C_STRUCT aiVector3D* min,C_STRUCT aiVector3D* max,C_STRUCT aiMatrix4x4* trafo){
	C_STRUCT aiMatrix4x4 prev;
	unsigned int n = 0, t;
	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const C_STRUCT aiMesh* mesh = g_scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			C_STRUCT aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = std::min(min->x,tmp.x);
			min->y = std::min(min->y,tmp.y);
			min->z = std::min(min->z,tmp.z);

			max->x = std::max(max->x,tmp.x);
			max->y = std::max(max->y,tmp.y);
			max->z = std::max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

void get_bounding_box(C_STRUCT aiVector3D* min, C_STRUCT aiVector3D* max){
	C_STRUCT aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);
	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(g_scene->mRootNode,min,max,&trafo);
}

void Color4f(const aiColor4D *color){
	glColor4f(color->r, color->g, color->b, color->a);
}

void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4]){
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

void set_float4(float f[4], float a, float b, float c, float d){
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void apply_material(const aiMaterial *mtl){
	float c[4];
	GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;	// changed: to unsigned

	int texIndex = 0;
	aiString texPath;	//contains filename of texture

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
	if((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void recursive_render (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd){
	unsigned int i;
	unsigned int n = 0, t;
	C_STRUCT aiMatrix4x4 m = nd->mTransformation;

	/* update transform */
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	/* draw all meshes assigned to this node */
	for (; n < nd->mNumMeshes; ++n) {
		const C_STRUCT aiMesh* mesh = g_scene->mMeshes[nd->mMeshes[n]];

		apply_material(sc->mMaterials[mesh->mMaterialIndex]);

		if(mesh->mNormals == NULL) {
			glDisable(GL_LIGHTING);
		} else {
			glEnable(GL_LIGHTING);
		}
		if(mesh->mColors[0] != nullptr){
			glEnable(GL_COLOR_MATERIAL);
		} else {
			glDisable(GL_COLOR_MATERIAL);
		}

		for (t = 0; t < mesh->mNumFaces; ++t) {
			const C_STRUCT aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices) {
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}
			int mode = modes[mode_index];
			glBegin(mode);
			for(i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
			{
				int vertexIndex = face->mIndices[i];	// get group index for current index
				if(mesh->mColors[0] != nullptr)
					Color4f(&mesh->mColors[0][vertexIndex]);
				if(mesh->mNormals != nullptr)

					if(mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
					{
						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
					}					
					if (g_bflip){
						glNormal3f(mesh->mNormals[vertexIndex].x,-mesh->mNormals[vertexIndex].y,-mesh->mNormals[vertexIndex].z);
						glVertex3f(-mesh->mVertices[vertexIndex].x,mesh->mVertices[vertexIndex].y,mesh->mVertices[vertexIndex].z);
					} else {
						glNormal3fv(&mesh->mNormals[vertexIndex].x);
						glVertex3fv(&mesh->mVertices[vertexIndex].x);
					}	
			}
			glEnd();
		}
	}

	/* draw all children */
	for (n = 0; n < nd->mNumChildren; ++n) {
		recursive_render(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}

void do_motion(){
	static GLint prev_fps_time = 0;
	static int frames = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	if (g_bAutoRotating)
		angles[1] += (float)((time-prev_time)*0.02);
	prev_time = time;

	frames += 1;
	if ((time - prev_fps_time) > 1000) /* update every seconds */
    {
        int current_fps = frames * 1000 / (time - prev_fps_time);
        frames = 0;
        prev_fps_time = time;
    }
}

void freeTextureIds(){
	textureIdMap.clear();
	if (textureIds){
		delete[] textureIds;
		textureIds = nullptr;
	}
}

std::string getBasePath(const std::string& path)
{
	size_t pos = path.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : path.substr(0, pos + 1);
}

int LoadGLTextures(const char*modelpath, const aiScene* scene){
	freeTextureIds();
    if (scene->HasTextures())
		return 1;
	for (unsigned int m=0; m<scene->mNumMaterials; m++)
	{
		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;
		aiString path;
		while (texFound == AI_SUCCESS){
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			textureIdMap[path.data] = nullptr; //fill map with textures, pointers still NULL yet
			texIndex++;
		}
	}
	const size_t numTextures = textureIdMap.size();
	textureIds = new GLuint[numTextures];
	glGenTextures(static_cast<GLsizei>(numTextures), textureIds);
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();
	std::string basepath = getBasePath(modelpath);
	for (size_t i = 0; i < numTextures; i++){
		std::string filename = (*itr).first;  // get filename
		(*itr).second =  &textureIds[i];	  // save texture id for filename in map
		itr++;								  // next texture
		std::string fileloc = basepath + filename;	/* Loading of image */
		if (filename.length() > 0){
			cv::Mat textImg=cv::imread(fileloc);
			if (textImg.data){
				glBindTexture(GL_TEXTURE_2D, textureIds[i]);
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexImage2D(GL_TEXTURE_2D, 0, 3, textImg.cols, textImg.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, textImg.data);
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
				glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 );
				glPixelStorei( GL_UNPACK_SKIP_PIXELS, 0 );
				glPixelStorei( GL_UNPACK_SKIP_ROWS, 0 );
			} else {
				std::cout<<"Couldn't load Texture" + fileloc<<std::endl;
			}
		}
	}
	return true;
}

void drawAxis(float scale = 0.6) {
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.6, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.6, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.6);
	glEnd();

	glBegin(GL_POINTS);
	glColor3f(1.0, 1.0, 1.0);
	for (int i = 0; i < 10 * scale; i++) {
		float axis = i * 1.0 / 10;
		glVertex3f(axis, 0, 0);
		glVertex3f(0, axis, 0);
		glVertex3f(0, 0, axis);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.58, 0, 0);
	glRotatef(90, 0, 1, 0);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.58, 0);
	glRotatef(270, 1, 0, 0);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, 0.58);
	glutSolidCone(0.01, 0.03, 100, 10);
	glPopMatrix();

	glRasterPos3f(0.62f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
	glRasterPos3f(0.0f, 0.62f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
	glRasterPos3f(0.0f, 0.0f, 0.62f);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
}

void show_angles(){
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(eye_pos[0],eye_pos[1],eye_pos[2],0.f,0.f,0.f,0.f,1.f,0.f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2f(0.8,-1);
	#if USE_GLM
		glColor3f(0.0f, 0.0f, 1.0f);
		glRasterPos2f(0.8,-1);
		std::string strangle = double2string(g_angle);
		for (int i = 0; i < strangle.length();i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, strangle[i]);
		}
	#else
		std::string strangle;
		for (int i = 0; i < 3; i++){
			strangle = strangle+int2string(angles[i])+" ";
		}
		for (int i = 0; i < strangle.length();i++){
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, strangle[i]);
		}
	#endif
	glPopMatrix();
}

int loadasset(const char* path){
	if (path){
		g_scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);		
		if (g_scene) {	
			if (!LoadGLTextures(path, g_scene)){
				return -2;
			}
			get_bounding_box(&scene_min,&scene_max);
			scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
			scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
			scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
			return 0;
		}
	}
	return -1;
}

void update_drawlist(){
	if (scene_list != 0) {
		glDeleteLists(scene_list, 0);
	}
	scene_list = glGenLists(1);
	glNewList(scene_list, GL_COMPILE);
	recursive_render(g_scene, g_scene->mRootNode);
	glEndList();
}

glm::vec3 computePointOnSphere(double x, double y){
	glm::vec3 coord = glm::vec3(1.0*x / g_screen_width * 2 - 1.0,
        1.0*y / g_screen_height * 2 - 1.0, 0);
    coord.y = -coord.y;
    float length_squared = coord.x * coord.x + coord.y * coord.y;
    if (length_squared <= 1.0)
        coord.z = sqrt(1.0 - length_squared);
    else
        coord = glm::normalize(coord);
    return coord;
}

void mouse_button_callback(GLFWwindow *win, int button, int action, int mods){
    switch (button){
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS){
				g_bLbutton = true;
			} else if (action == GLFW_RELEASE){
				g_bLbutton = false;
			}
			break;
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y){
	if (g_bLbutton){
		glm::vec3 prevPos = computePointOnSphere(g_lastX, g_lastY);
		glm::vec3 currPos = computePointOnSphere(x, y);
		glm::vec3 axis  = glm::cross(prevPos, currPos);
		float angle = acos(std::min(1.0f, glm::dot(prevPos, currPos)))*3;
		axis = glm::inverse(glm::mat3(g_view*g_model)) * axis;
        g_model = glm::rotate(g_model,angle,axis);
	}
	g_lastX = x;
	g_lastY = y;
}

void scroll_callback(GLFWwindow *win, double xoffset, double yoffset){
	if (scale + 0.01 * yoffset > 0){
		scale += 0.01 * yoffset;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch (key){
        case 'a':
        case 'A':
            if (action == GLFW_PRESS){
                g_bShowAxes = !g_bShowAxes;
            }
            break;
		case 'f':
		case 'F':
			if (action == GLFW_PRESS){
				g_bflip = !g_bflip;
				update_drawlist();
			}
			break;
		case 'm':
		case 'M':
			if (action == GLFW_PRESS){
				mode_index = (mode_index + 1) %(sizeof(modes)/sizeof(int));
				update_drawlist();
			}
			break;
		case 's':
		case 'S':
			if (action == GLFW_PRESS){
				g_bShowModel = !g_bShowModel;
			}
			break;
		case 'r':
		case 'R':
			if (action == GLFW_PRESS){
			}
			break;
	}
}

void reshape(GLFWwindow* window, int w, int h){
    g_screen_width = w;
    g_screen_height = h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	const double aspectRatio = (float) w / h, fieldOfView = 60.0;
	gluPerspective(fieldOfView, aspectRatio, 1.0, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye_pos[0],eye_pos[1],eye_pos[2],0.f,0.f,0.f,0.f,1.f,0.f);
}

void initOpenGL(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.5);
	glPointSize(3.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	update_drawlist();
	glMatrixMode(GL_PROJECTION);
	g_proj = glm::perspective(glm::radians(60.0f), g_screen_width * 1.0f / g_screen_height, 0.01f, 10.0f);
	glLoadMatrixf(glm::value_ptr(g_proj));
	glMatrixMode(GL_MODELVIEW);
	g_view = glm::lookAt(glm::vec3(eye_pos[0], eye_pos[1], eye_pos[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	g_model = glm::mat4(1.0);
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glfwInit();
	g_window = glfwCreateWindow(g_screen_width, g_screen_height, "gl3d", NULL, NULL);
    if (g_window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(g_window);
    glfwSetWindowSizeCallback(g_window, reshape);
    glfwSetKeyCallback(g_window, key_callback);
    glfwSetMouseButtonCallback(g_window, mouse_button_callback);
    glfwSetCursorPosCallback(g_window, cursor_position_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
	#if _WIN32
		glewInit();
	#endif
    char* model_file = "cube.obj";
    if (argc > 1){
        model_file = argv[1];
    }
	std::cout<<"Loading "<<model_file<<std::endl;
	if (0 != loadasset(model_file)) {
		printf("Failed to load model. Please ensure that the specified file exists.\n");
		return -1;
	}
 	initOpenGL();
	while (!glfwWindowShouldClose(g_window)){
        glClearColor(0.384, 0.384, 0.768, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 g_mv = g_view * g_model;
		glLoadMatrixf(glm::value_ptr(g_mv));
		
		if (g_bShowAxes)
			drawAxis();

		glEnable(GL_TEXTURE_2D);
		float tmp = scene_max.x-scene_min.x;
		tmp = std::max(scene_max.y - scene_min.y,tmp);
		tmp = std::max(scene_max.z - scene_min.z,tmp);
		tmp = scale / tmp;
		glScalef(tmp, tmp, tmp);
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);

		glLineWidth(0.5);
		if (g_bShowModel && scene_list)
			glCallList(scene_list);
		glPopMatrix();

		glfwSwapBuffers(g_window);
        glfwWaitEvents();
    }
    glfwTerminate();	
    aiReleaseImport(g_scene);
	return 0;
}