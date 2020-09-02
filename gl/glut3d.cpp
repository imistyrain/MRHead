#define _USE_MATH_DEFINES
#include "vector"
#include "opencv2/opencv.hpp"
#include "mrgl.h"
#include "mrutil.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define USE_GLM 1
#if USE_GLM
#include "glm/glm.hpp"
glm::vec3 g_camaxis;
float g_angle = 0;
#endif

const aiScene* g_scene = NULL;
GLuint scene_list = 0;
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

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

void reshape(int width, int height){
	const double aspectRatio = (float) width / height, fieldOfView = 60.0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio, 1.0, 1000.0);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye_pos[0],eye_pos[1],eye_pos[2],0.f,0.f,0.f,0.f,1.f,0.f);
}

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

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
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
	for (size_t i=0; i<numTextures; i++){
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

void draw_axes(){
	glLineWidth(1.5);
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
	for (int i = 0;i<10*(scale/2);i++){
		float axis = i * 1.0/ 10;
		glVertex3f(axis,0,0);
		glVertex3f(0,axis,0);
		glVertex3f(0,0,axis);
	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.58,0,0);
	glRotatef(90,0,1,0);
	glutSolidCone(0.01,0.03,100,10);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.58,0);
	glRotatef(270,1,0,0);
	glutSolidCone(0.01,0.03,100,10);	
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.0,0.58);
	glutSolidCone(0.01,0.03,100,10);	
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

void display(){
	glClearColor(0.384, 0.384, 0.768, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	show_angles();
	#if USE_GLM
		glRotatef(g_angle, g_camaxis[0],g_camaxis[1],g_camaxis[2]);
	#else
		glLoadIdentity();
		gluLookAt(eye_pos[0],eye_pos[1],eye_pos[2],0.f,0.f,0.f,0.f,1.f,0.f);
		glRotatef(angles[1],0.f,1.f,0.f);
		glRotatef(-angles[0],-1.f,0.f,0.f);
	#endif
	glPushMatrix();
	if (g_bShowAxes){
		draw_axes();
	}
	glEnable(GL_TEXTURE_2D);
	float tmp = scene_max.x-scene_min.x;
	tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
	tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
	tmp = scale / tmp;
	glScalef(tmp, tmp, tmp);
	glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);

	glLineWidth(0.5);
	if (g_bShowModel && scene_list)
		glCallList(scene_list);
	glPopMatrix();
	glutSwapBuffers();
}

void update_drawlist(){
	if (scene_list != 0) {
		glDeleteLists(scene_list, 0);
	}
	scene_list = glGenLists(1);
	glNewList(scene_list, GL_COMPILE);
	recursive_render(g_scene, g_scene->mRootNode);
	glEndList();
	glutPostRedisplay();
}

int loadasset (const char* path){
	if (path)
		g_scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);
	if (g_scene) {
		if (!LoadGLTextures(path, g_scene)){
			return -1;
		}
		get_bounding_box(&scene_min,&scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}
	return 1;
}

#if USE_GLM
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
#endif

void onMouse(GLint button, GLint state, GLint x, GLint y){
	switch (button){
		case GLUT_LEFT_BUTTON:
			switch (state){
				case GLUT_DOWN:
					g_lastX = x;
					g_lastY = y;
					g_bLbutton = true;
					break;
				case GLUT_UP:
					g_lastX = x;
					g_lastY = y;
					g_bLbutton = false;
					break;
			}
		break;
		default:
			break;
	}
	g_bAutoRotating = false;
}

void onMotion(GLint x, GLint y){
	if (g_bLbutton){
		#if USE_GLM
			glm::vec3 prevPos = computePointOnSphere(g_lastX, g_lastY);
			glm::vec3 currPos = computePointOnSphere(x, y);
			g_camaxis = glm::cross(prevPos, currPos);
			g_angle = acos(std::min(1.0f, glm::dot(prevPos, currPos))) * 2 * 180 / M_PI;
		#else
			angles[1] += (x - g_lastX);
			angles[0] += (y - g_lastY);
			for (int i = 0; i < 3; i++){
				angles[i] += int(abs(angles[i])/360)*360;
				angles[i] = angles[i] - int(angles[i]/360) * 360;	
			}
		#endif
		glutPostRedisplay();
	}
	g_lastX = x;
	g_lastY = y;
}

void on_timer(int val){
	if (g_bAutoRotating){
		do_motion();
		glutPostRedisplay();
		glutTimerFunc(30,on_timer,0);
	}
}

void processNormalKeys(unsigned char key,int x,int y){
    switch(key){
		case 'a':
		case 'A':
			g_bShowAxes = !g_bShowAxes;
			glutPostRedisplay();
			break;
		case 'f':
		case 'F':
			g_bflip = !g_bflip;
			update_drawlist();
			glutPostRedisplay();
			break;
		case 'm':
		case 'M':
			mode_index = (mode_index + 1) %(sizeof(modes)/sizeof(int));
			update_drawlist();
			glutPostRedisplay();
			break;
		case 's':
		case 'S':
			g_bShowModel = !g_bShowModel;
			glutPostRedisplay();
			break;
		case 'r':
		case 'R':
			for (int i = 0; i < 3; i++){
				angles[i] = 0;
			}
			glutPostRedisplay();
			break;
		case 't':
		case 'T':
			g_bAutoRotating = !g_bAutoRotating;
			if (g_bAutoRotating){
				prev_time = glutGet(GLUT_ELAPSED_TIME);
				glutTimerFunc(30,on_timer,0);
			}
			break;
		case '-':
			scale /= 1.05;
			glutPostRedisplay();
			break;
		case '=':
			scale *= 1.05;
			glutPostRedisplay();
			break;
	}
}

void initOpenGL(){
	glutInitWindowSize(g_screen_width,g_screen_height);
    glutCreateWindow("gl3d");
	#if _WIN32
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cout << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
		}
	#endif
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutKeyboardFunc(processNormalKeys);
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
	if (g_bAutoRotating){
		prev_time = glutGet(GLUT_ELAPSED_TIME);
		glutTimerFunc(30,on_timer,0);
	}
}

int main(int argc, char **argv){
	glutInit(&argc, argv);
    const char* model_file = "cube.obj";
    if (argc > 1){
        model_file = argv[1];
    }
	std::cout<<"Loading "<<model_file<<std::endl;
	initOpenGL();
	if (0 != loadasset(model_file)) {
		printf("Failed to load model. Please ensure that the specified file exists.\n");
		return -1;
	}
	update_drawlist();
	glutMainLoop();
    aiReleaseImport(g_scene);
    aiDetachAllLogStreams();
	return 0;
}