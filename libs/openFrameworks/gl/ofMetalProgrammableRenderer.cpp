#include "ofMetalProgrammableRenderer.h"
#include "ofMesh.h"
#include "ofPath.h"
#include "ofMesh.h"
#include "ofBitmapFont.h"
#include "ofGLUtils.h"
#include "ofImage.h"
#include "ofFbo.h"
#include "ofVbo.h"
#include "of3dPrimitives.h"
#include "ofLight.h"
#include "ofMaterial.h"
#include "ofCamera.h"
#include "ofTrueTypeFont.h"
#include "ofNode.h"
#include "ofVideoBaseTypes.h"

using namespace std;

const string ofMetalProgrammableRenderer::TYPE="Metal";
static bool computerRendererCreated = false;

bool ofIsComputerProgrammableRenderer(){
	return computerRendererCreated;
}


//----------------------------------------------------------
ofMetalProgrammableRenderer::ofMetalProgrammableRenderer(const ofAppBaseWindow * _window)
:matrixStack(_window)
,graphics3d(this)
{
	computerRendererCreated = true;
	bBackgroundAuto = true;

	lineMesh.getVertices().resize(2);
	lineMesh.setMode(OF_PRIMITIVE_LINES);
	triangleMesh.getVertices().resize(3);
	rectMesh.getVertices().resize(4);

	bitmapStringEnabled = false;
    verticesEnabled = true;
    colorsEnabled = false;
    texCoordsEnabled = false;
    normalsEnabled = false;
	settingDefaultShader = false;
	usingVideoShader = false;
	usingCustomShader = false;

	wrongUseLoggedOnce = false;

	uniqueShader = false;

	currentShader = nullptr;

	currentTextureTarget = OF_NO_TEXTURE;
	currentMaterial = nullptr;
	alphaMaskTextureTarget = OF_NO_TEXTURE;

	major = 3;
	minor = 2;
	window = _window;

	currentFramebufferId = 0;
	defaultFramebufferId = 0;
	path.setMode(ofPath::POLYLINES);
    path.setUseShapeColor(false);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::startRender() {
	currentFramebufferId = defaultFramebufferId;
	framebufferIdStack.push_back(defaultFramebufferId);
	matrixStack.setRenderSurface(*window);
	beginDefaultShader();
	viewport();
    // to do non auto clear on PC for now - we do something like "single" buffering --
    // it's not that pretty but it work for the most part

    #ifdef TARGET_WIN32
    if (getBackgroundAuto() == false){
        glDrawBuffer (GL_FRONT);
    }
    #endif

	if ( getBackgroundAuto() ){// || ofGetFrameNum() < 3){
		background(currentStyle.bgColor);
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::finishRender() {
	if (!uniqueShader) {
		glUseProgram(0);
		if(!usingCustomShader) currentShader = nullptr;
	}
	matrixStack.clearStacks();
	framebufferIdStack.clear();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors, bool useTextures, bool useNormals) const{
	if (vertexData.getVertices().empty()) return;
	
	
//	// tig: note that for GL3+ we use glPolygonMode to draw wireframes or filled meshes, and not the primitive mode.
//	// the reason is not purely aesthetic, but more conformant with the behaviour of ofGLRenderer. Whereas
//	// gles2.0 doesn't allow for a polygonmode.
//	// Also gles2 still supports vertex array syntax for uploading data to attributes and it seems to be faster than
//	// vbo's for meshes that are updated frequently so let's use that instead
//
//	//if (bSmoothHinted) startSmoothing();
//
//#if defined(TARGET_OPENGLES) && !defined(TARGET_EMSCRIPTEN)
//	glEnableVertexAttribArray(ofShader::POSITION_ATTRIBUTE);
//	glVertexAttribPointer(ofShader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(ofVec3f), vertexData.getVerticesPointer());
//
//	useNormals &= (vertexData.getNumNormals()>0);
//	if(useNormals){
//		glEnableVertexAttribArray(ofShader::NORMAL_ATTRIBUTE);
//		glVertexAttribPointer(ofShader::NORMAL_ATTRIBUTE, 3, GL_FLOAT, GL_TRUE, sizeof(ofVec3f), vertexData.getNormalsPointer());
//	}else{
//		glDisableVertexAttribArray(ofShader::NORMAL_ATTRIBUTE);
//	}
//
//	useColors &= (vertexData.getNumColors()>0);
//	if(useColors){
//		glEnableVertexAttribArray(ofShader::COLOR_ATTRIBUTE);
//		glVertexAttribPointer(ofShader::COLOR_ATTRIBUTE, 4,GL_FLOAT, GL_FALSE, sizeof(ofFloatColor), vertexData.getColorsPointer());
//	}else{
//		glDisableVertexAttribArray(ofShader::COLOR_ATTRIBUTE);
//	}
//
//	useTextures &= (vertexData.getNumTexCoords()>0);
//	if(useTextures){
//		glEnableVertexAttribArray(ofShader::TEXCOORD_ATTRIBUTE);
//		glVertexAttribPointer(ofShader::TEXCOORD_ATTRIBUTE,2, GL_FLOAT, GL_FALSE, sizeof(ofVec2f), vertexData.getTexCoordsPointer());
//	}else{
//		glDisableVertexAttribArray(ofShader::TEXCOORD_ATTRIBUTE);
//	}
//
//
//	const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(true,useColors,useTextures,useNormals);
//
//	GLenum drawMode;
//	switch(renderType){
//	case OF_MESH_POINTS:
//		drawMode = GL_POINTS;
//		break;
//	case OF_MESH_WIREFRAME:
//		drawMode = GL_LINES;
//		break;
//	case OF_MESH_FILL:
//		drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
//		break;
//	default:
//		drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
//		break;
//	}
//
//	if(vertexData.getNumIndices()){
//		glDrawElements(drawMode, vertexData.getNumIndices(),GL_UNSIGNED_SHORT,vertexData.getIndexPointer());
//	}else{
//		glDrawArrays(drawMode, 0, vertexData.getNumVertices());
//	}
//#else
//
//
//#ifndef TARGET_OPENGLES
//	meshVbo.setMesh(vertexData, GL_STREAM_DRAW, useColors, useTextures, useNormals);
//	glPolygonMode(GL_FRONT_AND_BACK, ofGetGLPolyMode(renderType));
//	GLenum drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
//#else
//	meshVbo.setMesh(vertexData, GL_STATIC_DRAW, useColors, useTextures, useNormals);
//	GLenum drawMode;
//	switch(renderType){
//	case OF_MESH_POINTS:
//		drawMode = GL_POINTS;
//		break;
//	case OF_MESH_WIREFRAME:
//		drawMode = GL_LINE_STRIP;
//		break;
//	case OF_MESH_FILL:
//		drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
//		break;
//	default:
//		drawMode = ofGetGLPrimitiveMode(vertexData.getMode());
//		break;
//	}
//#endif
//	if(meshVbo.getUsingIndices()) {
//		drawElements(meshVbo,drawMode, meshVbo.getNumIndices());
//	} else {
//		draw(meshVbo, drawMode, 0, vertexData.getNumVertices());
//	}
//
//	// tig: note further that we could glGet() and store the current polygon mode, but don't, since that would
//	// infer a massive performance hit. instead, we revert the glPolygonMode to mirror the current ofFill state
//	// after we're finished drawing, following the principle of least surprise.
//	// ideally the glPolygonMode (or the polygon draw mode) should be part of ofStyle so that we can keep track
//	// of its state on the client side...
//
//#ifndef TARGET_OPENGLES
//	glPolygonMode(GL_FRONT_AND_BACK, currentStyle.bFill ?  GL_FILL : GL_LINE);
//#endif
//
//#endif
	
	//if (bSmoothHinted) endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofVboMesh & mesh, ofPolyRenderMode renderType) const{
	//drawInstanced(mesh,renderType,1);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw( const of3dPrimitive& model, ofPolyRenderMode renderType) const {
	const_cast<ofMetalProgrammableRenderer*>(this)->pushMatrix();
	const_cast<ofMetalProgrammableRenderer*>(this)->multMatrix(model.getGlobalTransformMatrix());
	if(model.isUsingVbo()){
		draw(static_cast<const ofVboMesh&>(model.getMesh()),renderType);
	}else{
		draw(model.getMesh(),renderType);
	}
	const_cast<ofMetalProgrammableRenderer*>(this)->popMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofNode& node) const{
	const_cast<ofMetalProgrammableRenderer*>(this)->pushMatrix();
	const_cast<ofMetalProgrammableRenderer*>(this)->multMatrix(node.getGlobalTransformMatrix());
	node.customDraw(this);
	const_cast<ofMetalProgrammableRenderer*>(this)->popMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofPolyline & poly) const{
	if(poly.getVertices().empty()) return;

	// use smoothness, if requested:
	//if (bSmoothHinted) startSmoothing();

//#if defined( TARGET_OPENGLES ) && !defined(TARGET_EMSCRIPTEN)
//
//	glEnableVertexAttribArray(ofShader::POSITION_ATTRIBUTE);
//	glVertexAttribPointer(ofShader::POSITION_ATTRIBUTE, 3, GL_FLOAT, GL_FALSE, sizeof(ofVec3f), &poly[0]);
//
//	const_cast<ofGLProgrammableRenderer*>(this)->setAttributes(true,false,false,false);
//
//	GLenum drawMode = poly.isClosed()?GL_LINE_LOOP:GL_LINE_STRIP;
//
//	glDrawArrays(drawMode, 0, poly.size());
//
//#else
//
//	meshVbo.setVertexData(&poly.getVertices()[0], poly.size(), GL_DYNAMIC_DRAW);
//	meshVbo.draw(poly.isClosed()?GL_LINE_LOOP:GL_LINE_STRIP, 0, poly.size());
//
//#endif
	// use smoothness, if requested:
	//if (bSmoothHinted) endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofPath & shape) const{
	ofColor prevColor;
	if(shape.getUseShapeColor()){
		prevColor = currentStyle.color;
	}
//	ofMetalProgrammableRenderer * mut_this = const_cast<ofMetalProgrammableRenderer*>(this);
//	if(shape.isFilled()){
//		const ofMesh & mesh = shape.getTessellation();
//		if(shape.getUseShapeColor()){
//			mut_this->setColor( shape.getFillColor(),shape.getFillColor().a);
//		}
//		draw(mesh,OF_MESH_FILL);
//	}
//	if(shape.hasOutline()){
//		float lineWidth = currentStyle.lineWidth;
//		if(shape.getUseShapeColor()){
//			mut_this->setColor( shape.getStrokeColor(), shape.getStrokeColor().a);
//		}
//		mut_this->setLineWidth( shape.getStrokeWidth() );
//		const vector<ofPolyline> & outlines = shape.getOutline();
//		for(int i=0; i<(int)outlines.size(); i++)
//			draw(outlines[i]);
//		mut_this->setLineWidth(lineWidth);
//	}
//	if(shape.getUseShapeColor()){
//		mut_this->setColor(prevColor);
//	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const{
	if(image.isUsingTexture()){
		const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(true,false,true,false);
		const ofTexture& tex = image.getTexture();
		if(tex.isAllocated()) {
			const_cast<ofMetalProgrammableRenderer*>(this)->bind(tex,0);
			draw(tex.getMeshForSubsection(x,y,z,w,h,sx,sy,sw,sh,isVFlipped(),currentStyle.rectMode),OF_MESH_FILL,false,true,false);
			const_cast<ofMetalProgrammableRenderer*>(this)->unbind(tex,0);
		} else {
			ofLogWarning("ofMetalProgrammableRenderer") << "draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofFloatImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const{
	if(image.isUsingTexture()){
		const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(true,false,true,false);
		const ofTexture& tex = image.getTexture();
		if(tex.isAllocated()) {
			const_cast<ofMetalProgrammableRenderer*>(this)->bind(tex,0);
			draw(tex.getMeshForSubsection(x,y,z,w,h,sx,sy,sw,sh,isVFlipped(),currentStyle.rectMode),OF_MESH_FILL,false,true,false);
			const_cast<ofMetalProgrammableRenderer*>(this)->unbind(tex,0);
		} else {
			ofLogWarning("ofMetalProgrammableRenderer") << "draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofShortImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const{
	if(image.isUsingTexture()){
		const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(true,false,true,false);
		const ofTexture& tex = image.getTexture();
		if(tex.isAllocated()) {
			const_cast<ofMetalProgrammableRenderer*>(this)->bind(tex,0);
			draw(tex.getMeshForSubsection(x,y,z,w,h,sx,sy,sw,sh,isVFlipped(),currentStyle.rectMode),OF_MESH_FILL,false,true,false);
			const_cast<ofMetalProgrammableRenderer*>(this)->unbind(tex,0);
		} else {
			ofLogWarning("ofMetalProgrammableRenderer") << "draw(): texture is not allocated";
		}
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofTexture & tex, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const{
	const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(true,false,true,false);
	if(tex.isAllocated()) {
		const_cast<ofMetalProgrammableRenderer*>(this)->bind(tex,0);
		draw(tex.getMeshForSubsection(x,y,z,w,h,sx,sy,sw,sh,isVFlipped(),currentStyle.rectMode),OF_MESH_FILL,false,true,false);
		const_cast<ofMetalProgrammableRenderer*>(this)->unbind(tex,0);
	} else {
		ofLogWarning("ofMetalProgrammableRenderer") << "draw(): texture is not allocated";
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofBaseVideoDraws & video, float x, float y, float w, float h) const{
	if(!video.isInitialized() || !video.isUsingTexture() || video.getTexturePlanes().empty()){
		return;
	}
	const_cast<ofMetalProgrammableRenderer*>(this)->bind(video);
	draw(video.getTexture().getMeshForSubsection(x,y,0,w,h,0,0,video.getWidth(),video.getHeight(),isVFlipped(),currentStyle.rectMode),OF_MESH_FILL,false,true,false);
	const_cast<ofMetalProgrammableRenderer*>(this)->unbind(video);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::draw(const ofVbo & vbo, GLuint drawMode, int first, int total) const{
	if(vbo.getUsingVerts()) {
		vbo.bind();
		const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(vbo.getUsingVerts(),vbo.getUsingColors(),vbo.getUsingTexCoords(),vbo.getUsingNormals());
		//glDrawArrays(drawMode, first, total);
		vbo.unbind();
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawElements(const ofVbo & vbo, GLuint drawMode, int amt, int offsetelements) const{
	if(vbo.getUsingVerts()) {
		vbo.bind();
		const_cast<ofMetalProgrammableRenderer*>(this)->setAttributes(vbo.getUsingVerts(),vbo.getUsingColors(),vbo.getUsingTexCoords(),vbo.getUsingNormals());
//#ifdef TARGET_OPENGLES
//        glDrawElements(drawMode, amt, GL_UNSIGNED_SHORT, (void*)(sizeof(ofIndexType) * offsetelements));
//#else
//        glDrawElements(drawMode, amt, GL_UNSIGNED_INT, (void*)(sizeof(ofIndexType) * offsetelements));
//#endif
		vbo.unbind();
	}
}


//----------------------------------------------------------
void ofMetalProgrammableRenderer::bind(const ofBaseVideoDraws & video){
	if(!video.isInitialized() || !video.isUsingTexture() || video.getTexturePlanes().empty()){
		return;
	}
	const ofShader * shader = nullptr;
	if(!usingCustomShader){
		shader = getVideoShader(video);
		if(shader){
			bind(*shader);
			setVideoShaderUniforms(video,*shader);
			usingVideoShader = true;
		}
	}

	if(!usingVideoShader){
		bind(video.getTexture(),0);
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::unbind(const ofBaseVideoDraws & video){
	if(!video.isInitialized() || !video.isUsingTexture() || video.getTexturePlanes().empty()){
		return;
	}
	if(usingVideoShader){
		unbind(*currentShader);
	}else{
		unbind(video.getTexture(),0);
	}
	usingVideoShader = false;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::pushView() {
	matrixStack.pushView();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::popView() {
	matrixStack.popView();
	uploadMatrices();
	viewport(matrixStack.getCurrentViewport());
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::viewport(ofRectangle viewport_){
	viewport(viewport_.x,viewport_.y,viewport_.width,viewport_.height,isVFlipped());
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::viewport(float x, float y, float width, float height, bool vflip) {
	matrixStack.viewport(x,y,width,height,vflip);
	ofRectangle nativeViewport = matrixStack.getNativeViewport();
	//glViewport(nativeViewport.x,nativeViewport.y,nativeViewport.width,nativeViewport.height);
}

//----------------------------------------------------------
ofRectangle ofMetalProgrammableRenderer::getCurrentViewport() const{
	return matrixStack.getCurrentViewport();
}

//----------------------------------------------------------
ofRectangle ofMetalProgrammableRenderer::getNativeViewport() const{
    return matrixStack.getNativeViewport();
}

//----------------------------------------------------------
int ofMetalProgrammableRenderer::getViewportWidth() const{
	return getCurrentViewport().width;
}

//----------------------------------------------------------
int ofMetalProgrammableRenderer::getViewportHeight() const{
	return getCurrentViewport().height;
}

//----------------------------------------------------------
bool ofMetalProgrammableRenderer::isVFlipped() const{
	return matrixStack.isVFlipped();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setCoordHandedness(ofHandednessType handedness) {
	;
}

//----------------------------------------------------------
ofHandednessType ofMetalProgrammableRenderer::getCoordHandedness() const{
	return matrixStack.getHandedness();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setOrientation(ofOrientation orientation, bool vFlip){
	matrixStack.setOrientation(orientation,vFlip);
	uploadMatrices();

}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setupScreenPerspective(float width, float height, float fov, float nearDist, float farDist) {
	float viewW, viewH;
	if(width<0 || height<0){
		ofRectangle currentViewport = getCurrentViewport();

		viewW = currentViewport.width;
		viewH = currentViewport.height;
	}else{
		viewW = width;
		viewH = height;
	}

	float eyeX = viewW / 2;
	float eyeY = viewH / 2;
	float halfFov = PI * fov / 360;
	float theTan = tanf(halfFov);
	float dist = eyeY / theTan;
	float aspect = (float) viewW / viewH;

	if(nearDist == 0) nearDist = dist / 10.0f;
	if(farDist == 0) farDist = dist * 10.0f;


	matrixMode(OF_MATRIX_PROJECTION);
	auto persp = glm::perspective(ofDegToRad(fov), aspect, nearDist, farDist);
	loadMatrix( persp );

	matrixMode(OF_MATRIX_MODELVIEW);
	auto lookAt = glm::lookAt( glm::vec3{eyeX, eyeY, dist},  glm::vec3{eyeX, eyeY, 0.f},  glm::vec3{0.f, 1.f, 0.f} );
	loadViewMatrix(lookAt);
	
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setupScreenOrtho(float width, float height, float nearDist, float farDist) {
	float viewW, viewH;
	if(width<0 || height<0){
		ofRectangle currentViewport = getCurrentViewport();

		viewW = currentViewport.width;
		viewH = currentViewport.height;
	}else{
		viewW = width;
		viewH = height;
	}

	auto ortho = glm::ortho(0.f, viewW, 0.f, viewH, nearDist, farDist);

	matrixMode(OF_MATRIX_PROJECTION);
	loadMatrix(ortho); // make ortho our new projection matrix.

	matrixMode(OF_MATRIX_MODELVIEW);
	loadViewMatrix(glm::mat4(1.0));
}

//----------------------------------------------------------
//Resets openGL parameters back to OF defaults
void ofMetalProgrammableRenderer::setupGraphicDefaults(){
	setStyle(ofStyle());
	path.setMode(ofPath::POLYLINES);
	path.setUseShapeColor(false);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setupScreen(){
	beginDefaultShader();
	setupScreenPerspective();	// assume defaults
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setCircleResolution(int res){
	if((int)circlePolyline.size()!=res+1){
		circlePolyline.clear();
		circlePolyline.arc(0,0,0,1,1,0,360,res);
		circleMesh.getVertices() = circlePolyline.getVertices();
		path.setCircleResolution(res);
	}
	currentStyle.circleResolution = res; 
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setPolyMode(ofPolyWindingMode mode){
	currentStyle.polyMode = mode;
	path.setPolyWindingMode(mode);
}

//our openGL wrappers
//----------------------------------------------------------
void ofMetalProgrammableRenderer::pushMatrix(){
	matrixStack.pushMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::popMatrix(){
	matrixStack.popMatrix();
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::translate(const glm::vec3& p){
	translate(p.x, p.y, p.z);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::translate(float x, float y, float z){
	matrixStack.translate(x,y,z);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::scale(float xAmnt, float yAmnt, float zAmnt){
	matrixStack.scale(xAmnt, yAmnt, zAmnt);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::rotateRad(float radians, float vecX, float vecY, float vecZ){
	matrixStack.rotateRad(radians, vecX, vecY, vecZ);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::rotateXRad(float radians){
	rotateRad(radians, 1, 0, 0);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::rotateYRad(float radians){
	rotateRad(radians, 0, 1, 0);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::rotateZRad(float radians){
	rotateRad(radians, 0, 0, 1);
}

//same as ofRotateZ
//----------------------------------------------------------
void ofMetalProgrammableRenderer::rotateRad(float radians){
	rotateZRad(radians);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::matrixMode(ofMatrixMode mode){
	matrixStack.matrixMode(mode);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::loadIdentityMatrix (void){
	matrixStack.loadIdentityMatrix();
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::loadMatrix (const glm::mat4 & m){
	matrixStack.loadMatrix(m);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::loadMatrix (const float *m){
	loadMatrix(glm::make_mat4(m));
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::multMatrix (const glm::mat4 & m){
	matrixStack.multMatrix(m);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::multMatrix (const float *m){
	multMatrix(glm::make_mat4(m));
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::loadViewMatrix(const glm::mat4 & m){
	matrixStack.loadViewMatrix(m);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::multViewMatrix(const glm::mat4 & m){
	matrixStack.multViewMatrix(m);
	uploadCurrentMatrix();
}

//----------------------------------------------------------
glm::mat4 ofMetalProgrammableRenderer::getCurrentViewMatrix() const{
	return matrixStack.getViewMatrix();
}

//----------------------------------------------------------
glm::mat4 ofMetalProgrammableRenderer::getCurrentNormalMatrix() const{
	return glm::transpose(glm::inverse(getCurrentMatrix(OF_MATRIX_MODELVIEW)));
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::uploadCurrentMatrix(){
	if(!currentShader) return;
	// uploads the current matrix to the current shader.
//	switch(matrixStack.getCurrentMatrixMode()){
//	case OF_MATRIX_MODELVIEW:
//		currentShader->setUniformMatrix4f(MODEL_MATRIX_UNIFORM, matrixStack.getModelMatrix());
//		currentShader->setUniformMatrix4f(VIEW_MATRIX_UNIFORM, matrixStack.getViewMatrix());
//		currentShader->setUniformMatrix4f(MODELVIEW_MATRIX_UNIFORM, matrixStack.getModelViewMatrix());
//		currentShader->setUniformMatrix4f(MODELVIEW_PROJECTION_MATRIX_UNIFORM, matrixStack.getModelViewProjectionMatrix());
//		if(currentMaterial){
//			//currentMaterial->uploadMatrices(*currentShader,*this);
//		}
//		break;
//	case OF_MATRIX_PROJECTION:
//		currentShader->setUniformMatrix4f(PROJECTION_MATRIX_UNIFORM, matrixStack.getProjectionMatrix());
//		currentShader->setUniformMatrix4f(MODELVIEW_PROJECTION_MATRIX_UNIFORM, matrixStack.getModelViewProjectionMatrix());
//		break;
//	case OF_MATRIX_TEXTURE:
//		currentShader->setUniformMatrix4f(TEXTURE_MATRIX_UNIFORM, matrixStack.getTextureMatrix());
//		break;
//	}

}

//----------------------------------------------------------
glm::mat4 ofMetalProgrammableRenderer::getCurrentMatrix(ofMatrixMode matrixMode_) const {
	switch (matrixMode_) {
		case OF_MATRIX_MODELVIEW:
			return matrixStack.getModelViewMatrix();
			break;
		case OF_MATRIX_PROJECTION:
			return matrixStack.getProjectionMatrix();
			break;
		case OF_MATRIX_TEXTURE:
			return matrixStack.getTextureMatrix();
			break;
		default:
			ofLogWarning() << "Invalid getCurrentMatrix query";
			return glm::mat4(1.0);
			break;
	}
}

//----------------------------------------------------------
glm::mat4 ofMetalProgrammableRenderer::getCurrentOrientationMatrix() const {
	return matrixStack.getOrientationMatrix();
}
//----------------------------------------------------------
void ofMetalProgrammableRenderer::setColor(const ofColor & color){
	setColor(color.r,color.g,color.b,color.a);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setColor(const ofColor & color, int _a){
	setColor(color.r,color.g,color.b,_a);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setColor(int _r, int _g, int _b){
	setColor(_r, _g, _b, 255);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setColor(int _r, int _g, int _b, int _a){
	ofColor newColor(_r,_g,_b,_a);
	if(newColor!=currentStyle.color){
        currentStyle.color = newColor;
//		if(currentShader){
//			currentShader->setUniform4f(COLOR_UNIFORM,_r/255.,_g/255.,_b/255.,_a/255.);
//		}
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setColor(int gray){
	setColor(gray, gray, gray);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setHexColor(int hexColor){
	int r = (hexColor >> 16) & 0xff;
	int g = (hexColor >> 8) & 0xff;
	int b = (hexColor >> 0) & 0xff;
	setColor(r,g,b);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setBitmapTextMode(ofDrawBitmapMode mode){
	currentStyle.drawBitmapMode = mode;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::clear(){
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::clear(float r, float g, float b, float a) {
	//glClearColor(r / 255., g / 255., b / 255., a / 255.);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::clear(float brightness, float a) {
	clear(brightness, brightness, brightness, a);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::clearAlpha() {
//	glColorMask(0, 0, 0, 1);
//	glClearColor(0, 0, 0, 1);
//	glClear(GL_COLOR_BUFFER_BIT);
//	glColorMask(1, 1, 1, 1);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setBackgroundAuto(bool bAuto){
	bBackgroundAuto = bAuto;
}

//----------------------------------------------------------
bool ofMetalProgrammableRenderer::getBackgroundAuto(){
	return bBackgroundAuto;
}

//----------------------------------------------------------
ofColor ofMetalProgrammableRenderer::getBackgroundColor(){
	return currentStyle.bgColor;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setBackgroundColor(const ofColor & c){
	currentStyle.bgColor = c;
//	glClearColor(currentStyle.bgColor[0]/255., currentStyle.bgColor[1]/255., currentStyle.bgColor[2]/255., currentStyle.bgColor[3]/255.);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::background(const ofColor & c){
	setBackgroundColor(c);
//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::background(float brightness) {
	background(ofColor(brightness));
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::background(int hexColor, float _a){
	background ( (hexColor >> 16) & 0xff, (hexColor >> 8) & 0xff, (hexColor >> 0) & 0xff, _a);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::background(int r, int g, int b, int a){
	background(ofColor(r,g,b,a));
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setFillMode(ofFillFlag fill){
	currentStyle.bFill = (fill==OF_FILLED);
	if(currentStyle.bFill){
		path.setFilled(true);
		path.setStrokeWidth(0);
//		#ifndef TARGET_OPENGLES
//			// GLES does not support glPolygonMode
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		#endif
	}else{
		path.setFilled(false);
		path.setStrokeWidth(currentStyle.lineWidth);
//		#ifndef TARGET_OPENGLES
//			// GLES does not support glPolygonMode
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		#endif
	}
}

//----------------------------------------------------------
ofFillFlag ofMetalProgrammableRenderer::getFillMode(){
	if(currentStyle.bFill){
		return OF_FILLED;
	}else{
		return OF_OUTLINE;
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setRectMode(ofRectMode mode){
	currentStyle.rectMode = mode;
}

//----------------------------------------------------------
ofRectMode ofMetalProgrammableRenderer::getRectMode(){
	return currentStyle.rectMode;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setLineWidth(float lineWidth){
	// tig: glLinewidth is 'kind of' deprecated.
	// http://www.opengl.org/registry/doc/glspec32.core.20090803.pdf
	// p.330: "LineWidth values greater than 1.0 will generate an
	// INVALID_VALUE error".
	// use geometry shaders to draw lines of varying thickness...

	currentStyle.lineWidth = lineWidth;
	if(!currentStyle.bFill){
		path.setStrokeWidth(lineWidth);
	}
	//glLineWidth(lineWidth);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setDepthTest(bool depthTest) {
//	if(depthTest) {
//		glEnable(GL_DEPTH_TEST);
//	} else {
//		glDisable(GL_DEPTH_TEST);
//	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setLineSmoothing(bool smooth){
	currentStyle.smoothing = smooth;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::startSmoothing(){
    // TODO :: needs ES2 code.
}

//----------ofMetalProgrammableRenderer-----------------------------------------------
void ofMetalProgrammableRenderer::endSmoothing(){
    // TODO :: needs ES2 code.
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setBlendMode(ofBlendMode blendMode){
//	switch (blendMode){
//		case OF_BLENDMODE_DISABLED:
//			glDisable(GL_BLEND);
//			break;
//
//		case OF_BLENDMODE_ALPHA:
//			glEnable(GL_BLEND);
//			glBlendEquation(GL_FUNC_ADD);
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			break;
//
//		case OF_BLENDMODE_ADD:
//			glEnable(GL_BLEND);
//			glBlendEquation(GL_FUNC_ADD);
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//			break;
//
//		case OF_BLENDMODE_MULTIPLY:
//			glEnable(GL_BLEND);
//			glBlendEquation(GL_FUNC_ADD);
//			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA /* GL_ZERO or GL_ONE_MINUS_SRC_ALPHA */);
//			break;
//
//		case OF_BLENDMODE_SCREEN:
//			glEnable(GL_BLEND);
//			glBlendEquation(GL_FUNC_ADD);
//			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
//			break;
//
//		case OF_BLENDMODE_SUBTRACT:
//			glEnable(GL_BLEND);
//			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//			break;
//
//		default:
//			break;
//	}
	currentStyle.blendingMode = blendMode;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::enablePointSprites(){
//#ifdef TARGET_OPENGLES
//	#ifndef TARGET_PROGRAMMABLE_GL
//		glEnable(GL_POINT_SPRITE_OES);
//	#endif
//#else
//	glEnable(GL_PROGRAM_POINT_SIZE);
//#endif
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::disablePointSprites(){
//#ifdef TARGET_OPENGLES
//	#ifndef TARGET_PROGRAMMABLE_GL
//		glEnable(GL_POINT_SPRITE_OES);
//	#endif
//#else
//	glDisable(GL_PROGRAM_POINT_SIZE);
//#endif
}


//----------------------------------------------------------
void ofMetalProgrammableRenderer::enableAntiAliasing(){
//#if !defined(TARGET_PROGRAMMABLE_GL) || !defined(TARGET_OPENGLES)
//	glEnable(GL_MULTISAMPLE);
//#endif
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::disableAntiAliasing(){
//#if !defined(TARGET_PROGRAMMABLE_GL) || !defined(TARGET_OPENGLES)
//	glDisable(GL_MULTISAMPLE);
//#endif
}

//----------------------------------------------------------
const ofShader & ofMetalProgrammableRenderer::getCurrentShader() const{
	return *currentShader;
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::setAlphaBitmapText(bool bitmapText){
	bool wasBitmapStringEnabled = bitmapStringEnabled;
	bitmapStringEnabled = bitmapText;

//	if(wasBitmapStringEnabled!=bitmapText){
//		if(currentShader) currentShader->setUniform1f(BITMAP_STRING_UNIFORM,bitmapText);
//	}
}

ofStyle ofMetalProgrammableRenderer::getStyle() const{
	return currentStyle;
}

void ofMetalProgrammableRenderer::pushStyle(){
	styleHistory.push_back(currentStyle);
	//if we are over the max number of styles we have set, then delete the oldest styles.
	if( styleHistory.size() > OF_MAX_STYLE_HISTORY ){
		styleHistory.pop_front();
		//should we warn here?
		ofLogWarning("ofGraphics") << "ofPushStyle(): maximum number of style pushes << " << OF_MAX_STYLE_HISTORY << " reached, did you forget to pop somewhere?";
	}
}

void ofMetalProgrammableRenderer::popStyle(){
	if( styleHistory.size() ){
		setStyle(styleHistory.back());
		styleHistory.pop_back();
	}
}

void ofMetalProgrammableRenderer::setStyle(const ofStyle & style){

	//color
	setColor((int)style.color.r, (int)style.color.g, (int)style.color.b, (int)style.color.a);

	//bg color
	setBackgroundColor(style.bgColor);

	//circle resolution - don't worry it only recalculates the display list if the res has changed
	setCircleResolution(style.circleResolution);

	setCurveResolution(style.curveResolution);

	//line width - finally!
	setLineWidth(style.lineWidth);

	//ofSetDepthTest(style.depthTest); removed since it'll break old projects setting depth test through glEnable

	//rect mode: corner/center
	setRectMode(style.rectMode);

	//poly mode: winding type
	setPolyMode(style.polyMode);

	//fill
	if(style.bFill ){
		setFillMode(OF_FILLED);
	}else{
		setFillMode(OF_OUTLINE);
	}

	//smoothing
	/*if(style.smoothing ){
		enableSmoothing();
	}else{
		disableSmoothing();
	}*/

	//blending
	setBlendMode(style.blendingMode);

	currentStyle = style;
}

void ofMetalProgrammableRenderer::setCurveResolution(int resolution){
	currentStyle.curveResolution = resolution;
	path.setCurveResolution(resolution);
}




//----------------------------------------------------------
void ofMetalProgrammableRenderer::disableTextureTarget(int textureTarget, int textureLocation){
	bool wasUsingTexture = texCoordsEnabled & (currentTextureTarget!=OF_NO_TEXTURE);
	currentTextureTarget = OF_NO_TEXTURE;

	if(!uniqueShader || currentMaterial){
		beginDefaultShader();
	}

//	bool usingTexture = texCoordsEnabled & (currentTextureTarget!=OF_NO_TEXTURE);
//	if(wasUsingTexture!=usingTexture){
//		if(currentShader) currentShader->setUniform1f(USE_TEXTURE_UNIFORM,usingTexture);
//	}
//	glActiveTexture(GL_TEXTURE0+textureLocation);
//	glBindTexture(textureTarget, 0);
//	glActiveTexture(GL_TEXTURE0);
}



//----------------------------------------------------------
void ofMetalProgrammableRenderer::bind(const ofShader & shader){
    if(currentShader && *currentShader==shader){
		return;
    }
	//glUseProgram(shader.getProgram());

	currentShader = &shader;
	uploadMatrices();
	setDefaultUniforms();
	if(!settingDefaultShader){
		usingCustomShader = true;
	}
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::unbind(const ofShader & shader){
	//glUseProgram(0);
	usingCustomShader = false;
	beginDefaultShader();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::begin(const ofFbo & fbo, ofFboMode mode){
	pushView();
    pushStyle();
    if(mode & OF_FBOMODE_MATRIXFLIP){
        matrixStack.setRenderSurface(fbo);
    }else{
        matrixStack.setRenderSurfaceNoMatrixFlip(fbo);
    }
	viewport();
    if(mode & OF_FBOMODE_PERSPECTIVE){
		setupScreenPerspective();
	}else{
		uploadMatrices();
	}
	bind(fbo);
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::end(const ofFbo & fbo){
	unbind(fbo);
	matrixStack.setRenderSurface(*window);
	uploadMatrices();
	popStyle();
	popView();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::bind(const ofFbo & fbo){
	if (currentFramebufferId == fbo.getId()){
		ofLogWarning() << "Framebuffer with id: " << fbo.getId() << " cannot be bound onto itself. \n" <<
			"Most probably you forgot to end() the current framebuffer before calling begin() again or you forgot to allocate() before calling begin().";
		return;
	}
	// this method could just as well have been placed in ofBaseGLRenderer
	// and shared over both programmable and fixed function renderer.
	// I'm keeping it here, so that if we want to do more fancyful
	// named framebuffers with GL 4.5+, we can have 
	// different implementations.
	framebufferIdStack.push_back(currentFramebufferId);
	currentFramebufferId = fbo.getId();
	//glBindFramebuffer(GL_FRAMEBUFFER, currentFramebufferId);
}



//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawLine(float x1, float y1, float z1, float x2, float y2, float z2) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	lineMesh.getVertices()[0] = {x1,y1,z1};
	lineMesh.getVertices()[1] = {x2,y2,z2};
    
	// use smoothness, if requested:
	if (currentStyle.smoothing) mutThis->startSmoothing();
    
	draw(lineMesh,OF_MESH_FILL,false,false,false);
    
	// use smoothness, if requested:
	if (currentStyle.smoothing) mutThis->endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawRectangle(float x, float y, float z, float w, float h) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	if (currentStyle.rectMode == OF_RECTMODE_CORNER){
		rectMesh.getVertices()[0] = {x,y,z};
		rectMesh.getVertices()[1] = {x+w, y, z};
		rectMesh.getVertices()[2] = {x+w, y+h, z};
		rectMesh.getVertices()[3] = {x, y+h, z};
	}else{
		rectMesh.getVertices()[0] = {x-w/2.0f, y-h/2.0f, z};
		rectMesh.getVertices()[1] = {x+w/2.0f, y-h/2.0f, z};
		rectMesh.getVertices()[2] = {x+w/2.0f, y+h/2.0f, z};
		rectMesh.getVertices()[3] = {x-w/2.0f, y+h/2.0f, z};
	}
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->startSmoothing();

	rectMesh.setMode(currentStyle.bFill ? OF_PRIMITIVE_TRIANGLE_FAN : OF_PRIMITIVE_LINE_LOOP);
	draw(rectMesh,OF_MESH_FILL,false,false,false);
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	triangleMesh.getVertices()[0] = {x1,y1,z1};
	triangleMesh.getVertices()[1] = {x2,y2,z2};
	triangleMesh.getVertices()[2] = {x3,y3,z3};
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->startSmoothing();

	triangleMesh.setMode(currentStyle.bFill ? OF_PRIMITIVE_TRIANGLE_STRIP : OF_PRIMITIVE_LINE_LOOP);
	draw(triangleMesh,OF_MESH_FILL,false,false,false);
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawCircle(float x, float y, float z,  float radius) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	const auto & circleCache = circlePolyline.getVertices();
	for(int i=0;i<(int)circleCache.size();i++){
		circleMesh.getVertices()[i] = {radius*circleCache[i].x+x,radius*circleCache[i].y+y,z};
	}
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->startSmoothing();

	circleMesh.setMode(currentStyle.bFill ? OF_PRIMITIVE_TRIANGLE_FAN : OF_PRIMITIVE_LINE_STRIP);
	draw(circleMesh,OF_MESH_FILL,false,false,false);
	
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawEllipse(float x, float y, float z, float width, float height) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	float radiusX = width*0.5;
	float radiusY = height*0.5;
	const auto & circleCache = circlePolyline.getVertices();
	for(int i=0;i<(int)circleCache.size();i++){
		circleMesh.getVertices()[i] = {radiusX*circlePolyline[i].x+x,radiusY*circlePolyline[i].y+y,z};
	}
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->startSmoothing();

	circleMesh.setMode(currentStyle.bFill ? OF_PRIMITIVE_TRIANGLE_FAN : OF_PRIMITIVE_LINE_STRIP);
	draw(circleMesh,OF_MESH_FILL,false,false,false);
    
	// use smoothness, if requested:
	if (currentStyle.smoothing && !currentStyle.bFill) mutThis->endSmoothing();
}

//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawString(string textString, float x, float y, float z) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	float sx = 0;
	float sy = 0;

	///////////////////////////
	// APPLY TRANSFORM / VIEW
	///////////////////////////
	//

	bool hasModelView = false;
	bool hasProjection = false;
	bool hasViewport = false;

	ofRectangle rViewport;
	glm::mat4 modelView = glm::mat4(1.0);

	switch (currentStyle.drawBitmapMode) {

		case OF_BITMAPMODE_SIMPLE:

			sx += x;
			sy += y;
			break;

		case OF_BITMAPMODE_SCREEN:

			hasViewport = true;
			mutThis->pushView();

			rViewport = matrixStack.getFullSurfaceViewport();
			mutThis->viewport(rViewport);

			mutThis->matrixMode(OF_MATRIX_PROJECTION);
			mutThis->loadIdentityMatrix();
			mutThis->matrixMode(OF_MATRIX_MODELVIEW);

			modelView = glm::translate(modelView, glm::vec3(-1,-1,0));
			modelView = glm::scale(modelView, glm::vec3(2/rViewport.width, 2/rViewport.height, 1));
			modelView = glm::translate(modelView, glm::vec3(x,y, 0));
			mutThis->loadMatrix(modelView);
			break;

		case OF_BITMAPMODE_VIEWPORT:

			rViewport = getCurrentViewport();

			hasProjection = true;
			mutThis->matrixMode(OF_MATRIX_PROJECTION);
			mutThis->pushMatrix();
			mutThis->loadIdentityMatrix();

			hasModelView = true;
			mutThis->matrixMode(OF_MATRIX_MODELVIEW);
			mutThis->pushMatrix();

			modelView = glm::translate(modelView, glm::vec3(-1,-1,0));
			modelView = glm::scale(modelView, glm::vec3(2/rViewport.width, 2/rViewport.height, 1));
			modelView = glm::translate(modelView, glm::vec3(x,y, 0));
			mutThis->loadMatrix(modelView);
			break;

		case OF_BITMAPMODE_MODEL:

			hasModelView = true;
			mutThis->matrixMode(OF_MATRIX_MODELVIEW);
			mutThis->pushMatrix();

			mutThis->translate(x, y, z);
			break;

		case OF_BITMAPMODE_MODEL_BILLBOARD:
		{
			//our aim here is to draw to screen
			//at the viewport position related
			//to the world position x,y,z

			// tig: we want to get the signed normalised screen coordinates (-1,+1) of our point (x,y,z)
			// that's projection * modelview * point in GLSL multiplication order
			// then doing the good old (v + 1.0) / 2. to get unsigned normalized screen (0,1) coordinates.
			// we then multiply x by width and y by height to get window coordinates.
			
			rViewport = getCurrentViewport();
			
			glm::mat4 mat = matrixStack.getProjectionMatrixNoOrientation()  * matrixStack.getModelViewMatrix();
			glm::vec4 dScreen4 = mat * glm::vec4(x,y,z,1.0);
			glm::vec3 dScreen = glm::vec3(dScreen4) / dScreen4.w;
			dScreen += glm::vec3(1.0) ;
			dScreen *= 0.5;
			
			dScreen.x += rViewport.x;
			dScreen.x *= rViewport.width;

			dScreen.y += rViewport.y;
			dScreen.y *= rViewport.height;
			
			if (dScreen.z >= 1) return;


			hasProjection = true;
			mutThis->matrixMode(OF_MATRIX_PROJECTION);
			mutThis->pushMatrix();
			mutThis->loadIdentityMatrix();

			hasModelView = true;
			mutThis->matrixMode(OF_MATRIX_MODELVIEW);
			mutThis->pushMatrix();

			modelView = glm::translate(modelView, glm::vec3(-1,-1,0));
			modelView = glm::scale(modelView, glm::vec3(2/rViewport.width, 2/rViewport.height, 1));
			modelView = glm::translate(modelView, glm::vec3(dScreen.x, dScreen.y, 0));
			mutThis->loadMatrix(modelView);
		}
			break;

		default:
			break;
	}
	//
	///////////////////////////

	// tig: we switch over to our built-in bitmapstring shader
	// to render text. This gives us more flexibility & control
	// and does not mess/interfere with client side shaders.

	// (c) enable texture once before we start drawing each char (no point turning it on and off constantly)
	//We do this because its way faster
	mutThis->setAlphaBitmapText(true);
	ofMesh charMesh = bitmapFont.getMesh(textString, sx, sy, currentStyle.drawBitmapMode, isVFlipped());
	mutThis->bind(bitmapFont.getTexture(),0);
	draw(charMesh,OF_MESH_FILL,false,true,false);
	mutThis->unbind(bitmapFont.getTexture(),0);
	mutThis->setAlphaBitmapText(false);


	if (hasViewport){
		mutThis->popView();
	}else{
		if (hasModelView){
			mutThis->popMatrix();
		}

		if (hasProjection)
		{
			mutThis->matrixMode(OF_MATRIX_PROJECTION);
			mutThis->popMatrix();
			mutThis->matrixMode(OF_MATRIX_MODELVIEW);
		}
	}
}


//----------------------------------------------------------
void ofMetalProgrammableRenderer::drawString(const ofTrueTypeFont & font, string text, float x, float y) const{
	ofMetalProgrammableRenderer * mutThis = const_cast<ofMetalProgrammableRenderer*>(this);
	ofBlendMode blendMode = currentStyle.blendingMode;

	mutThis->setBlendMode(OF_BLENDMODE_ALPHA);

	mutThis->bind(font.getFontTexture(),0);
	draw(font.getStringMesh(text,x,y,isVFlipped()),OF_MESH_FILL);
	mutThis->unbind(font.getFontTexture(),0);

	mutThis->setBlendMode(blendMode);
}



void ofMetalProgrammableRenderer::saveFullViewport(ofPixels & pixels){
	ofRectangle v = getCurrentViewport();
	saveScreen(v.x,v.y,v.width,v.height,pixels);
}

ofPath & ofMetalProgrammableRenderer::getPath(){
    return path;
}

