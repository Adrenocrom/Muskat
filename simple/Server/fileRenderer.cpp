/***********************************************************
 *
 *
 *						FILEREADER SOURCE
 *					 =======================
 *
 *		AUTHOR: Josef Schulz
 *
 * 		
 *	 
 ***********************************************************/

#include "muskat.h"

// fills the frame buffer, the frames is constructed or in this case seleced
// with the frame info datastructure
void FileRenderer::getFrame(FrameInfo& info, FrameBuffer& fb) {
	fb = m_pScene->m_fbs[info.id];
}

// sets the scene, or could be loaded here
void FileRenderer::setScene(Scene* pScene) {
	m_pScene = pScene;
}
